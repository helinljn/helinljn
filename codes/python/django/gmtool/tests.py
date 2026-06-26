import json
import tempfile
from pathlib import Path
from unittest.mock import patch
from urllib.parse import urlencode

from django.contrib.auth import get_user_model
from django.contrib.auth.models import AnonymousUser
from django.core.files.uploadedfile import SimpleUploadedFile
from django.db import DatabaseError, IntegrityError, connection
from django.http import JsonResponse
from django.test import RequestFactory, TestCase, TransactionTestCase, override_settings
from django.urls import reverse

from .announcement_client import create_announcement, delete_announcement, query_announcements
from .announcement_review_services import approve_announcement_review
from .command_parser import _enrich_schema_fields, sync_commands_to_db, validate_json_command_ids
from .command_review_services import approve_command_review, get_command_review_type, has_command_review_type_permission
from .command_services import validate_command_params_basic
from .context_processors import gmtool_permissions
from .decorators import announcement_permission_required, has_announcement_permission
from .forms import AnnouncementCreateForm, AnnouncementDeleteForm, AnnouncementQueryForm
from .idip_client import send_idip_command
from .models import (
    AnnouncementLog,
    AnnouncementReview,
    CommandLog,
    CommandReview,
    GMCommand,
    UserAnnouncementPermission,
    UserCommandPermission,
    UserProfile,
)
from .security_utils import get_client_ip


class IdipClientEncodingTests(TestCase):
    def test_content_is_not_pre_url_encoded_before_requests_form_encoding(self):
        command = GMCommand(
            command_id='10001',
            command_name='测试命令',
            tab='测试',
            request_name='TestReq',
            request_id=4100,
            response_name='TestRsp',
            response_id=4101,
        )
        captured = {}

        class FakeResponse:
            def raise_for_status(self):
                return None

            def json(self):
                return {'status': True, 'id': 4100, 'json': {'Result': 0, 'RetMsg': 'OK'}}

        class FakeRequests:
            class Timeout(Exception):
                pass

            class ConnectionError(Exception):
                pass

            JSONDecodeError = json.JSONDecodeError

            def post(self, url, data, timeout):
                captured['url'] = url
                captured['data'] = data
                captured['timeout'] = timeout
                return FakeResponse()

        with patch('gmtool.idip_client._get_requests', return_value=FakeRequests()):
            response_data, error_message, request_content, error_type = send_idip_command(
                command,
                {
                    'AreaId': 10,
                    'Partition': 1,
                    'PlatId': 1,
                    'RoleName': '测试角色',
                },
            )

        self.assertIsNone(error_message)
        self.assertEqual(error_type, '')
        self.assertTrue(response_data['status'])

        content = captured['data']['content']
        self.assertTrue(content.startswith('{'))
        self.assertNotIn('%7B', content)
        content_payload = json.loads(content)
        self.assertEqual(
            content_payload,
            {
                'AreaId': 10,
                'Partition': 1,
                'PlatId': 1,
                'RoleName': '测试角色',
            },
        )
        self.assertNotIn('head', content_payload)
        self.assertNotIn('body', content_payload)
        self.assertEqual(json.loads(request_content)['form_data']['content'], content)


class AnnouncementClientTests(TestCase):
    @override_settings(ANNOUNCEMENT_BASE_URL='http://example.com/game', ANNOUNCEMENT_TIMEOUT=3)
    def test_create_announcement_posts_form_data_and_accepts_ok(self):
        captured = {}

        class FakeResponse:
            status_code = 200
            text = ' OK \n'

        class FakeRequests:
            class Timeout(Exception):
                pass

            def post(self, url, data, timeout):
                captured['url'] = url
                captured['data'] = data
                captured['timeout'] = timeout
                return FakeResponse()

        payload = {'Platform': 'Android', 'Channel': '小米', 'AnnouncementType': '1'}
        with patch('gmtool.announcement_client._get_requests', return_value=FakeRequests()):
            response_data, error_message, raw_response, error_type = create_announcement(payload)

        self.assertEqual(response_data, {'result': 'OK'})
        self.assertEqual(error_message, '')
        self.assertEqual(raw_response, ' OK \n')
        self.assertEqual(error_type, '')
        self.assertEqual(captured['url'], 'http://example.com/game/server/announcementSave.php')
        self.assertEqual(captured['data'], payload)
        self.assertEqual(captured['timeout'], 3)

    @override_settings(ANNOUNCEMENT_BASE_URL='http://example.com')
    def test_delete_announcement_posts_form_data(self):
        captured = {}

        class FakeResponse:
            status_code = 200
            text = 'OK'

        class FakeRequests:
            class Timeout(Exception):
                pass

            def post(self, url, data, timeout):
                captured['url'] = url
                captured['data'] = data
                return FakeResponse()

        with patch('gmtool.announcement_client._get_requests', return_value=FakeRequests()):
            response_data, error_message, _raw_response, error_type = delete_announcement(
                'Android',
                'VIVO',
                '2',
                171869000000000,
            )

        self.assertEqual(response_data, {'result': 'OK'})
        self.assertEqual(error_message, '')
        self.assertEqual(error_type, '')
        self.assertEqual(captured['url'], 'http://example.com/server/announcementDelete.php')
        self.assertEqual(captured['data']['AnnouncementId'], '171869000000000')
        self.assertEqual(captured['data']['AnnouncementType'], '2')

    @override_settings(ANNOUNCEMENT_BASE_URL='https://example.com')
    def test_query_announcements_uses_get_query_params(self):
        captured = {}

        class FakeResponse:
            status_code = 200
            text = '[{"AnnouncementId": "1"}]'

            def json(self):
                return [{'AnnouncementId': '1'}]

        class FakeRequests:
            class Timeout(Exception):
                pass

            def get(self, url, params, timeout):
                captured['url'] = url
                captured['params'] = params
                return FakeResponse()

        with patch('gmtool.announcement_client._get_requests', return_value=FakeRequests()):
            response_data, error_message, raw_response, error_type = query_announcements('IOS', 'OPPO')

        self.assertEqual(response_data, [{'AnnouncementId': '1'}])
        self.assertEqual(error_message, '')
        self.assertEqual(raw_response, '[{"AnnouncementId": "1"}]')
        self.assertEqual(error_type, '')
        self.assertEqual(captured['url'], 'https://example.com/client/announcement.php')
        self.assertEqual(captured['params'], {
            'Platform': 'IOS',
            'Channel': 'OPPO',
        })

    @override_settings(ANNOUNCEMENT_BASE_URL='http://example.com')
    def test_create_announcement_treats_invalid_or_fail_as_failed(self):
        class FakeResponse:
            status_code = 200
            text = 'Invalid: Platform'

        class FakeRequests:
            class Timeout(Exception):
                pass

            def post(self, url, data, timeout):
                return FakeResponse()

        with patch('gmtool.announcement_client._get_requests', return_value=FakeRequests()):
            response_data, error_message, raw_response, error_type = create_announcement({})

        self.assertIsNone(response_data)
        self.assertEqual(error_message, 'Invalid: Platform')
        self.assertEqual(raw_response, 'Invalid: Platform')
        self.assertEqual(error_type, 'failed')

    @override_settings(ANNOUNCEMENT_BASE_URL='http://example.com')
    def test_create_announcement_treats_fail_prefix_as_failed(self):
        class FakeResponse:
            status_code = 200
            text = 'FAIL: busy'

        class FakeRequests:
            class Timeout(Exception):
                pass

            def post(self, url, data, timeout):
                return FakeResponse()

        with patch('gmtool.announcement_client._get_requests', return_value=FakeRequests()):
            response_data, error_message, raw_response, error_type = create_announcement({})

        self.assertIsNone(response_data)
        self.assertEqual(error_message, 'FAIL: busy')
        self.assertEqual(raw_response, 'FAIL: busy')
        self.assertEqual(error_type, 'failed')

    @override_settings(ANNOUNCEMENT_BASE_URL='http://example.com')
    def test_create_announcement_accepts_ok_after_php_warning_noise(self):
        """远端在 OK 之前掺入 PHP/MySQL Warning 时，应取末行结果判定成功。"""
        noisy_text = (
            '<br />\n'
            '<b>Warning</b>:  Packets out of order. Expected 1 received 0. '
            'Packet size=145 in <b>/data1/DirServerNew/webroot20008/common/'
            'announcementBase.php</b> on line <b>230</b><br />\n'
            'OK'
        )

        class FakeResponse:
            status_code = 200
            text = noisy_text

        class FakeRequests:
            class Timeout(Exception):
                pass

            def post(self, url, data, timeout):
                return FakeResponse()

        with patch('gmtool.announcement_client._get_requests', return_value=FakeRequests()):
            response_data, error_message, raw_response, error_type = create_announcement({})

        self.assertEqual(response_data, {'result': 'OK'})
        self.assertEqual(error_message, '')
        self.assertEqual(raw_response, noisy_text)
        self.assertEqual(error_type, '')

    @override_settings(ANNOUNCEMENT_BASE_URL='http://example.com')
    def test_delete_announcement_accepts_ok_after_php_warning_noise(self):
        """删除接口同样需要在 Warning 噪音后识别末行 OK。"""
        noisy_text = (
            '<br />\n'
            '<b>Warning</b>:  Packets out of order. Expected 1 received 0. '
            'Packet size=145 in <b>/data1/DirServerNew/webroot20008/common/'
            'announcementBase.php</b> on line <b>230</b><br />\n'
            'OK'
        )

        class FakeResponse:
            status_code = 200
            text = noisy_text

        class FakeRequests:
            class Timeout(Exception):
                pass

            def post(self, url, data, timeout):
                return FakeResponse()

        with patch('gmtool.announcement_client._get_requests', return_value=FakeRequests()):
            response_data, error_message, raw_response, error_type = delete_announcement(
                'Android',
                'VIVO',
                '2',
                123,
            )

        self.assertEqual(response_data, {'result': 'OK'})
        self.assertEqual(error_message, '')
        self.assertEqual(error_type, '')

    @override_settings(ANNOUNCEMENT_BASE_URL='http://example.com')
    def test_post_text_keeps_fail_result_after_warning_noise(self):
        """结果行为 FAIL 时即便前面有 Warning 也应判定失败。"""
        noisy_text = (
            '<br />\n'
            '<b>Warning</b>:  Packets out of order in <b>announcementBase.php</b><br />\n'
            'FAIL: busy'
        )

        class FakeResponse:
            status_code = 200
            text = noisy_text

        class FakeRequests:
            class Timeout(Exception):
                pass

            def post(self, url, data, timeout):
                return FakeResponse()

        with patch('gmtool.announcement_client._get_requests', return_value=FakeRequests()):
            response_data, error_message, raw_response, error_type = create_announcement({})

        self.assertIsNone(response_data)
        self.assertEqual(error_message, 'FAIL: busy')
        self.assertEqual(error_type, 'failed')

    @override_settings(ANNOUNCEMENT_BASE_URL='http://example.com')
    def test_query_announcements_rejects_non_array_json(self):
        class FakeResponse:
            status_code = 200
            text = '{"ok": true}'

            def json(self):
                return {'ok': True}

        class FakeRequests:
            class Timeout(Exception):
                pass

            def get(self, url, params, timeout):
                return FakeResponse()

        with patch('gmtool.announcement_client._get_requests', return_value=FakeRequests()):
            response_data, error_message, _raw_response, error_type = query_announcements('Android', '小米')

        self.assertIsNone(response_data)
        self.assertIn('JSON 数组', error_message)
        self.assertEqual(error_type, 'failed')

    @override_settings(ANNOUNCEMENT_BASE_URL='http://example.com')
    def test_query_announcements_rejects_non_object_items(self):
        class FakeResponse:
            status_code = 200
            text = '[null]'

            def json(self):
                return [None]

        class FakeRequests:
            class Timeout(Exception):
                pass

            def get(self, url, params, timeout):
                return FakeResponse()

        with patch('gmtool.announcement_client._get_requests', return_value=FakeRequests()):
            response_data, error_message, _raw_response, error_type = query_announcements('Android', '小米')

        self.assertIsNone(response_data)
        self.assertIn('数组元素必须是对象', error_message)
        self.assertEqual(error_type, 'failed')

    @override_settings(ANNOUNCEMENT_BASE_URL='http://example.com')
    def test_http_error_is_failed_and_keeps_raw_response(self):
        class FakeResponse:
            status_code = 500
            text = 'server error'

        class FakeRequests:
            class Timeout(Exception):
                pass

            def post(self, url, data, timeout):
                return FakeResponse()

        with patch('gmtool.announcement_client._get_requests', return_value=FakeRequests()):
            response_data, error_message, raw_response, error_type = create_announcement({})

        self.assertIsNone(response_data)
        self.assertIn('500', error_message)
        self.assertEqual(raw_response, 'server error')
        self.assertEqual(error_type, 'failed')

    @override_settings(ANNOUNCEMENT_BASE_URL='http://example.com')
    def test_timeout_is_classified_as_timeout(self):
        class FakeRequests:
            class Timeout(Exception):
                pass

            def post(self, url, data, timeout):
                raise self.Timeout('slow')

        with patch('gmtool.announcement_client._get_requests', return_value=FakeRequests()):
            response_data, error_message, _raw_response, error_type = create_announcement({})

        self.assertIsNone(response_data)
        self.assertIn('超时', error_message)
        self.assertEqual(error_type, 'timeout')

    @override_settings(ANNOUNCEMENT_BASE_URL='http://example.com')
    def test_create_timeout_log_uses_payload_summary(self):
        class FakeRequests:
            class Timeout(Exception):
                pass

            def post(self, url, data, timeout):
                raise self.Timeout('slow')

        payload = {
            'Platform': 'Android',
            'Channel': '小米',
            'AnnouncementType': '1',
            'AnnouncementId': '-1',
            'Title': '标题',
            'Content': 'secret body',
        }
        with patch('gmtool.announcement_client._get_requests', return_value=FakeRequests()):
            with self.assertLogs('gmtool.announcement_client', level='WARNING') as captured:
                create_announcement(payload)

        joined_logs = '\n'.join(captured.output)
        self.assertIn('Android', joined_logs)
        self.assertNotIn('secret body', joined_logs)
        self.assertNotIn('Title', joined_logs)
        self.assertNotIn('Content', joined_logs)

    @override_settings(ANNOUNCEMENT_BASE_URL='')
    def test_empty_base_url_is_configuration_error(self):
        response_data, error_message, raw_response, error_type = create_announcement({})

        self.assertIsNone(response_data)
        self.assertIn('未配置', error_message)
        self.assertEqual(raw_response, '')
        self.assertEqual(error_type, 'failed')

    @override_settings(ANNOUNCEMENT_BASE_URL='ftp://example.com')
    def test_invalid_base_url_scheme_is_configuration_error(self):
        response_data, error_message, _raw_response, error_type = query_announcements('Android', '小米')

        self.assertIsNone(response_data)
        self.assertIn('HTTP/HTTPS', error_message)
        self.assertEqual(error_type, 'failed')

    def test_invalid_base_url_shape_is_configuration_error(self):
        cases = [
            ('http:///bad', '有效的 HTTP/HTTPS 地址'),
            ('http://example.com?x=1', '不能包含 query string 或 fragment'),
            ('http://example.com#frag', '不能包含 query string 或 fragment'),
        ]

        for base_url, expected_message in cases:
            with self.subTest(base_url=base_url):
                with override_settings(ANNOUNCEMENT_BASE_URL=base_url):
                    response_data, error_message, raw_response, error_type = query_announcements(
                        'Android',
                        '小米',
                    )

                self.assertIsNone(response_data)
                self.assertIn(expected_message, error_message)
                self.assertEqual(raw_response, '')
                self.assertEqual(error_type, 'failed')

    @override_settings(ANNOUNCEMENT_BASE_URL='http://example.com')
    def test_network_exception_is_failed_without_raising(self):
        class FakeRequests:
            class Timeout(Exception):
                pass

            def get(self, url, params, timeout):
                raise RuntimeError('connection refused')

        with patch('gmtool.announcement_client._get_requests', return_value=FakeRequests()):
            response_data, error_message, raw_response, error_type = query_announcements('Android', '小米')

        self.assertIsNone(response_data)
        self.assertIn('connection refused', error_message)
        self.assertEqual(raw_response, '')
        self.assertEqual(error_type, 'failed')


