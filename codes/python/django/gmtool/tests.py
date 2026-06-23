import json
import tempfile
from pathlib import Path
from unittest.mock import patch

from django.contrib.auth import get_user_model
from django.contrib.auth.models import AnonymousUser
from django.core.files.uploadedfile import SimpleUploadedFile
from django.db import DatabaseError, IntegrityError
from django.http import JsonResponse
from django.test import RequestFactory, TestCase, override_settings
from django.urls import reverse

from .announcement_client import create_announcement, delete_announcement, query_announcements
from .command_parser import _enrich_schema_fields, sync_commands_to_db, validate_json_command_ids
from .command_services import validate_command_params_basic
from .context_processors import gmtool_permissions
from .decorators import announcement_permission_required, has_announcement_permission
from .forms import AnnouncementCreateForm, AnnouncementDeleteForm
from .idip_client import send_idip_command
from .models import AnnouncementLog, CommandLog, GMCommand, UserAnnouncementPermission, UserCommandPermission, UserProfile
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
            response_data, error_message, raw_response, error_type = query_announcements('IOS', 'OPPO', '3')

        self.assertEqual(response_data, [{'AnnouncementId': '1'}])
        self.assertEqual(error_message, '')
        self.assertEqual(raw_response, '[{"AnnouncementId": "1"}]')
        self.assertEqual(error_type, '')
        self.assertEqual(captured['url'], 'https://example.com/client/announcement.php')
        self.assertEqual(captured['params'], {
            'Platform': 'IOS',
            'Channel': 'OPPO',
            'AnnouncementType': '3',
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
            response_data, error_message, _raw_response, error_type = query_announcements('Android', '小米', '1')

        self.assertIsNone(response_data)
        self.assertIn('JSON 数组', error_message)
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

    @override_settings(ANNOUNCEMENT_BASE_URL='')
    def test_empty_base_url_is_configuration_error(self):
        response_data, error_message, raw_response, error_type = create_announcement({})

        self.assertIsNone(response_data)
        self.assertIn('未配置', error_message)
        self.assertEqual(raw_response, '')
        self.assertEqual(error_type, 'failed')

    @override_settings(ANNOUNCEMENT_BASE_URL='ftp://example.com')
    def test_invalid_base_url_scheme_is_configuration_error(self):
        response_data, error_message, _raw_response, error_type = query_announcements('Android', '小米', '1')

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
                        '1',
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
            response_data, error_message, raw_response, error_type = query_announcements('Android', '小米', '1')

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
    def test_create_form_builds_reserve_1_only_for_persistent_announcement(self):
        form = AnnouncementCreateForm(data={
            'Platform': 'Android',
            'Channel': '小米',
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
    def test_create_form_uses_empty_reserve_1_for_weekly_and_carousel(self):
        for announcement_type in ('1', '3'):
            form = AnnouncementCreateForm(data={
                'Platform': 'Android',
                'Channel': '小米',
                'AnnouncementType': announcement_type,
                'Title': '标题',
                'Content': '正文',
                'Priority': '0',
            })
            self.assertTrue(form.is_valid(), form.errors)
            self.assertEqual(form.to_payload('小米')['Reserve_1'], '')

    @override_settings(
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米'],
    )
    def test_carousel_create_form_does_not_require_title_or_content(self):
        form = AnnouncementCreateForm(data={
            'Platform': 'Android',
            'Channel': '小米',
            'AnnouncementType': '3',
            'Priority': '-1',
            'Image_1': 'https://example.com/a.png',
            'ImageLink_1': 'https://example.com/a',
        })

        self.assertTrue(form.is_valid(), form.errors)
        payload = form.to_payload()
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
                'Channel': '小米',
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
            payload = form.to_payload()
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
            'Channel': '小米',
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
            'Channel': '小米',
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
        })

        self.assertEqual(response.status_code, 302)
        self.assertTrue(response['Location'].endswith(reverse('gmtool:announcement_query') + '?Platform=Android'))

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
            'AnnouncementType': '1',
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
            'AnnouncementType': '1',
        })

        self.assertEqual(response.status_code, 200)
        mocked_query.assert_called_once_with('Android', '小米', '1')
        self.assertEqual(AnnouncementLog.objects.count(), 0)

    @override_settings(
        ANNOUNCEMENT_BASE_URL='http://example.com',
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米'],
    )
    @patch('gmtool.announcement_views.query_announcements')
    def test_query_result_delete_link_points_to_delete_page(self, mocked_query):
        mocked_query.return_value = ([{
            'Platform': 'Android',
            'Channel': '小米',
            'AnnouncementType': '2',
            'AnnouncementId': '123',
        }], '', '[]', '')

        response = self.client.get(reverse('gmtool:announcement_query'), {
            'Platform': 'Android',
            'Channel': '小米',
            'AnnouncementType': '2',
        })

        self.assertEqual(response.status_code, 200)
        self.assertContains(response, reverse('gmtool:announcement_delete'))
        self.assertContains(response, 'AnnouncementId=123')
        self.assertContains(response, 'Channel=%E5%B0%8F%E7%B1%B3')

    @override_settings(
        ANNOUNCEMENT_BASE_URL='http://example.com',
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米'],
    )
    def test_create_and_delete_pages_render_forms(self):
        create_response = self.client.get(reverse('gmtool:announcement_create'))
        delete_response = self.client.get(reverse('gmtool:announcement_delete'), {
            'Platform': 'Android',
            'Channel': '小米',
            'AnnouncementType': '2',
            'AnnouncementId': '123',
        })

        self.assertEqual(create_response.status_code, 200)
        self.assertContains(create_response, '发布公告')
        self.assertEqual(delete_response.status_code, 200)
        self.assertContains(delete_response, 'value="123"')

    @override_settings(
        ANNOUNCEMENT_BASE_URL='http://example.com',
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米', 'VIVO'],
    )
    @patch('gmtool.announcement_views.log_operation')
    @patch('gmtool.announcement_views.create_announcement')
    def test_create_single_channel_writes_log_per_remote_call(self, mocked_create, mocked_audit):
        mocked_create.return_value = ({'result': 'OK'}, '', 'OK', '')

        response = self.client.post(
            reverse('gmtool:announcement_create'),
            data=self._valid_create_payload(channel='小米'),
        )

        self.assertEqual(response.status_code, 302)
        mocked_create.assert_called_once()
        self.assertEqual(AnnouncementLog.objects.filter(action='create', status='success').count(), 1)
        detail = mocked_audit.call_args.kwargs['detail']
        self.assertEqual(detail['success_channels'], ['小米'])
        self.assertEqual(detail['remote_call_count'], 1)

    @override_settings(
        ANNOUNCEMENT_BASE_URL='http://example.com',
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米'],
    )
    @patch('gmtool.announcement_views.log_operation')
    @patch('gmtool.announcement_views.create_announcement')
    def test_create_carousel_without_title_or_content_sends_blank_payload(self, mocked_create, mocked_audit):
        mocked_create.return_value = ({'result': 'OK'}, '', 'OK', '')

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
        mocked_create.assert_called_once()
        payload = mocked_create.call_args.args[0]
        self.assertEqual(payload['Title'], '')
        self.assertEqual(payload['Content'], '')
        self.assertEqual(payload['Image_1'], 'https://example.com/banner.png')
        self.assertEqual(payload['ImageLink_1'], 'https://example.com/open')
        self.assertEqual(payload['Reserve_1'], '')
        self.assertEqual(AnnouncementLog.objects.filter(action='create', status='success').count(), 1)

    @override_settings(
        ANNOUNCEMENT_BASE_URL='http://example.com',
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米'],
    )
    @patch('gmtool.announcement_views.log_operation')
    @patch('gmtool.announcement_views.delete_announcement')
    @patch('gmtool.announcement_views.query_announcements')
    @patch('gmtool.announcement_views.create_announcement')
    def test_create_persistent_does_not_query_or_delete_old_announcements(
        self,
        mocked_create,
        mocked_query,
        mocked_delete,
        mocked_audit,
    ):
        mocked_create.return_value = ({'result': 'OK'}, '', 'OK', '')

        response = self.client.post(
            reverse('gmtool:announcement_create'),
            data=self._valid_create_payload(announcement_type='2'),
        )

        self.assertEqual(response.status_code, 302)
        mocked_query.assert_not_called()
        mocked_delete.assert_not_called()
        mocked_create.assert_called_once()

    @override_settings(
        ANNOUNCEMENT_BASE_URL='http://example.com',
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米'],
    )
    @patch('gmtool.announcement_views.create_announcement')
    @patch('gmtool.announcement_views.delete_announcement')
    @patch('gmtool.announcement_views.query_announcements')
    def test_weekly_create_deletes_old_announcements_before_create(self, mocked_query, mocked_delete, mocked_create):
        mocked_query.return_value = ([{
            'Platform': 'Android',
            'Channel': '小米',
            'AnnouncementType': '1',
            'AnnouncementId': 'old-1',
        }], '', '[]', '')
        mocked_delete.return_value = ({'result': 'OK'}, '', 'OK', '')
        mocked_create.return_value = ({'result': 'OK'}, '', 'OK', '')

        response = self.client.post(
            reverse('gmtool:announcement_create'),
            data=self._valid_create_payload(announcement_type='1'),
        )

        self.assertEqual(response.status_code, 302)
        mocked_query.assert_called_once_with('Android', '小米', '1')
        mocked_delete.assert_called_once_with('Android', '小米', '1', 'old-1')
        mocked_create.assert_called_once()
        self.assertEqual(AnnouncementLog.objects.filter(action='delete').count(), 1)
        self.assertEqual(AnnouncementLog.objects.filter(action='create').count(), 1)

    @override_settings(
        ANNOUNCEMENT_BASE_URL='http://example.com',
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米'],
    )
    @patch('gmtool.announcement_views.create_announcement')
    @patch('gmtool.announcement_views.query_announcements')
    def test_weekly_create_does_not_create_when_old_query_fails(self, mocked_query, mocked_create):
        mocked_query.return_value = (None, '查询失败', '', 'failed')

        response = self.client.post(
            reverse('gmtool:announcement_create'),
            data=self._valid_create_payload(announcement_type='1'),
        )

        self.assertEqual(response.status_code, 400)
        mocked_create.assert_not_called()
        self.assertEqual(AnnouncementLog.objects.count(), 0)

    @override_settings(
        ANNOUNCEMENT_BASE_URL='http://example.com',
        ANNOUNCEMENT_PLATFORMS=['Android'],
        ANNOUNCEMENT_CHANNELS=['小米', 'VIVO'],
    )
    @patch('gmtool.announcement_views.delete_announcement')
    def test_delete_uses_announcement_own_channel_and_type(self, mocked_delete):
        mocked_delete.return_value = ({'result': 'OK'}, '', 'OK', '')

        response = self.client.post(reverse('gmtool:announcement_delete'), data={
            'Platform': 'Android',
            'Channel': 'VIVO',
            'AnnouncementType': '3',
            'AnnouncementId': '99',
            'return_Platform': 'Android',
            'return_Channel': ['小米'],
            'return_AnnouncementType': '1',
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
    @patch('gmtool.announcement_views.AnnouncementLog.objects.create', side_effect=DatabaseError('db failed'))
    @patch('gmtool.announcement_views.create_announcement')
    def test_remote_success_still_redirects_when_log_persistence_fails(
        self,
        mocked_create,
        mocked_log_create,
        mocked_audit,
    ):
        mocked_create.return_value = ({'result': 'OK'}, '', 'OK', '')

        response = self.client.post(
            reverse('gmtool:announcement_create'),
            data=self._valid_create_payload(),
        )

        self.assertEqual(response.status_code, 302)
        mocked_log_create.assert_called_once()
        self.assertFalse(mocked_audit.call_args.kwargs['detail']['log_persisted'])

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