class AnnouncementConfigHelperTests(TestCase):
    def test_csv_unique_config_strips_empty_values_and_keeps_first_duplicate(self):
        from mysite.settings import csv_unique_config

        with patch('mysite.settings.config', return_value=' Android, Android,IOS,,IOS,小米, 小米 '):
            self.assertEqual(csv_unique_config('ANNOUNCEMENT_PLATFORMS'), ['Android', 'IOS', '小米'])


class AnnouncementPermissionTests(TestCase):
    def setUp(self):
        self.factory = RequestFactory()
        user_model = get_user_model()
        self.admin = user_model.objects.create_superuser(
            username='ann_admin',
            email='ann_admin@example.com',
            password='password123',
        )
        self.user = user_model.objects.create_user(
            username='ann_user',
            email='ann_user@example.com',
            password='password123',
        )

    def _decorated_response(self, request):
        @announcement_permission_required
        def sample_view(inner_request):
            return JsonResponse({'ok': True})

        return sample_view(request)

    def test_superuser_has_announcement_permission_without_row(self):
        self.assertTrue(has_announcement_permission(self.admin))
        self.assertFalse(UserAnnouncementPermission.objects.filter(user=self.admin).exists())

    def test_user_requires_explicit_announcement_permission(self):
        self.assertFalse(has_announcement_permission(self.user))
        UserAnnouncementPermission.objects.create(user=self.user)
        self.assertTrue(has_announcement_permission(self.user))

    def test_anonymous_post_gets_json_401(self):
        request = self.factory.post('/gmtool/announcements/create/')
        request.user = AnonymousUser()

        response = self._decorated_response(request)

        self.assertEqual(response.status_code, 401)
        self.assertEqual(json.loads(response.content)['error'], '请先登录')

    def test_unauthorized_user_post_gets_403(self):
        request = self.factory.post('/gmtool/announcements/create/')
        request.user = self.user

        response = self._decorated_response(request)

        self.assertEqual(response.status_code, 403)
        self.assertEqual(json.loads(response.content)['error'], '您没有公告管理权限')

    def test_authorized_user_can_access_decorated_view(self):
        UserAnnouncementPermission.objects.create(user=self.user)
        request = self.factory.get('/gmtool/announcements/')
        request.user = self.user

        response = self._decorated_response(request)

        self.assertEqual(response.status_code, 200)
        self.assertTrue(json.loads(response.content)['ok'])

    def test_context_processor_exposes_announcement_permission(self):
        request = self.factory.get('/gmtool/')
        request.user = self.admin

        context = gmtool_permissions(request)

        self.assertTrue(context['is_admin'])
        self.assertTrue(context['has_announcement_permission'])


class AnnouncementFormTests(TestCase):
    @override_settings(
        ANNOUNCEMENT_PLATFORMS=['Android', 'IOS'],
        ANNOUNCEMENT_CHANNELS=['小米', 'VIVO'],
    )
    def test_query_form_does_not_include_announcement_type(self):
        form = AnnouncementQueryForm(data={
            'Platform': 'Android',
            'Channel': ['小米'],
            'AnnouncementType': '1',
        })

        self.assertTrue(form.is_valid(), form.errors)
        self.assertEqual(list(form.fields), ['Platform', 'Channel'])
        self.assertNotIn('AnnouncementType', form.cleaned_data)

    @override_settings(
        ANNOUNCEMENT_PLATFORMS=['Android', 'IOS'],
        ANNOUNCEMENT_CHANNELS=['小米', 'VIVO'],
    )
    def test_create_form_builds_reserve_1_only_for_persistent_announcement(self):
        form = AnnouncementCreateForm(data={
            'Platform': 'Android',
            'Channel': ['小米'],
            'AnnouncementType': '2',
            'Title': '标题',
            'Content': '正文',
            'Priority': '5',
        })

        self.assertTrue(form.is_valid(), form.errors)
        payload = form.to_payload('小米')

        self.assertEqual(payload['AnnouncementId'], '-1')
        self.assertEqual(payload['Channel'], '小米')
        self.assertEqual(payload['Reserve_2'], '')
        reserve_1 = json.loads(payload['Reserve_1'])
        self.assertEqual(reserve_1, {'priority': 5})

    @override_settings(
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米'],
    )
    def test_create_form_preserves_content_outer_whitespace(self):
        form = AnnouncementCreateForm(data={
            'Platform': 'Android',
            'Channel': ['小米'],
            'AnnouncementType': '1',
            'Title': '标题',
            'Content': '\n  正文\n  ',
            'Priority': '0',
        })

        self.assertTrue(form.is_valid(), form.errors)
        self.assertEqual(form.to_payload('小米')['Content'], '\n  正文\n  ')

    @override_settings(
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米'],
    )
    def test_create_form_uses_empty_reserve_1_for_weekly_and_carousel(self):
        for announcement_type in ('1', '3'):
            data = {
                'Platform': 'Android',
                'Channel': ['小米'],
                'AnnouncementType': announcement_type,
                'Title': '标题',
                'Content': '正文',
                'Priority': '0',
            }
            if announcement_type == '3':
                data['Image_1'] = 'https://example.com/banner.png'
            form = AnnouncementCreateForm(data=data)
            self.assertTrue(form.is_valid(), form.errors)
            self.assertEqual(form.to_payload('小米')['Reserve_1'], '')

    @override_settings(
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米'],
    )
    def test_carousel_create_form_does_not_require_title_or_content(self):
        form = AnnouncementCreateForm(data={
            'Platform': 'Android',
            'Channel': ['小米'],
            'AnnouncementType': '3',
            'Priority': '-1',
            'Image_1': 'https://example.com/a.png',
            'ImageLink_1': 'https://example.com/a',
        })

        self.assertTrue(form.is_valid(), form.errors)
        payload = form.to_payload('小米')
        self.assertEqual(payload['Title'], '')
        self.assertEqual(payload['Content'], '')
        self.assertEqual(payload['Image_1'], 'https://example.com/a.png')
        self.assertEqual(payload['ImageLink_1'], 'https://example.com/a')
        self.assertEqual(payload['Reserve_1'], '')

    @override_settings(
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米'],
    )
    def test_non_carousel_create_form_clears_image_fields(self):
        for announcement_type in ('1', '2'):
            form = AnnouncementCreateForm(data={
                'Platform': 'Android',
                'Channel': ['小米'],
                'AnnouncementType': announcement_type,
                'Title': '标题',
                'Content': '正文',
                'Priority': '3',
                'Image_1': 'https://example.com/a.png',
                'ImageLink_1': 'https://example.com/a',
                'Image_2': 'https://example.com/b.png',
                'ImageLink_2': 'https://example.com/b',
                'Image_3': 'https://example.com/c.png',
                'ImageLink_3': 'https://example.com/c',
            })

            self.assertTrue(form.is_valid(), form.errors)
            payload = form.to_payload('小米')
            self.assertEqual(payload['Image_1'], '')
            self.assertEqual(payload['ImageLink_1'], '')
            self.assertEqual(payload['Image_2'], '')
            self.assertEqual(payload['ImageLink_2'], '')
            self.assertEqual(payload['Image_3'], '')
            self.assertEqual(payload['ImageLink_3'], '')

    @override_settings(
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米'],
    )
    def test_persistent_create_form_rejects_negative_priority(self):
        form = AnnouncementCreateForm(data={
            'Platform': 'Android',
            'Channel': ['小米'],
            'AnnouncementType': '2',
            'Title': '标题',
            'Content': '正文',
            'Priority': '-1',
        })

        self.assertFalse(form.is_valid())
        self.assertIn('Priority', form.errors)

    @override_settings(
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米'],
    )
    def test_weekly_create_form_requires_title_and_content(self):
        form = AnnouncementCreateForm(data={
            'Platform': 'Android',
            'Channel': ['小米'],
            'AnnouncementType': '1',
            'Priority': '0',
        })

        self.assertFalse(form.is_valid())
        self.assertIn('Title', form.errors)
        self.assertIn('Content', form.errors)

    @override_settings(
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米'],
    )
    def test_carousel_create_form_requires_image_1(self):
        form = AnnouncementCreateForm(data={
            'Platform': 'Android',
            'Channel': ['小米'],
            'AnnouncementType': '3',
            'Priority': '0',
        })

        self.assertFalse(form.is_valid())
        self.assertIn('Image_1', form.errors)

    @override_settings(
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米'],
    )
    def test_delete_form_rejects_invalid_channel(self):
        form = AnnouncementDeleteForm(data={
            'Platform': 'Android',
            'Channel': '非法',
            'AnnouncementType': '1',
            'AnnouncementId': '123',
        })

        self.assertFalse(form.is_valid())
        self.assertIn('Channel', form.errors)


class AnnouncementViewTests(TestCase):
    def setUp(self):
        user_model = get_user_model()
        self.admin = user_model.objects.create_superuser(
            username='ann_view_admin',
            email='ann_view_admin@example.com',
            password='password123',
        )
        self.client.defaults['HTTP_HOST'] = '127.0.0.1'
        self.client.force_login(self.admin)

    def _valid_create_payload(self, announcement_type='2', channel='小米'):
        return {
            'Platform': 'Android',
            'Channel': channel,
            'AnnouncementType': announcement_type,
            'Title': '维护公告',
            'Content': '今晚维护',
            'Priority': '0',
            'Image_1': '',
            'ImageLink_1': '',
            'Image_2': '',
            'ImageLink_2': '',
            'Image_3': '',
            'ImageLink_3': '',
        }

    @override_settings(
        ANNOUNCEMENT_BASE_URL='http://example.com',
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米', 'VIVO'],
    )
    @patch('gmtool.announcement_views.query_announcements')
    def test_first_get_does_not_query_or_create_logs(self, mocked_query):
        response = self.client.get(reverse('gmtool:announcement_query'))

        self.assertEqual(response.status_code, 200)
        mocked_query.assert_not_called()
        self.assertEqual(AnnouncementLog.objects.count(), 0)

    def test_announcement_list_redirects_to_query_page(self):
        response = self.client.get(reverse('gmtool:announcement_list'), {
            'Platform': 'Android',
            'Channel': '小米',
            'AnnouncementType': '1',
        })

        self.assertEqual(response.status_code, 302)
        self.assertTrue(response['Location'].endswith(
            reverse('gmtool:announcement_query') + '?Platform=Android&Channel=%E5%B0%8F%E7%B1%B3',
        ))
        self.assertNotIn('AnnouncementType', response['Location'])

    @override_settings(
        ANNOUNCEMENT_BASE_URL='',
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米'],
    )
    @patch('gmtool.announcement_views.query_announcements')
    def test_first_get_shows_base_url_configuration_error(self, mocked_query):
        response = self.client.get(reverse('gmtool:announcement_query'))

        self.assertEqual(response.status_code, 200)
        mocked_query.assert_not_called()
        self.assertContains(response, '公告目录服配置不可用')
        self.assertContains(response, '公告目录服地址未配置')

    @override_settings(
        ANNOUNCEMENT_BASE_URL='',
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米'],
    )
    @patch('gmtool.announcement_views.query_announcements')
    def test_query_does_not_call_client_when_base_url_configuration_is_invalid(self, mocked_query):
        response = self.client.get(reverse('gmtool:announcement_query'), {
            'Platform': 'Android',
            'Channel': '小米',
        })

        self.assertEqual(response.status_code, 200)
        mocked_query.assert_not_called()
        self.assertNotContains(response, '没有任何游戏登录公告！！！')

    @override_settings(
        ANNOUNCEMENT_BASE_URL='http://example.com',
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米', 'VIVO'],
    )
    @patch('gmtool.announcement_views.query_announcements')
    def test_query_single_channel_calls_client_without_writing_logs(self, mocked_query):
        mocked_query.return_value = ([{'AnnouncementId': '1'}], '', '[]', '')

        response = self.client.get(reverse('gmtool:announcement_query'), {
            'Platform': 'Android',
            'Channel': '小米',
        })

        self.assertEqual(response.status_code, 200)
        mocked_query.assert_called_once_with('Android', '小米')
        self.assertEqual(AnnouncementLog.objects.count(), 0)
        self.assertNotContains(response, 'name="AnnouncementType"')

    @override_settings(
        ANNOUNCEMENT_BASE_URL='http://example.com',
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米'],
    )
    @patch('gmtool.announcement_views.query_announcements')
    def test_query_result_offers_inline_and_batch_delete(self, mocked_query):
        mocked_query.return_value = ([{
            'Platform': 'Android',
            'Channel': '小米',
            'AnnouncementType': '2',
            'AnnouncementId': '123',
            'Title': '完整标题',
            'Content': '完整正文',
        }], '', '[]', '')

        response = self.client.get(reverse('gmtool:announcement_query'), {
            'Platform': 'Android',
            'Channel': '小米',
        })

        self.assertEqual(response.status_code, 200)
        # 删除均走批量删除端点，不再跳转独立删除页
        self.assertContains(response, reverse('gmtool:announcement_batch_delete'))
        # 单条删除按钮携带该公告的 item 数据
        self.assertContains(response, 'ann-delete-one')
        self.assertContains(response, 'AnnouncementId=123')
        self.assertContains(response, 'AnnouncementType=2')
        # 批量删除多选 checkbox
        self.assertContains(response, 'name="items"')
        self.assertContains(response, 'query-announcement-detail-btn')
        self.assertContains(response, 'queryAnnouncementModal')
        self.assertNotContains(response, 'data-bs-toggle="collapse"')

    @override_settings(
        ANNOUNCEMENT_BASE_URL='http://example.com',
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米'],
    )
    @patch('gmtool.announcement_views.query_announcements')
    def test_query_detail_modal_filters_sections_by_announcement_type(self, mocked_query):
        mocked_query.return_value = ([
            {
                'Platform': 'Android',
                'Channel': '小米',
                'AnnouncementType': '1',
                'AnnouncementId': 'weekly-1',
                'Title': '周更标题',
                'Content': '周更正文',
                'Image_1': 'https://example.com/weekly.png',
                'Reserve_1': 'weekly reserve',
            },
            {
                'Platform': 'Android',
                'Channel': '小米',
                'AnnouncementType': '3',
                'AnnouncementId': 'carousel-1',
                'Title': '轮播标题',
                'Content': '轮播正文',
                'Image_1': 'https://example.com/banner.png',
                'ImageLink_1': 'https://example.com/open',
                'Reserve_1': 'carousel reserve',
            },
        ], '', '[]', '')

        response = self.client.get(reverse('gmtool:announcement_query'), {
            'Platform': 'Android',
            'Channel': '小米',
        })

        self.assertEqual(response.status_code, 200)
        self.assertContains(response, 'data-announcement-type="1"')
        self.assertContains(response, 'data-announcement-type="3"')
        content = response.content.decode()
        self.assertIn("if (announcementType === '1' || announcementType === '2')", content)
        self.assertIn('detailHtml = titleSection + contentSection;', content)
        self.assertIn("} else if (announcementType === '3') {", content)
        self.assertIn('detailHtml = imageSection;', content)
        self.assertIn('detailHtml = titleSection + imageSection + reserveSection + contentSection;', content)

    @override_settings(
        ANNOUNCEMENT_BASE_URL='http://example.com',
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米'],
    )
    def test_create_page_renders_form(self):
        create_response = self.client.get(reverse('gmtool:announcement_create'))

        self.assertEqual(create_response.status_code, 200)
        self.assertContains(create_response, '提交审核')

    @override_settings(
        ANNOUNCEMENT_BASE_URL='http://example.com',
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米', 'VIVO'],
    )
    @patch('gmtool.announcement_review_services.create_announcement')
    @patch('gmtool.announcement_review_services.query_announcements')
    def test_create_single_channel_submits_review_without_remote_call(self, mocked_query, mocked_create):
        response = self.client.post(
            reverse('gmtool:announcement_create'),
            data=self._valid_create_payload(channel='小米'),
        )

        self.assertEqual(response.status_code, 302)
        self.assertEqual(response['Location'], reverse('gmtool:review_announcement_list'))
        mocked_query.assert_not_called()
        mocked_create.assert_not_called()
        self.assertEqual(AnnouncementLog.objects.count(), 0)
        review = AnnouncementReview.objects.get()
        self.assertEqual(review.status, 'pending')
        self.assertEqual(review.platform, 'Android')
        self.assertEqual(review.channel, '小米')
        self.assertEqual(review.announcement_type, '2')
        self.assertEqual(review.title, '维护公告')
        self.assertEqual(review.payload['Channel'], '小米')

    @override_settings(
        ANNOUNCEMENT_BASE_URL='http://example.com',
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米'],
    )
    @patch('gmtool.announcement_review_services.create_announcement')
    def test_create_carousel_without_title_or_content_stores_blank_payload(self, mocked_create):
        response = self.client.post(reverse('gmtool:announcement_create'), data={
            'Platform': 'Android',
            'Channel': '小米',
            'AnnouncementType': '3',
            'Priority': '9',
            'Image_1': 'https://example.com/banner.png',
            'ImageLink_1': 'https://example.com/open',
            'Image_2': '',
            'ImageLink_2': '',
            'Image_3': '',
            'ImageLink_3': '',
        })

        self.assertEqual(response.status_code, 302)
        mocked_create.assert_not_called()
        review = AnnouncementReview.objects.get()
        self.assertEqual(review.title, '')
        self.assertEqual(review.content, '')
        self.assertEqual(review.image_1, 'https://example.com/banner.png')
        self.assertEqual(review.image_link_1, 'https://example.com/open')
        self.assertEqual(review.reserve_1, '')
        self.assertEqual(review.payload['Title'], '')
        self.assertEqual(review.payload['Content'], '')
        self.assertEqual(AnnouncementLog.objects.count(), 0)

    @override_settings(
        ANNOUNCEMENT_BASE_URL='http://example.com',
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米'],
    )
    @patch('gmtool.announcement_review_services.delete_announcement')
    @patch('gmtool.announcement_review_services.query_announcements')
    @patch('gmtool.announcement_review_services.create_announcement')
    def test_create_persistent_does_not_query_delete_or_publish(
        self,
        mocked_create,
        mocked_query,
        mocked_delete,
    ):
        response = self.client.post(
            reverse('gmtool:announcement_create'),
            data=self._valid_create_payload(announcement_type='2'),
        )

        self.assertEqual(response.status_code, 302)
        mocked_query.assert_not_called()
        mocked_delete.assert_not_called()
        mocked_create.assert_not_called()
        self.assertEqual(AnnouncementReview.objects.count(), 1)

    @override_settings(
        ANNOUNCEMENT_BASE_URL='http://example.com',
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米'],
    )
    @patch('gmtool.announcement_review_services.delete_announcement')
    @patch('gmtool.announcement_review_services.query_announcements')
    @patch('gmtool.announcement_review_services.create_announcement')
    def test_weekly_create_only_submits_pending_review(self, mocked_create, mocked_query, mocked_delete):
        response = self.client.post(
            reverse('gmtool:announcement_create'),
            data=self._valid_create_payload(announcement_type='1'),
        )

        self.assertEqual(response.status_code, 302)
        mocked_query.assert_not_called()
        mocked_delete.assert_not_called()
        mocked_create.assert_not_called()
        review = AnnouncementReview.objects.get()
        self.assertEqual(review.status, 'pending')
        self.assertEqual(review.announcement_type, '1')
        self.assertEqual(AnnouncementLog.objects.count(), 0)

    @override_settings(
        ANNOUNCEMENT_BASE_URL='http://example.com',
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米'],
    )
    def test_weekly_create_rejects_duplicate_pending_review(self):
        AnnouncementReview.objects.create(
            submitter=self.admin,
            submitter_username=self.admin.username,
            platform='Android',
            channel='小米',
            announcement_type='1',
            announcement_id='-1',
            title='旧待审',
            content='旧待审',
            payload={
                'Platform': 'Android',
                'Channel': '小米',
                'AnnouncementType': '1',
                'AnnouncementId': '-1',
            },
        )

        response = self.client.post(
            reverse('gmtool:announcement_create'),
            data=self._valid_create_payload(announcement_type='1'),
        )

        self.assertEqual(response.status_code, 400)
        self.assertContains(response, '已有待审核周更新公告', status_code=400)
        self.assertEqual(AnnouncementReview.objects.count(), 1)

    @override_settings(
        ANNOUNCEMENT_BASE_URL='http://example.com',
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米', 'VIVO'],
    )
    def test_create_multi_channel_submits_per_channel_review(self):
        payload = self._valid_create_payload()
        payload['Channel'] = ['小米', 'VIVO']
        response = self.client.post(
            reverse('gmtool:announcement_create'),
            data=payload,
        )

        self.assertEqual(response.status_code, 302)
        self.assertEqual(AnnouncementReview.objects.count(), 2)
        self.assertEqual(
            sorted(AnnouncementReview.objects.values_list('channel', flat=True)),
            ['VIVO', '小米'],
        )
        self.assertEqual(AnnouncementLog.objects.count(), 0)

    @override_settings(
        ANNOUNCEMENT_BASE_URL='http://example.com',
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米', 'VIVO'],
    )
    def test_create_multi_channel_duplicate_weekly_allows_partial_success(self):
        AnnouncementReview.objects.create(
            submitter=self.admin,
            submitter_username=self.admin.username,
            platform='Android',
            channel='VIVO',
            announcement_type='1',
            announcement_id='-1',
            title='旧待审',
            content='旧待审',
            payload={
                'Platform': 'Android',
                'Channel': 'VIVO',
                'AnnouncementType': '1',
                'AnnouncementId': '-1',
            },
        )
        payload = self._valid_create_payload(announcement_type='1')
        payload['Channel'] = ['小米', 'VIVO']

        response = self.client.post(
            reverse('gmtool:announcement_create'),
            data=payload,
        )

        self.assertEqual(response.status_code, 400)
        self.assertContains(response, '小米', status_code=400)
        self.assertContains(response, '已有待审核周更新公告', status_code=400)
        self.assertEqual(AnnouncementReview.objects.filter(status='pending').count(), 2)

    @override_settings(
        ANNOUNCEMENT_BASE_URL='http://example.com',
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米', 'VIVO'],
    )
    @patch('gmtool.announcement_views.log_operation')
    @patch('gmtool.announcement_views.delete_announcement')
    def test_single_delete_uses_announcement_own_channel_and_type(self, mocked_delete, mocked_audit):
        mocked_delete.return_value = ({'result': 'OK'}, '', 'OK', '')

        # 单条删除复用批量删除端点：item 为该公告的四元组 query string
        item = urlencode({
            'Platform': 'Android',
            'Channel': 'VIVO',
            'AnnouncementType': '3',
            'AnnouncementId': '99',
        })
        response = self.client.post(reverse('gmtool:announcement_batch_delete'), data={
            'items': [item],
            'Platform': 'Android',
            'Channel': '小米',
        })

        self.assertEqual(response.status_code, 302)
        mocked_delete.assert_called_once_with('Android', 'VIVO', '3', '99')
        log = AnnouncementLog.objects.get()
        self.assertEqual(log.channel, 'VIVO')
        self.assertEqual(log.announcement_type, '3')

    @override_settings(
        ANNOUNCEMENT_BASE_URL='http://example.com',
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米'],
    )
    @patch('gmtool.announcement_views.log_operation')
    @patch('gmtool.announcement_views.delete_announcement')
    def test_batch_delete_removes_each_selected_announcement(self, mocked_delete, mocked_audit):
        mocked_delete.return_value = ({'result': 'OK'}, '', 'OK', '')

        items = [
            urlencode({'Platform': 'Android', 'Channel': '小米',
                       'AnnouncementType': '2', 'AnnouncementId': str(i)})
            for i in (101, 102, 103)
        ]
        response = self.client.post(reverse('gmtool:announcement_batch_delete'), data={
            'items': items,
            'Platform': 'Android',
            'Channel': '小米',
        })

        self.assertEqual(response.status_code, 302)
        self.assertEqual(mocked_delete.call_count, 3)
        self.assertEqual(AnnouncementLog.objects.filter(action='delete', status='success').count(), 3)
        detail = mocked_audit.call_args.kwargs['detail']
        self.assertEqual(detail['success_count'], 3)
        self.assertEqual(detail['failed_count'], 0)

    @override_settings(
        ANNOUNCEMENT_BASE_URL='http://example.com',
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米'],
    )
    @patch('gmtool.announcement_views.log_operation')
    @patch('gmtool.announcement_views.delete_announcement')
    def test_batch_delete_continues_and_aggregates_on_partial_failure(self, mocked_delete, mocked_audit):
        # 第一条成功，第二条远端失败：应继续删除并汇总
        mocked_delete.side_effect = [
            ({'result': 'OK'}, '', 'OK', ''),
            (None, 'FAIL: busy', 'FAIL: busy', 'failed'),
        ]

        items = [
            urlencode({'Platform': 'Android', 'Channel': '小米',
                       'AnnouncementType': '2', 'AnnouncementId': '201'}),
            urlencode({'Platform': 'Android', 'Channel': '小米',
                       'AnnouncementType': '2', 'AnnouncementId': '202'}),
        ]
        response = self.client.post(reverse('gmtool:announcement_batch_delete'), data={
            'items': items,
            'Platform': 'Android',
            'Channel': '小米',
        })

        self.assertEqual(response.status_code, 302)
        self.assertEqual(mocked_delete.call_count, 2)
        detail = mocked_audit.call_args.kwargs['detail']
        self.assertEqual(detail['success_count'], 1)
        self.assertEqual(detail['failed_count'], 1)

    @override_settings(
        ANNOUNCEMENT_BASE_URL='http://example.com',
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米'],
        BATCH_EXECUTE_MAX_TARGETS=2,
    )
    @patch('gmtool.announcement_views.delete_announcement')
    def test_batch_delete_rejects_when_over_max_targets(self, mocked_delete):
        items = [
            urlencode({'Platform': 'Android', 'Channel': '小米',
                       'AnnouncementType': '2', 'AnnouncementId': str(i)})
            for i in (1, 2, 3)
        ]
        response = self.client.post(reverse('gmtool:announcement_batch_delete'), data={
            'items': items,
            'Platform': 'Android',
            'Channel': '小米',
        })

        self.assertEqual(response.status_code, 302)
        mocked_delete.assert_not_called()
        self.assertEqual(AnnouncementLog.objects.count(), 0)

    @override_settings(
        ANNOUNCEMENT_BASE_URL='http://example.com',
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米'],
    )
    @patch('gmtool.announcement_views.delete_announcement')
    def test_batch_delete_without_selection_does_nothing(self, mocked_delete):
        response = self.client.post(reverse('gmtool:announcement_batch_delete'), data={
            'Platform': 'Android',
            'Channel': '小米',
        })

        self.assertEqual(response.status_code, 302)
        mocked_delete.assert_not_called()
        self.assertEqual(AnnouncementLog.objects.count(), 0)

    @override_settings(
        ANNOUNCEMENT_BASE_URL='',
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米'],
    )
    def test_submit_review_does_not_require_announcement_base_url(self):
        response = self.client.post(
            reverse('gmtool:announcement_create'),
            data=self._valid_create_payload(),
        )

        self.assertEqual(response.status_code, 302)
        self.assertEqual(AnnouncementReview.objects.count(), 1)
        self.assertEqual(AnnouncementLog.objects.count(), 0)

    @override_settings(
        ANNOUNCEMENT_BASE_URL='http://example.com',
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米'],
        PAGE_SIZE=2,
    )
    def test_announcement_log_pagination_links_encode_filters(self):
        for index in range(3):
            AnnouncementLog.objects.create(
                user=self.admin,
                operator_username='a&b',
                action='create',
                platform='Android',
                channel='小米',
                announcement_type='1',
                announcement_id=str(index),
                request_data={'Title': f'公告 {index}'},
                response_data={'result': 'OK'},
                raw_response='OK',
                status='success',
                error_message='',
                ip_address='127.0.0.1',
            )

        response = self.client.get(reverse('gmtool:announcement_log_list'), {
            'user': 'a&b',
            'Channel': '小米',
        })

        self.assertEqual(response.status_code, 200)
        self.assertContains(response, 'user=a%26b')
        self.assertContains(response, 'Channel=%E5%B0%8F%E7%B1%B3')
        self.assertContains(response, 'announcement-log-detail-btn')
        self.assertContains(response, '/gmtool/api/v1/announcements/logs/')

    @override_settings(
        ANNOUNCEMENT_BASE_URL='http://example.com',
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米', 'VIVO'],
    )
    @patch('gmtool.announcement_views.query_announcements')
    def test_query_multi_channel_calls_client_per_channel(self, mocked_query):
        # 目录服不支持批量查询：每个渠道各调一次，结果分组返回
        mocked_query.side_effect = [
            ([{'AnnouncementId': '1', 'AnnouncementType': '2'}], '', '[]', ''),
            ([{'AnnouncementId': '2', 'AnnouncementType': '2'}], '', '[]', ''),
        ]

        response = self.client.get(reverse('gmtool:announcement_query'), {
            'Platform': 'Android',
            'Channel': ['小米', 'VIVO'],
        })

        self.assertEqual(response.status_code, 200)
        self.assertEqual(mocked_query.call_count, 2)
        mocked_query.assert_any_call('Android', '小米')
        mocked_query.assert_any_call('Android', 'VIVO')
        self.assertEqual(len(response.context['query_results']), 2)
        self.assertEqual(AnnouncementLog.objects.count(), 0)

    @override_settings(
        ANNOUNCEMENT_BASE_URL='http://example.com',
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米', 'VIVO'],
    )
    @patch('gmtool.announcement_views.query_announcements')
    def test_query_multi_channel_partial_failure_groups_results_and_failures(self, mocked_query):
        mocked_query.side_effect = [
            ([{'AnnouncementId': '1', 'AnnouncementType': '2'}], '', '[]', ''),
            (None, 'FAIL: busy', 'FAIL: busy', 'failed'),
        ]

        response = self.client.get(reverse('gmtool:announcement_query'), {
            'Platform': 'Android',
            'Channel': ['小米', 'VIVO'],
        })

        self.assertEqual(response.status_code, 200)
        self.assertEqual(len(response.context['query_results']), 1)
        self.assertEqual(len(response.context['query_failures']), 1)
        self.assertEqual(response.context['query_failures'][0]['channel'], 'VIVO')

    @override_settings(
        ANNOUNCEMENT_BASE_URL='http://example.com',
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米', 'VIVO'],
    )
    @patch('gmtool.announcement_review_services.create_announcement')
    def test_create_multi_channel_does_not_publish_or_log_remote_actions(self, mocked_create):
        payload = self._valid_create_payload()
        payload['Channel'] = ['小米', 'VIVO']
        response = self.client.post(reverse('gmtool:announcement_create'), data=payload)

        self.assertEqual(response.status_code, 302)
        mocked_create.assert_not_called()
        self.assertEqual(AnnouncementReview.objects.count(), 2)
        self.assertEqual(AnnouncementLog.objects.filter(action='create').count(), 0)

    @override_settings(
        ANNOUNCEMENT_BASE_URL='http://example.com',
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米', 'VIVO'],
    )
    def test_create_multi_channel_weekly_duplicate_partial_failure_aggregates(self):
        AnnouncementReview.objects.create(
            submitter=self.admin,
            submitter_username=self.admin.username,
            platform='Android',
            channel='VIVO',
            announcement_type='1',
            title='旧待审',
            content='旧待审',
            payload={
                'Platform': 'Android',
                'Channel': 'VIVO',
                'AnnouncementType': '1',
                'AnnouncementId': '-1',
            },
        )

        payload = self._valid_create_payload(announcement_type='1')
        payload['Channel'] = ['小米', 'VIVO']
        response = self.client.post(reverse('gmtool:announcement_create'), data=payload)

        self.assertEqual(response.status_code, 400)
        self.assertContains(response, '小米', status_code=400)
        self.assertContains(response, 'VIVO', status_code=400)
        self.assertEqual(AnnouncementReview.objects.filter(status='pending').count(), 2)


class AnnouncementReviewViewTests(TestCase):
    def setUp(self):
        user_model = get_user_model()
        self.submitter = user_model.objects.create_user(
            username='review_submitter',
            email='review_submitter@example.com',
            password='password123',
        )
        self.reviewer = user_model.objects.create_user(
            username='reviewer',
            email='reviewer@example.com',
            password='password123',
        )
        self.no_permission_user = user_model.objects.create_user(
            username='review_no_perm',
            email='review_no_perm@example.com',
            password='password123',
        )
        UserAnnouncementPermission.objects.create(user=self.submitter)
        UserAnnouncementPermission.objects.create(user=self.reviewer)
        self.client.defaults['HTTP_HOST'] = '127.0.0.1'
        self.client.force_login(self.reviewer)

    def _review_payload(self, announcement_type='2', channel='小米'):
        payload = {
            'Platform': 'Android',
            'Channel': channel,
            'AnnouncementType': announcement_type,
            'AnnouncementId': '-1',
            'Title': '待审核标题',
            'Content': '待审核正文',
            'Image_1': '',
            'ImageLink_1': '',
            'Image_2': '',
            'ImageLink_2': '',
            'Image_3': '',
            'ImageLink_3': '',
            'Reserve_1': '',
            'Reserve_2': '',
        }
        if announcement_type == '3':
            payload.update({
                'Title': '',
                'Content': '',
                'Image_1': 'https://example.com/banner.png',
                'ImageLink_1': 'https://example.com/open',
            })
        return payload

    def _create_review(self, *, status='pending', announcement_type='2', submitter=None, channel='小米'):
        submitter = submitter or self.submitter
        payload = self._review_payload(announcement_type=announcement_type, channel=channel)
        return AnnouncementReview.objects.create(
            status=status,
            submitter=submitter,
            submitter_username=submitter.username,
            platform=payload['Platform'],
            channel=payload['Channel'],
            announcement_type=payload['AnnouncementType'],
            announcement_id=payload['AnnouncementId'],
            title=payload['Title'],
            content=payload['Content'],
            priority=7 if announcement_type == '2' else 0,
            image_1=payload['Image_1'],
            image_link_1=payload['ImageLink_1'],
            payload=payload,
            error_message='上次失败' if status == 'failed' else '',
        )

    def test_review_index_redirects_to_announcement_review(self):
        response = self.client.get(reverse('gmtool:review_index'))

        self.assertEqual(response.status_code, 302)
        self.assertEqual(response['Location'], reverse('gmtool:review_announcement_list'))

    def test_review_urls_require_announcement_permission(self):
        self.client.force_login(self.no_permission_user)

        response = self.client.get(reverse('gmtool:review_announcement_list'))

        self.assertEqual(response.status_code, 302)
        self.assertEqual(response['Location'], reverse('gmtool:dashboard'))

    def test_review_navigation_visible_only_with_permission(self):
        response = self.client.get(reverse('gmtool:dashboard'))
        self.assertContains(response, '审核管理')

        self.client.force_login(self.no_permission_user)
        response = self.client.get(reverse('gmtool:dashboard'))
        self.assertNotContains(response, '审核管理')

    def test_review_list_defaults_to_pending_and_can_filter_failed(self):
        pending = self._create_review(status='pending')
        failed = self._create_review(status='failed', channel='VIVO')
        pending.title = 'PENDING_REVIEW_MARKER'
        pending.save(update_fields=['title'])
        failed.title = 'FAILED_REVIEW_MARKER'
        failed.error_message = 'FAIL_REASON_MARKER'
        failed.save(update_fields=['title', 'error_message'])

        response = self.client.get(reverse('gmtool:review_announcement_list'))
        self.assertEqual(response.status_code, 200)
        self.assertContains(response, 'PENDING_REVIEW_MARKER')
        self.assertNotContains(response, 'FAILED_REVIEW_MARKER')

        response = self.client.get(reverse('gmtool:review_announcement_list'), {'status': 'failed'})
        self.assertContains(response, 'FAILED_REVIEW_MARKER')
        self.assertContains(response, 'FAIL_REASON_MARKER')
        self.assertNotContains(response, 'PENDING_REVIEW_MARKER')

    @override_settings(ANNOUNCEMENT_BASE_URL='http://example.com')
    @patch('gmtool.announcement_review_services.create_announcement')
    def test_reject_updates_status_without_remote_call(self, mocked_create):
        review = self._create_review(status='pending')

        response = self.client.post(
            reverse('gmtool:review_announcement_reject', args=[review.id]),
            data={'review_comment': '无需发布'},
        )

        self.assertEqual(response.status_code, 302)
        mocked_create.assert_not_called()
        review.refresh_from_db()
        self.assertEqual(review.status, 'rejected')
        self.assertEqual(review.reviewer, self.reviewer)
        self.assertEqual(review.review_comment, '无需发布')
        self.assertEqual(AnnouncementLog.objects.count(), 0)

    def test_submitter_cannot_reject_own_review(self):
        review = self._create_review(status='pending', submitter=self.submitter)
        self.client.force_login(self.submitter)

        response = self.client.post(reverse('gmtool:review_announcement_reject', args=[review.id]))

        self.assertEqual(response.status_code, 403)
        review.refresh_from_db()
        self.assertEqual(review.status, 'pending')

    @override_settings(ANNOUNCEMENT_BASE_URL='http://example.com')
    @patch('gmtool.announcement_review_services.query_announcements')
    @patch('gmtool.announcement_review_services.delete_announcement')
    @patch('gmtool.announcement_review_services.create_announcement')
    def test_approve_persistent_review_publishes_and_writes_log(
        self,
        mocked_create,
        mocked_delete,
        mocked_query,
    ):
        review = self._create_review(status='pending', announcement_type='2')
        mocked_create.return_value = ({'result': 'OK'}, '', 'OK', '')

        response = self.client.post(reverse('gmtool:review_announcement_approve', args=[review.id]))

        self.assertEqual(response.status_code, 302)
        mocked_query.assert_not_called()
        mocked_delete.assert_not_called()
        mocked_create.assert_called_once_with(review.payload)
        review.refresh_from_db()
        self.assertEqual(review.status, 'approved')
        self.assertEqual(review.reviewer, self.reviewer)
        log = AnnouncementLog.objects.get(action='create', status='success')
        self.assertEqual(log.user, self.submitter)
        self.assertEqual(log.operator_username, self.submitter.username)

    @override_settings(ANNOUNCEMENT_BASE_URL='http://example.com')
    @patch('gmtool.announcement_review_services.query_announcements')
    @patch('gmtool.announcement_review_services.delete_announcement')
    @patch('gmtool.announcement_review_services.create_announcement')
    def test_weekly_approve_deletes_old_weekly_before_create(
        self,
        mocked_create,
        mocked_delete,
        mocked_query,
    ):
        events = []
        review = self._create_review(status='pending', announcement_type='1')

        def fake_query(platform, channel):
            events.append('query')
            return ([
                {
                    'Platform': platform,
                    'Channel': channel,
                    'AnnouncementType': '1',
                    'AnnouncementId': 'old-weekly',
                },
                {
                    'Platform': platform,
                    'Channel': channel,
                    'AnnouncementType': '2',
                    'AnnouncementId': 'old-persistent',
                },
            ], '', '[]', '')

        def fake_delete(*args):
            events.append('delete')
            return ({'result': 'OK'}, '', 'OK', '')

        def fake_create(*args):
            events.append('create')
            return ({'result': 'OK'}, '', 'OK', '')

        mocked_query.side_effect = fake_query
        mocked_delete.side_effect = fake_delete
        mocked_create.side_effect = fake_create

        response = self.client.post(reverse('gmtool:review_announcement_approve', args=[review.id]))

        self.assertEqual(response.status_code, 302)
        self.assertEqual(events, ['query', 'delete', 'create'])
        mocked_delete.assert_called_once_with('Android', '小米', '1', 'old-weekly')
        delete_log = AnnouncementLog.objects.get(action='delete', announcement_id='old-weekly')
        create_log = AnnouncementLog.objects.get(action='create')
        self.assertEqual(delete_log.user, self.submitter)
        self.assertEqual(delete_log.operator_username, self.submitter.username)
        self.assertEqual(create_log.user, self.submitter)
        self.assertEqual(create_log.operator_username, self.submitter.username)
        self.assertFalse(AnnouncementLog.objects.filter(announcement_id='old-persistent').exists())

    @override_settings(ANNOUNCEMENT_BASE_URL='http://example.com')
    @patch('gmtool.announcement_review_services.create_announcement')
    def test_approve_failure_marks_failed_and_retry_success(self, mocked_create):
        review = self._create_review(status='pending', announcement_type='2')
        mocked_create.return_value = (None, 'FAIL: busy', 'FAIL: busy', 'failed')

        response = self.client.post(reverse('gmtool:review_announcement_approve', args=[review.id]))

        self.assertEqual(response.status_code, 302)
        review.refresh_from_db()
        self.assertEqual(review.status, 'failed')
        self.assertEqual(review.error_message, 'FAIL: busy')
        failed_log = AnnouncementLog.objects.get(action='create', status='failed')
        self.assertEqual(failed_log.user, self.submitter)
        self.assertEqual(failed_log.operator_username, self.submitter.username)

        mocked_create.return_value = ({'result': 'OK'}, '', 'OK', '')
        response = self.client.post(reverse('gmtool:review_announcement_retry', args=[review.id]))

        self.assertEqual(response.status_code, 302)
        review.refresh_from_db()
        self.assertEqual(review.status, 'approved')
        self.assertEqual(review.error_message, '')
        success_log = AnnouncementLog.objects.get(action='create', status='success')
        self.assertEqual(success_log.user, self.submitter)
        self.assertEqual(success_log.operator_username, self.submitter.username)

    @override_settings(ANNOUNCEMENT_BASE_URL='http://example.com')
    @patch('gmtool.announcement_review_services.create_announcement')
    def test_retry_failure_keeps_failed_status(self, mocked_create):
        review = self._create_review(status='failed', announcement_type='2')
        mocked_create.return_value = (None, 'FAIL: busy again', 'FAIL: busy again', 'failed')

        response = self.client.post(reverse('gmtool:review_announcement_retry', args=[review.id]))

        self.assertEqual(response.status_code, 302)
        review.refresh_from_db()
        self.assertEqual(review.status, 'failed')
        self.assertEqual(review.error_message, 'FAIL: busy again')

    @override_settings(ANNOUNCEMENT_BASE_URL='http://example.com')
    @patch('gmtool.announcement_review_services.create_announcement')
    def test_submitter_cannot_approve_or_retry_own_review(self, mocked_create):
        pending = self._create_review(status='pending', submitter=self.submitter)
        failed = self._create_review(status='failed', submitter=self.submitter, channel='VIVO')
        self.client.force_login(self.submitter)

        approve_response = self.client.post(reverse('gmtool:review_announcement_approve', args=[pending.id]))
        retry_response = self.client.post(reverse('gmtool:review_announcement_retry', args=[failed.id]))

        self.assertEqual(approve_response.status_code, 403)
        self.assertEqual(retry_response.status_code, 403)
        mocked_create.assert_not_called()
        pending.refresh_from_db()
        failed.refresh_from_db()
        self.assertEqual(pending.status, 'pending')
        self.assertEqual(failed.status, 'failed')

    @override_settings(ANNOUNCEMENT_BASE_URL='http://example.com')
    @patch('gmtool.announcement_review_services.query_announcements')
    @patch('gmtool.announcement_review_services.create_announcement')
    def test_weekly_approve_fails_when_old_announcement_type_missing(self, mocked_create, mocked_query):
        review = self._create_review(status='pending', announcement_type='1')
        mocked_query.return_value = ([{
            'Platform': 'Android',
            'Channel': '小米',
            'AnnouncementId': 'old-unknown',
        }], '', '[]', '')

        response = self.client.post(reverse('gmtool:review_announcement_approve', args=[review.id]))

        self.assertEqual(response.status_code, 302)
        mocked_create.assert_not_called()
        review.refresh_from_db()
        self.assertEqual(review.status, 'failed')
        self.assertIn('旧公告缺少公告类型', review.error_message)
        self.assertEqual(AnnouncementLog.objects.count(), 0)

    @override_settings(ANNOUNCEMENT_BASE_URL='http://example.com')
    @patch('gmtool.announcement_log_services.AnnouncementLog.objects.create', side_effect=DatabaseError('db failed'))
    @patch('gmtool.announcement_review_services.create_announcement')
    def test_log_persistence_database_error_does_not_rollback_approval(self, mocked_create, mocked_log_create):
        review = self._create_review(status='pending', announcement_type='2')
        mocked_create.return_value = ({'result': 'OK'}, '', 'OK', '')

        response = self.client.post(reverse('gmtool:review_announcement_approve', args=[review.id]))

        self.assertEqual(response.status_code, 302)
        review.refresh_from_db()
        self.assertEqual(review.status, 'approved')
        mocked_log_create.assert_called_once()
        self.assertEqual(AnnouncementLog.objects.count(), 0)

    @override_settings(ANNOUNCEMENT_BASE_URL='http://example.com')
    @patch('gmtool.announcement_review_services.query_announcements')
    @patch('gmtool.announcement_review_services.create_announcement')
    def test_weekly_retry_rejects_stale_failed_review_when_newer_review_exists(
        self,
        mocked_create,
        mocked_query,
    ):
        stale = self._create_review(status='failed', announcement_type='1')
        self._create_review(status='approved', announcement_type='1')

        response = self.client.post(reverse('gmtool:review_announcement_retry', args=[stale.id]))

        self.assertEqual(response.status_code, 302)
        mocked_query.assert_not_called()
        mocked_create.assert_not_called()
        stale.refresh_from_db()
        self.assertEqual(stale.status, 'failed')


class CommandReviewClassificationTests(TestCase):
    def _command(self, *, tab, command_name, request_name):
        return GMCommand(
            command_id='cmd',
            command_name=command_name,
            tab=tab,
            request_name=request_name,
            request_id=1,
            response_name=request_name.replace('Req', 'Rsp'),
            response_id=2,
        )

    def test_mail_command_requires_mail_review(self):
        command = self._command(
            tab='发邮件送道具',
            command_name='发邮件送道具',
            request_name='DoSendMailReq',
        )

        self.assertEqual(get_command_review_type(command), CommandReview.TYPE_MAIL)

    def test_marquee_publish_command_requires_marquee_review(self):
        command = self._command(
            tab='发公告',
            command_name='发公告',
            request_name='DoSendRollingMsgReq',
        )

        self.assertEqual(get_command_review_type(command), CommandReview.TYPE_MARQUEE)

    def test_marquee_query_and_delete_commands_do_not_require_review(self):
        query_command = self._command(
            tab='查询公告',
            command_name='查询公告',
            request_name='DoQueryRollingMsgReq',
        )
        delete_command = self._command(
            tab='删除公告',
            command_name='删除公告',
            request_name='DoDeleteRollingMsgReq',
        )

        self.assertEqual(get_command_review_type(query_command), '')
        self.assertEqual(get_command_review_type(delete_command), '')


class CommandReviewFlowTests(TestCase):
    def setUp(self):
        user_model = get_user_model()
        self.submitter = user_model.objects.create_user(
            username='cmd_review_submitter',
            email='cmd_review_submitter@example.com',
            password='password123',
        )
        self.mail_reviewer = user_model.objects.create_user(
            username='mail_reviewer',
            email='mail_reviewer@example.com',
            password='password123',
        )
        self.marquee_reviewer = user_model.objects.create_user(
            username='marquee_reviewer',
            email='marquee_reviewer@example.com',
            password='password123',
        )
        self.no_permission_user = user_model.objects.create_user(
            username='cmd_review_no_perm',
            email='cmd_review_no_perm@example.com',
            password='password123',
        )
        self.mail_command = self._create_command(
            command_id='10226008',
            command_name='发邮件送道具',
            tab='发邮件送道具',
            request_name='DoSendMailReq',
            request_id=4211,
            response_name='DoSendMailRsp',
            response_id=4212,
        )
        self.marquee_publish_command = self._create_command(
            command_id='10226001',
            command_name='发公告',
            tab='发公告',
            request_name='DoSendRollingMsgReq',
            request_id=4201,
            response_name='DoSendRollingMsgRsp',
            response_id=4202,
        )
        self.marquee_query_command = self._create_command(
            command_id='10226002',
            command_name='查询公告',
            tab='查询公告',
            request_name='DoQueryRollingMsgReq',
            request_id=4203,
            response_name='DoQueryRollingMsgRsp',
            response_id=4204,
        )
        self.marquee_delete_command = self._create_command(
            command_id='10226003',
            command_name='删除公告',
            tab='删除公告',
            request_name='DoDeleteRollingMsgReq',
            request_id=4205,
            response_name='DoDeleteRollingMsgRsp',
            response_id=4206,
        )
        UserCommandPermission.objects.create(user=self.submitter, command=self.mail_command)
        UserCommandPermission.objects.create(user=self.submitter, command=self.marquee_publish_command)
        UserCommandPermission.objects.create(user=self.submitter, command=self.marquee_query_command)
        UserCommandPermission.objects.create(user=self.submitter, command=self.marquee_delete_command)
        UserCommandPermission.objects.create(user=self.mail_reviewer, command=self.mail_command)
        UserCommandPermission.objects.create(user=self.marquee_reviewer, command=self.marquee_publish_command)
        self.client.defaults['HTTP_HOST'] = '127.0.0.1'

    def _create_command(self, **kwargs):
        return GMCommand.objects.create(
            request_params=[
                {'id': 'AreaId', 'type': 'uint32', 'isnull': 'false'},
                {'id': 'Partition', 'type': 'uint32', 'isnull': 'false'},
                {'id': 'PlatId', 'type': 'uint8', 'isnull': 'false'},
            ],
            response_params=[],
            field_labels={},
            is_active=True,
            **kwargs,
        )

    def _params(self, partition=1):
        return {
            'AreaId': 10,
            'Partition': partition,
            'PlatId': 1,
        }

    def _post_execute(self, command, params=None):
        return self.client.post(
            reverse('gmtool:command_execute', args=[command.command_id]),
            data=json.dumps({'params': params or self._params()}),
            content_type='application/json',
        )

    def _create_review(self, *, status='pending', review_type=CommandReview.TYPE_MAIL, submitter=None):
        submitter = submitter or self.submitter
        command = self.mail_command if review_type == CommandReview.TYPE_MAIL else self.marquee_publish_command
        return CommandReview.objects.create(
            review_type=review_type,
            status=status,
            command=command,
            command_code=command.command_id,
            command_name=command.command_name,
            command_tab=command.tab,
            request_name=command.request_name,
            request_id=command.request_id,
            response_name=command.response_name,
            response_id=command.response_id,
            submitter=submitter,
            submitter_username=submitter.username,
            partition=1,
            params=self._params(),
            error_message='上次失败' if status == 'failed' else '',
            ip_address='127.0.0.1',
        )

    @patch('gmtool.command_views.send_idip_command')
    def test_mail_command_submit_creates_review_without_remote_call(self, mocked_send):
        self.client.force_login(self.submitter)

        response = self._post_execute(self.mail_command, self._params(partition='2'))

        self.assertEqual(response.status_code, 200)
        payload = response.json()
        self.assertTrue(payload['review_required'])
        self.assertEqual(payload['review_type'], CommandReview.TYPE_MAIL)
        mocked_send.assert_not_called()
        self.assertEqual(CommandLog.objects.count(), 0)
        review = CommandReview.objects.get()
        self.assertEqual(review.status, 'pending')
        self.assertEqual(review.submitter, self.submitter)
        self.assertEqual(review.partition, 2)
        self.assertEqual(review.params['Partition'], 2)

    @patch('gmtool.command_views.send_idip_command')
    def test_marquee_publish_command_submit_creates_review(self, mocked_send):
        self.client.force_login(self.submitter)

        response = self._post_execute(self.marquee_publish_command)

        self.assertEqual(response.status_code, 200)
        payload = response.json()
        self.assertTrue(payload['review_required'])
        self.assertEqual(payload['review_type'], CommandReview.TYPE_MARQUEE)
        mocked_send.assert_not_called()
        review = CommandReview.objects.get()
        self.assertEqual(review.command_code, self.marquee_publish_command.command_id)
        self.assertEqual(review.status, 'pending')

    @patch('gmtool.command_views.send_idip_command')
    def test_marquee_query_and_delete_commands_execute_directly(self, mocked_send):
        self.client.force_login(self.submitter)
        mocked_send.return_value = (
            {'status': True, 'id': 4203, 'json': {'Result': 0, 'RetMsg': 'OK'}},
            None,
            '{"mock": true}',
            '',
        )

        for command in (self.marquee_query_command, self.marquee_delete_command):
            with self.subTest(command=command.command_name):
                response = self._post_execute(command)

                self.assertEqual(response.status_code, 200)
                payload = response.json()
                self.assertNotIn('review_required', payload)
                self.assertTrue(payload['status'])

        self.assertEqual(mocked_send.call_count, 2)
        self.assertEqual(CommandReview.objects.count(), 0)
        self.assertEqual(CommandLog.objects.count(), 2)

    def test_reviewed_command_execute_page_uses_submit_review_label(self):
        self.client.force_login(self.submitter)

        response = self.client.get(reverse('gmtool:command_execute', args=[self.mail_command.command_id]))

        self.assertEqual(response.status_code, 200)
        self.assertContains(response, '提交审核')

    def test_review_permissions_follow_command_permissions(self):
        self.assertTrue(has_command_review_type_permission(self.mail_reviewer, CommandReview.TYPE_MAIL))
        self.assertFalse(has_command_review_type_permission(self.mail_reviewer, CommandReview.TYPE_MARQUEE))
        self.assertTrue(has_command_review_type_permission(self.marquee_reviewer, CommandReview.TYPE_MARQUEE))
        self.assertFalse(has_command_review_type_permission(self.no_permission_user, CommandReview.TYPE_MAIL))

    def test_review_navigation_and_pages_use_command_review_permissions(self):
        self.client.force_login(self.mail_reviewer)
        response = self.client.get(reverse('gmtool:dashboard'))
        self.assertContains(response, '审核管理')
        response = self.client.get(reverse('gmtool:review_index'))
        self.assertEqual(response.status_code, 302)
        self.assertEqual(response['Location'], reverse('gmtool:review_mail'))
        response = self.client.get(reverse('gmtool:review_mail'))
        self.assertEqual(response.status_code, 200)
        response = self.client.get(reverse('gmtool:review_marquee'))
        self.assertEqual(response.status_code, 302)
        self.assertEqual(response['Location'], reverse('gmtool:dashboard'))

        self.client.force_login(self.marquee_reviewer)
        response = self.client.get(reverse('gmtool:review_index'))
        self.assertEqual(response.status_code, 302)
        self.assertEqual(response['Location'], reverse('gmtool:review_marquee'))
        response = self.client.get(reverse('gmtool:review_marquee'))
        self.assertEqual(response.status_code, 200)

        self.client.force_login(self.no_permission_user)
        response = self.client.get(reverse('gmtool:dashboard'))
        self.assertNotContains(response, '审核管理')
        response = self.client.get(reverse('gmtool:review_index'))
        self.assertEqual(response.status_code, 302)
        self.assertEqual(response['Location'], reverse('gmtool:dashboard'))

    @patch('gmtool.command_review_services.send_idip_command')
    def test_approve_executes_command_and_writes_log_for_submitter(self, mocked_send):
        review = self._create_review(status='pending')
        self.client.force_login(self.mail_reviewer)
        mocked_send.return_value = (
            {'status': True, 'id': self.mail_command.request_id, 'json': {'Result': 0, 'RetMsg': 'OK'}},
            None,
            '{"mock": true}',
            '',
        )

        response = self.client.post(
            reverse(
                'gmtool:review_command_approve',
                kwargs={'review_type': CommandReview.TYPE_MAIL, 'review_id': review.id},
            ),
        )

        self.assertEqual(response.status_code, 302)
        mocked_send.assert_called_once()
        review.refresh_from_db()
        self.assertEqual(review.status, 'approved')
        self.assertEqual(review.reviewer, self.mail_reviewer)
        self.assertEqual(review.request_content, '{"mock": true}')
        log = CommandLog.objects.get(status='success')
        self.assertEqual(log.user, self.submitter)
        self.assertEqual(log.operator_username, self.submitter.username)
        self.assertEqual(log.command, self.mail_command)

    @patch('gmtool.command_review_services.send_idip_command')
    def test_approve_failure_marks_failed_and_retry_success(self, mocked_send):
        review = self._create_review(status='pending')
        self.client.force_login(self.mail_reviewer)
        mocked_send.return_value = (None, 'FAIL: busy', 'FAIL: busy', 'failed')

        response = self.client.post(
            reverse(
                'gmtool:review_command_approve',
                kwargs={'review_type': CommandReview.TYPE_MAIL, 'review_id': review.id},
            ),
        )

        self.assertEqual(response.status_code, 302)
        review.refresh_from_db()
        self.assertEqual(review.status, 'failed')
        self.assertEqual(review.error_message, 'FAIL: busy')
        self.assertEqual(CommandLog.objects.get().status, 'failed')

        mocked_send.return_value = (
            {'status': True, 'id': self.mail_command.request_id, 'json': {'Result': 0, 'RetMsg': 'OK'}},
            None,
            '{"mock": true}',
            '',
        )
        response = self.client.post(
            reverse(
                'gmtool:review_command_retry',
                kwargs={'review_type': CommandReview.TYPE_MAIL, 'review_id': review.id},
            ),
        )

        self.assertEqual(response.status_code, 302)
        review.refresh_from_db()
        self.assertEqual(review.status, 'approved')
        self.assertEqual(review.error_message, '')
        self.assertEqual(CommandLog.objects.filter(status='success').count(), 1)

    @patch('gmtool.command_review_services.send_idip_command')
    def test_submitter_cannot_approve_retry_or_reject_own_review(self, mocked_send):
        pending = self._create_review(status='pending', submitter=self.submitter)
        failed = self._create_review(status='failed', submitter=self.submitter)
        self.client.force_login(self.submitter)

        approve_response = self.client.post(
            reverse(
                'gmtool:review_command_approve',
                kwargs={'review_type': CommandReview.TYPE_MAIL, 'review_id': pending.id},
            ),
        )
        retry_response = self.client.post(
            reverse(
                'gmtool:review_command_retry',
                kwargs={'review_type': CommandReview.TYPE_MAIL, 'review_id': failed.id},
            ),
        )
        reject_response = self.client.post(
            reverse(
                'gmtool:review_command_reject',
                kwargs={'review_type': CommandReview.TYPE_MAIL, 'review_id': pending.id},
            ),
        )

        self.assertEqual(approve_response.status_code, 403)
        self.assertEqual(retry_response.status_code, 403)
        self.assertEqual(reject_response.status_code, 403)
        mocked_send.assert_not_called()
        pending.refresh_from_db()
        failed.refresh_from_db()
        self.assertEqual(pending.status, 'pending')
        self.assertEqual(failed.status, 'failed')

    @patch('gmtool.command_review_services.send_idip_command')
    def test_reject_updates_status_without_remote_call(self, mocked_send):
        review = self._create_review(status='pending')
        self.client.force_login(self.mail_reviewer)

        response = self.client.post(
            reverse(
                'gmtool:review_command_reject',
                kwargs={'review_type': CommandReview.TYPE_MAIL, 'review_id': review.id},
            ),
            data={'review_comment': '无需执行'},
        )

        self.assertEqual(response.status_code, 302)
        mocked_send.assert_not_called()
        review.refresh_from_db()
        self.assertEqual(review.status, 'rejected')
        self.assertEqual(review.reviewer, self.mail_reviewer)
        self.assertEqual(review.review_comment, '无需执行')
        self.assertEqual(CommandLog.objects.count(), 0)

    @patch('gmtool.command_review_services.CommandLog.objects.create', side_effect=DatabaseError('db failed'))
    @patch('gmtool.command_review_services.send_idip_command')
    def test_command_log_persistence_error_does_not_block_approval(self, mocked_send, mocked_log_create):
        review = self._create_review(status='pending')
        self.client.force_login(self.mail_reviewer)
        mocked_send.return_value = (
            {'status': True, 'id': self.mail_command.request_id, 'json': {'Result': 0, 'RetMsg': 'OK'}},
            None,
            '{"mock": true}',
            '',
        )

        response = self.client.post(
            reverse(
                'gmtool:review_command_approve',
                kwargs={'review_type': CommandReview.TYPE_MAIL, 'review_id': review.id},
            ),
        )

        self.assertEqual(response.status_code, 302)
        review.refresh_from_db()
        self.assertEqual(review.status, 'approved')
        mocked_log_create.assert_called_once()
        self.assertEqual(CommandLog.objects.count(), 0)


class ReviewProcessingStateTests(TransactionTestCase):
    def setUp(self):
        user_model = get_user_model()
        self.submitter = user_model.objects.create_user(
            username='processing_submitter',
            email='processing_submitter@example.com',
            password='password123',
        )
        self.reviewer = user_model.objects.create_user(
            username='processing_reviewer',
            email='processing_reviewer@example.com',
            password='password123',
        )
        self.command = GMCommand.objects.create(
            command_id='processing_mail',
            command_name='发邮件送道具',
            tab='发邮件送道具',
            request_name='DoSendMailReq',
            request_id=8801,
            response_name='DoSendMailRsp',
            response_id=8802,
            request_params=[],
            response_params=[],
            field_labels={},
            is_active=True,
        )
        self.command_review = CommandReview.objects.create(
            review_type=CommandReview.TYPE_MAIL,
            status='pending',
            command=self.command,
            command_code=self.command.command_id,
            command_name=self.command.command_name,
            command_tab=self.command.tab,
            request_name=self.command.request_name,
            request_id=self.command.request_id,
            response_name=self.command.response_name,
            response_id=self.command.response_id,
            submitter=self.submitter,
            submitter_username=self.submitter.username,
            partition=1,
            params={'AreaId': 10, 'Partition': 1, 'PlatId': 1},
            ip_address='127.0.0.1',
        )
        self.announcement_review = AnnouncementReview.objects.create(
            status='pending',
            submitter=self.submitter,
            submitter_username=self.submitter.username,
            platform='Android',
            channel='小米',
            announcement_type='2',
            announcement_id='-1',
            title='待审核标题',
            content='待审核正文',
            priority=1,
            payload={
                'Platform': 'Android',
                'Channel': '小米',
                'AnnouncementType': '2',
                'AnnouncementId': '-1',
                'Title': '待审核标题',
                'Content': '待审核正文',
            },
        )

    @patch('gmtool.command_review_services.send_idip_command')
    def test_command_remote_call_runs_after_processing_state_is_committed(self, mocked_send):
        atomic_states = []

        def fake_send(_command, _params):
            atomic_states.append(connection.in_atomic_block)
            self.command_review.refresh_from_db()
            self.assertEqual(self.command_review.status, 'processing')
            return (
                {'status': True, 'id': self.command.request_id, 'json': {'Result': 0, 'RetMsg': 'OK'}},
                None,
                '{"mock": true}',
                '',
            )

        mocked_send.side_effect = fake_send

        result = approve_command_review(self.command_review.id, self.reviewer, '127.0.0.1')

        self.assertEqual(atomic_states, [False])
        self.assertTrue(result.succeeded)
        self.command_review.refresh_from_db()
        self.assertEqual(self.command_review.status, 'approved')

    @patch('gmtool.announcement_review_services.create_announcement')
    def test_announcement_remote_call_runs_after_processing_state_is_committed(self, mocked_create):
        atomic_states = []

        def fake_create(_payload):
            atomic_states.append(connection.in_atomic_block)
            self.announcement_review.refresh_from_db()
            self.assertEqual(self.announcement_review.status, 'processing')
            return ({'result': 'OK'}, '', 'OK', '')

        mocked_create.side_effect = fake_create

        result = approve_announcement_review(self.announcement_review.id, self.reviewer, '127.0.0.1')

        self.assertEqual(atomic_states, [False])
        self.assertTrue(result.succeeded)
        self.announcement_review.refresh_from_db()
        self.assertEqual(self.announcement_review.status, 'approved')


class AnnouncementPermissionAssignmentTests(TestCase):
    def setUp(self):
        user_model = get_user_model()
        self.admin = user_model.objects.create_superuser(
            username='perm_admin',
            email='perm_admin@example.com',
            password='password123',
        )
        self.target = user_model.objects.create_user(
            username='perm_target',
            email='perm_target@example.com',
            password='password123',
        )
        self.client.defaults['HTTP_HOST'] = '127.0.0.1'
        self.client.force_login(self.admin)
        self.command = GMCommand.objects.create(
            command_id='ann_perm_cmd',
            command_name='权限测试命令',
            tab='测试',
            request_name='AnnPermReq',
            request_id=9900,
            response_name='AnnPermRsp',
            response_id=9901,
            request_params=[],
            response_params=[],
            field_labels={},
            is_active=True,
        )

    def test_permission_page_saves_command_and_announcement_permission_together(self):
        response = self.client.post(reverse('gmtool:user_permissions', args=[self.target.id]), data={
            'commands': [str(self.command.id)],
            'announcement_permission': 'on',
        })

        self.assertEqual(response.status_code, 302)
        self.assertTrue(UserCommandPermission.objects.filter(user=self.target, command=self.command).exists())
        self.assertTrue(UserAnnouncementPermission.objects.filter(user=self.target).exists())

        response = self.client.post(reverse('gmtool:user_permissions', args=[self.target.id]), data={
            'commands': [],
        })

        self.assertEqual(response.status_code, 302)
        self.assertFalse(UserCommandPermission.objects.filter(user=self.target).exists())
        self.assertFalse(UserAnnouncementPermission.objects.filter(user=self.target).exists())


class AnnouncementLogDetailApiTests(TestCase):
    def setUp(self):
        user_model = get_user_model()
        self.admin = user_model.objects.create_superuser(
            username='ann_log_admin',
            email='ann_log_admin@example.com',
            password='password123',
        )
        self.user = user_model.objects.create_user(
            username='ann_log_user',
            email='ann_log_user@example.com',
            password='password123',
        )
        UserAnnouncementPermission.objects.create(user=self.user)
        self.client.defaults['HTTP_HOST'] = '127.0.0.1'
        self.log = AnnouncementLog.objects.create(
            user=self.admin,
            operator_username=self.admin.username,
            action='create',
            platform='Android',
            channel='小米',
            announcement_type='2',
            announcement_id='-1',
            request_data={'Title': '标题', 'token': 'secret-token'},
            response_data={'Result': 0, 'access_token': 'response-token'},
            raw_response=json.dumps({'password': 'raw-secret', 'ok': True}, ensure_ascii=False),
            status='success',
            error_message='',
            ip_address='127.0.0.1',
        )

    def test_authorized_user_can_fetch_masked_announcement_log_detail(self):
        self.client.force_login(self.user)

        response = self.client.get(reverse('gmtool:announcement_log_detail_api', args=[self.log.id]))

        self.assertEqual(response.status_code, 200)
        payload = response.json()
        self.assertIn('"token": "***"', payload['request_data'])
        self.assertIn('"access_token": "***"', payload['response_data'])
        self.assertIn('"password": "***"', payload['raw_response'])

    def test_non_json_raw_response_masks_sensitive_key_value_text(self):
        self.log.raw_response = 'FAIL token=secret-token password: plain-secret access_token="abc123"'
        self.log.error_message = 'remote secret=hidden'
        self.log.save(update_fields=['raw_response', 'error_message'])
        self.client.force_login(self.user)

        response = self.client.get(reverse('gmtool:announcement_log_detail_api', args=[self.log.id]))

        self.assertEqual(response.status_code, 200)
        payload = response.json()
        self.assertNotIn('secret-token', payload['raw_response'])
        self.assertNotIn('plain-secret', payload['raw_response'])
        self.assertNotIn('abc123', payload['raw_response'])
        self.assertIn('token=***', payload['raw_response'])
        self.assertIn('password: ***', payload['raw_response'])
        self.assertIn('access_token="***"', payload['raw_response'])
        self.assertNotIn('hidden', payload['error_message'])
        self.assertIn('secret=***', payload['error_message'])

    def test_unauthorized_user_cannot_fetch_announcement_log_detail(self):
        other = get_user_model().objects.create_user(
            username='ann_log_other',
            email='ann_log_other@example.com',
            password='password123',
        )
        self.client.force_login(other)

        response = self.client.get(reverse('gmtool:announcement_log_detail_api', args=[self.log.id]))

        self.assertEqual(response.status_code, 403)


class StructuredCommandSchemaTests(TestCase):
    def test_enrich_schema_fields_adds_struct_list_metadata(self):
        request_params = _enrich_schema_fields(
            [
                {'isnull': 'false', 'id': 'ItemList1_count', 'type': 'uint32', 'name': '道具数量'},
                {'isnull': 'false', 'id': 'ItemList1', 'type': 'SItemInfo', 'name': '道具列表', 'max_size': '5'},
            ],
            {
                'SItemInfo': [
                    {'isnull': 'false', 'id': 'ItemId', 'type': 'uint32', 'name': '道具Id'},
                    {'isnull': 'false', 'id': 'ItemNum', 'type': 'uint32', 'name': '道具数量'},
                    {'isnull': 'false', 'id': 'Star', 'type': 'int32', 'name': '星级'},
                    {'isnull': 'false', 'id': 'ExtraParam', 'type': 'int32', 'name': '额外参数'},
                ],
            },
            require_struct_list_max_size=True,
        )

        count_field = next(field for field in request_params if field['id'] == 'ItemList1_count')
        list_field = next(field for field in request_params if field['id'] == 'ItemList1')

        self.assertEqual(count_field['auto_count_for'], 'ItemList1')
        self.assertEqual(list_field['count_id'], 'ItemList1_count')
        self.assertEqual(list_field['max_size'], 5)
        self.assertEqual([child['id'] for child in list_field['children']], ['ItemId', 'ItemNum', 'Star', 'ExtraParam'])

    def test_request_struct_list_requires_max_size(self):
        with self.assertRaisesMessage(ValueError, 'max_size'):
            _enrich_schema_fields(
                [
                    {'isnull': 'false', 'id': 'ItemList1_count', 'type': 'uint32', 'name': '道具数量'},
                    {'isnull': 'false', 'id': 'ItemList1', 'type': 'SItemInfo', 'name': '道具列表'},
                ],
                {'SItemInfo': [{'isnull': 'false', 'id': 'ItemId', 'type': 'uint32'}]},
                require_struct_list_max_size=True,
            )

    def test_response_struct_list_does_not_require_max_size(self):
        response_params = _enrich_schema_fields(
            [
                {'isnull': 'false', 'id': 'UsrList_count', 'type': 'uint32', 'name': '角色数量'},
                {'isnull': 'false', 'id': 'UsrList', 'type': 'SUsrInfo', 'name': '角色列表'},
            ],
            {'SUsrInfo': [{'isnull': 'false', 'id': 'RoleId', 'type': 'string'}]},
        )

        list_field = next(field for field in response_params if field['id'] == 'UsrList')
        self.assertEqual(list_field['count_id'], 'UsrList_count')
        self.assertNotIn('max_size', list_field)


class StructuredCommandValidationTests(TestCase):
    def setUp(self):
        self.command = GMCommand(
            command_id='10226009',
            command_name='发邮件送多个道具',
            tab='发邮件送道具',
            request_name='DoSendMultiMailReq',
            request_id=4213,
            response_name='DoSendMultiMailRsp',
            response_id=4214,
            request_params=[
                {'isnull': 'false', 'id': 'AreaId', 'type': 'uint32'},
                {'isnull': 'false', 'id': 'Partition', 'type': 'uint32'},
                {'isnull': 'false', 'id': 'ItemList1_count', 'type': 'uint32', 'auto_count_for': 'ItemList1'},
                {
                    'isnull': 'false',
                    'id': 'ItemList1',
                    'type': 'SItemInfo',
                    'count_id': 'ItemList1_count',
                    'max_size': 2,
                    'children': [
                        {'isnull': 'false', 'id': 'ItemId', 'type': 'uint32'},
                        {'isnull': 'false', 'id': 'ItemNum', 'type': 'uint32'},
                        {'isnull': 'false', 'id': 'Star', 'type': 'int32'},
                    ],
                },
            ],
        )

    def test_structured_list_params_are_normalized(self):
        params = {
            'AreaId': '10',
            'Partition': '2',
            'ItemList1_count': '2',
            'ItemList1': [
                {'ItemId': '1001', 'ItemNum': '3', 'Star': '-1'},
                {'ItemId': 1002, 'ItemNum': 4, 'Star': 0},
            ],
        }

        valid, error = validate_command_params_basic(self.command, params)

        self.assertTrue(valid)
        self.assertEqual(error, '')
        self.assertEqual(params['AreaId'], 10)
        self.assertEqual(params['ItemList1_count'], 2)
        self.assertEqual(params['ItemList1'][0], {'ItemId': 1001, 'ItemNum': 3, 'Star': -1})

    def test_structured_list_count_must_match_item_count(self):
        params = {
            'AreaId': 10,
            'Partition': 2,
            'ItemList1_count': 2,
            'ItemList1': [{'ItemId': 1001, 'ItemNum': 3, 'Star': 0}],
        }

        valid, error = validate_command_params_basic(self.command, params)

        self.assertFalse(valid)
        self.assertIn('ItemList1_count', error)

    def test_structured_list_count_may_be_less_than_max_size(self):
        params = {
            'AreaId': 10,
            'Partition': 2,
            'ItemList1_count': 1,
            'ItemList1': [{'ItemId': 1001, 'ItemNum': 3, 'Star': 0}],
        }

        valid, error = validate_command_params_basic(self.command, params)

        self.assertTrue(valid)
        self.assertEqual(error, '')

    def test_structured_list_count_cannot_exceed_max_size(self):
        params = {
            'AreaId': 10,
            'Partition': 2,
            'ItemList1_count': 3,
            'ItemList1': [
                {'ItemId': 1001, 'ItemNum': 3, 'Star': 0},
                {'ItemId': 1002, 'ItemNum': 4, 'Star': 0},
                {'ItemId': 1003, 'ItemNum': 5, 'Star': 0},
            ],
        }

        valid, error = validate_command_params_basic(self.command, params)

        self.assertFalse(valid)
        self.assertIn('ItemList1', error)

    def test_structured_list_missing_max_size_fails_validation(self):
        field = next(field for field in self.command.request_params if field.get('id') == 'ItemList1')
        del field['max_size']
        params = {
            'AreaId': 10,
            'Partition': 2,
            'ItemList1_count': 1,
            'ItemList1': [{'ItemId': 1001, 'ItemNum': 3, 'Star': 0}],
        }

        valid, error = validate_command_params_basic(self.command, params)

        self.assertFalse(valid)
        self.assertIn('max_size', error)


class CommandExecutePersistenceFallbackTests(TestCase):
    def setUp(self):
        user_model = get_user_model()
        self.user = user_model.objects.create_superuser(
            username='admin',
            email='admin@example.com',
            password='password123',
        )
        self.client.force_login(self.user)
        self.command = GMCommand.objects.create(
            command_id='10001',
            command_name='测试命令',
            tab='测试',
            request_name='TestReq',
            request_id=4100,
            response_name='TestRsp',
            response_id=4101,
            request_params=[
                {'id': 'AreaId', 'type': 'uint32', 'isnull': 'false'},
                {'id': 'Partition', 'type': 'uint32', 'isnull': 'false'},
                {'id': 'PlatId', 'type': 'uint8', 'isnull': 'false'},
            ],
            response_params=[],
            field_labels={},
            is_active=True,
        )

    @patch('gmtool.command_views.log_operation')
    @patch('gmtool.command_views.CommandLog.objects.create', side_effect=DatabaseError('db write failed'))
    @patch('gmtool.command_views.send_idip_command')
    def test_remote_success_still_returns_remote_result_when_log_persistence_fails(
        self,
        mocked_send,
        mocked_log_create,
        mocked_log_operation,
    ):
        mocked_send.return_value = (
            {'status': True, 'id': 4100, 'json': {'Result': 0, 'RetMsg': 'OK'}},
            None,
            '{"mock": true}',
            '',
        )

        response = self.client.post(
            reverse('gmtool:command_execute', args=[self.command.command_id]),
            data=json.dumps({
                'params': {
                    'AreaId': 10,
                    'Partition': 1,
                    'PlatId': 1,
                }
            }),
            content_type='application/json',
        )

        self.assertEqual(response.status_code, 200)
        payload = response.json()
        self.assertTrue(payload['status'])
        self.assertFalse(payload['log_persisted'])
        self.assertIn('执行结果记录失败', payload['warning'])
        self.assertEqual(payload['json']['Result'], 0)
        mocked_log_create.assert_called_once()
        self.assertFalse(mocked_log_operation.call_args.kwargs['detail']['log_persisted'])


class ValidateJsonCommandIdsTests(TestCase):
    def setUp(self):
        GMCommand.objects.create(
            command_id='cmd_a',
            command_name='命令A',
            tab='测试',
            request_name='ReqA',
            request_id=100,
            response_name='RspA',
            response_id=101,
            request_params=[],
            response_params=[],
            field_labels={},
            is_active=True,
        )
        GMCommand.objects.create(
            command_id='cmd_b',
            command_name='命令B',
            tab='测试',
            request_name='ReqB',
            request_id=200,
            response_name='RspB',
            response_id=201,
            request_params=[],
            response_params=[],
            field_labels={},
            is_active=True,
        )

    def test_detects_conflict_when_existing_json_command_reuses_db_only_protocol_id(self):
        data = {
            'cmd_a': {
                'request_id': 200,
                'response_id': 301,
            }
        }

        is_valid, error_msg = validate_json_command_ids(data)

        self.assertFalse(is_valid)
        self.assertIn('cmd_a', error_msg)
        self.assertIn('cmd_b', error_msg)
        self.assertIn('请求 ID 200', error_msg)

    def test_allows_same_batch_updates_without_comparing_against_db_old_values(self):
        data = {
            'cmd_a': {
                'request_id': 200,
                'response_id': 202,
            },
            'cmd_b': {
                'request_id': 300,
                'response_id': 301,
            },
        }

        is_valid, error_msg = validate_json_command_ids(data)

        self.assertTrue(is_valid)
        self.assertEqual(error_msg, '')


class SyncCommandsToDbValidationTests(TestCase):
    def setUp(self):
        GMCommand.objects.create(
            command_id='cmd_a',
            command_name='命令A',
            tab='测试',
            request_name='ReqA',
            request_id=100,
            response_name='RspA',
            response_id=101,
            request_params=[],
            response_params=[],
            field_labels={},
            is_active=True,
        )
        self.temp_dir = tempfile.TemporaryDirectory()
        self.json_path = Path(self.temp_dir.name) / 'idip_commands.json'

    def tearDown(self):
        self.temp_dir.cleanup()

    def test_sync_rejects_cross_protocol_id_conflict_before_writing(self):
        self.json_path.write_text(
            json.dumps(
                {
                    'cmd_b': {
                        'name': '命令B',
                        'tab': '测试',
                        'request': 'ReqB',
                        'request_id': 101,
                        'response': 'RspB',
                        'response_id': 102,
                        'ReqB': [],
                        'RspB': [],
                    }
                },
                ensure_ascii=False,
            ),
            encoding='utf-8',
        )

        with self.assertRaisesMessage(ValueError, '检测到 ID 冲突'):
            sync_commands_to_db(str(self.json_path))

        self.assertFalse(GMCommand.objects.filter(command_id='cmd_b').exists())


class GetClientIpTests(TestCase):
    def setUp(self):
        self.factory = RequestFactory()

    @override_settings(TRUSTED_PROXY=True, TRUSTED_PROXY_COUNT=1)
    def test_single_trusted_proxy_returns_real_client_ip(self):
        request = self.factory.get(
            '/',
            REMOTE_ADDR='10.0.0.2',
            HTTP_X_FORWARDED_FOR='203.0.113.10, 10.0.0.2',
        )

        self.assertEqual(get_client_ip(request), '203.0.113.10')

    @override_settings(TRUSTED_PROXY=True, TRUSTED_PROXY_COUNT=2)
    def test_multiple_trusted_proxies_respect_proxy_count(self):
        request = self.factory.get(
            '/',
            REMOTE_ADDR='10.0.0.3',
            HTTP_X_FORWARDED_FOR='198.51.100.8, 10.0.0.2, 10.0.0.3',
        )

        self.assertEqual(get_client_ip(request), '198.51.100.8')

    @override_settings(TRUSTED_PROXY=False)
    def test_without_trusted_proxy_falls_back_to_remote_addr(self):
        request = self.factory.get('/', REMOTE_ADDR='192.0.2.99')

        self.assertEqual(get_client_ip(request), '192.0.2.99')


class UploadCommandsRollbackTests(TestCase):
    def setUp(self):
        user_model = get_user_model()
        self.user = user_model.objects.create_superuser(
            username='root',
            email='root@example.com',
            password='password123',
        )
        self.client.force_login(self.user)
        self.temp_dir = tempfile.TemporaryDirectory()
        self.json_path = Path(self.temp_dir.name) / 'idip_commands.json'
        self.original_content = json.dumps(
            {
                'old_cmd': {
                    'tab': '查询',
                    'request': 'OldReq',
                    'request_id': 5000,
                    'response_id': 5001,
                    'response': 'OldRsp',
                    'OldReq': [],
                    'OldRsp': [],
                }
            },
            ensure_ascii=False,
            indent=4,
        )
        self.json_path.write_text(self.original_content, encoding='utf-8')

    def tearDown(self):
        self.temp_dir.cleanup()

    @override_settings(UPLOAD_MAX_SIZE=1024 * 1024)
    @patch('gmtool.api_views.sync_commands_to_db', side_effect=DatabaseError('sync failed'))
    def test_upload_rolls_back_json_file_when_sync_fails(self, mocked_sync):
        uploaded_content = json.dumps(
            {
                'new_cmd': {
                    'tab': '查询',
                    'request': 'NewReq',
                    'request_id': 6000,
                    'response_id': 6001,
                    'response': 'NewRsp',
                    'NewReq': [],
                    'NewRsp': [],
                }
            },
            ensure_ascii=False,
            indent=4,
        )
        upload_file = SimpleUploadedFile(
            'idip_commands.json',
            uploaded_content.encode('utf-8'),
            content_type='application/json',
        )

        with override_settings(IDIP_JSON_PATH=str(self.json_path)):
            response = self.client.post(
                reverse('gmtool:upload_commands_api'),
                data={'file': upload_file},
            )

        self.assertEqual(response.status_code, 500)
        self.assertIn('已回滚', response.json()['error'])
        self.assertEqual(self.json_path.read_text(encoding='utf-8'), self.original_content)
        mocked_sync.assert_called_once()


class AddGmCommandRollbackTests(TestCase):
    def setUp(self):
        user_model = get_user_model()
        self.user = user_model.objects.create_superuser(
            username='rollbacker',
            email='rollbacker@example.com',
            password='password123',
        )
        self.client.force_login(self.user)
        self.temp_dir = tempfile.TemporaryDirectory()
        self.json_path = Path(self.temp_dir.name) / 'idip_commands.json'
        self.original_content = '{}'
        self.json_path.write_text(self.original_content, encoding='utf-8')

    def tearDown(self):
        self.temp_dir.cleanup()

    @patch('gmtool.command_views.sync_commands_to_db', side_effect=DatabaseError('sync failed'))
    def test_add_gm_command_rolls_back_json_file_when_sync_fails(self, mocked_sync):
        with override_settings(IDIP_JSON_PATH=str(self.json_path)):
            response = self.client.post(
                reverse('gmtool:add_gm_command'),
                data={
                    'command_id': '10229998',
                    'tab': '运营工具',
                    'command_name': '回滚测试命令',
                    'request_name': 'RollbackReq',
                    'request_id': 6198,
                    'response_name': 'RollbackRsp',
                    'response_id': 6199,
                    'request_desc': '',
                    'response_desc': '',
                    'request_params': '',
                    'response_params': '',
                },
            )

        self.assertEqual(response.status_code, 200)
        self.assertEqual(self.json_path.read_text(encoding='utf-8'), self.original_content)
        self.assertFalse(GMCommand.objects.filter(command_id='10229998').exists())
        mocked_sync.assert_called_once()


class AddGmCommandNamePersistenceTests(TestCase):
    def setUp(self):
        user_model = get_user_model()
        self.user = user_model.objects.create_superuser(
            username='creator',
            email='creator@example.com',
            password='password123',
        )
        self.client.force_login(self.user)
        self.temp_dir = tempfile.TemporaryDirectory()
        self.json_path = Path(self.temp_dir.name) / 'idip_commands.json'
        self.json_path.write_text('{}', encoding='utf-8')

    def tearDown(self):
        self.temp_dir.cleanup()

    def test_add_gm_command_persists_command_name_to_json_and_database(self):
        with override_settings(IDIP_JSON_PATH=str(self.json_path)):
            response = self.client.post(
                reverse('gmtool:add_gm_command'),
                data={
                    'command_id': '10229999',
                    'tab': '运营工具',
                    'command_name': '精准封禁',
                    'request_name': 'DoExactBanReq',
                    'request_id': 6200,
                    'response_name': 'DoExactBanRsp',
                    'response_id': 6201,
                    'request_desc': '',
                    'response_desc': '',
                    'request_params': '',
                    'response_params': '',
                },
            )

        self.assertEqual(response.status_code, 302)
        self.assertEqual(response.headers['Location'], reverse('gmtool:dashboard'))

        stored_json = json.loads(self.json_path.read_text(encoding='utf-8'))
        self.assertEqual(stored_json['10229999']['command_name'], '精准封禁')

        command = GMCommand.objects.get(command_id='10229999')
        self.assertEqual(command.command_name, '精准封禁')
        self.assertEqual(command.tab, '运营工具')


class LogDetailApiTests(TestCase):
    def setUp(self):
        user_model = get_user_model()
        self.owner = user_model.objects.create_user(
            username='owner',
            email='owner@example.com',
            password='password123',
        )
        self.other = user_model.objects.create_user(
            username='other',
            email='other@example.com',
            password='password123',
        )
        self.command = GMCommand.objects.create(
            command_id='10002',
            command_name='日志测试命令',
            tab='测试',
            request_name='LogReq',
            request_id=5100,
            response_name='LogRsp',
            response_id=5101,
            request_params=[],
            response_params=[],
            field_labels={},
            is_active=True,
        )
        self.log = CommandLog.objects.create(
            user=self.owner,
            operator_username=self.owner.username,
            command=self.command,
            partition=1,
            request_data={'password': 'plain-secret', 'nickname': 'alice'},
            request_content=json.dumps({
                'head': {'token': 'head-secret'},
                'body': {'nickname': 'alice', 'nested': {'access_token': 'nested-secret'}},
            }, ensure_ascii=False),
            response_data={'Result': 0, 'token': 'response-secret'},
            status='success',
            ip_address='127.0.0.1',
        )

    def test_owner_can_fetch_masked_log_detail(self):
        self.client.force_login(self.owner)

        response = self.client.get(reverse('gmtool:log_detail_api', args=[self.log.id]))

        self.assertEqual(response.status_code, 200)
        payload = response.json()
        self.assertIn('"password": "***"', payload['request_data'])
        self.assertIn('"token": "***"', payload['request_content'])
        self.assertIn('"access_token": "***"', payload['request_content'])
        self.assertIn('"nickname": "alice"', payload['request_content'])
        self.assertIn('"token": "***"', payload['response_data'])

    def test_non_owner_cannot_access_other_users_log_detail(self):
        self.client.force_login(self.other)

        response = self.client.get(reverse('gmtool:log_detail_api', args=[self.log.id]))

        self.assertEqual(response.status_code, 404)


class SuperuserPermissionSourceTests(TestCase):
    def test_superuser_creation_only_auto_creates_profile(self):
        user_model = get_user_model()
        user = user_model.objects.create_superuser(
            username='boss',
            email='boss@example.com',
            password='password123',
        )

        self.assertTrue(UserProfile.objects.filter(user=user).exists())
        self.assertEqual(UserCommandPermission.objects.filter(user=user).count(), 0)


class GMCommandDatabaseConstraintTests(TestCase):
    def setUp(self):
        GMCommand.objects.create(
            command_id='base_cmd',
            command_name='基础命令',
            tab='测试',
            request_name='BaseReq',
            request_id=7000,
            response_name='BaseRsp',
            response_id=7001,
            request_params=[],
            response_params=[],
            field_labels={},
            is_active=True,
        )

    def test_request_id_must_be_unique(self):
        with self.assertRaises(IntegrityError):
            GMCommand.objects.create(
                command_id='dup_req',
                command_name='重复请求ID',
                tab='测试',
                request_name='DupReq',
                request_id=7000,
                response_name='DupRsp',
                response_id=7002,
                request_params=[],
                response_params=[],
                field_labels={},
                is_active=True,
            )

    def test_protocol_ids_cannot_overlap_across_request_and_response_columns(self):
        with self.assertRaises(IntegrityError):
            GMCommand.objects.create(
                command_id='cross_dup',
                command_name='交叉重复',
                tab='测试',
                request_name='CrossReq',
                request_id=8000,
                response_name='CrossRsp',
                response_id=7000,
                request_params=[],
                response_params=[],
                field_labels={},
                is_active=True,
            )

    def test_request_and_response_id_must_differ_within_same_command(self):
        with self.assertRaises(IntegrityError):
            GMCommand.objects.create(
                command_id='same_ids',
                command_name='同号命令',
                tab='测试',
                request_name='SameReq',
                request_id=9000,
                response_name='SameRsp',
                response_id=9000,
                request_params=[],
                response_params=[],
                field_labels={},
                is_active=True,
            )
