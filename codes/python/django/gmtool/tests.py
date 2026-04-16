import json
import tempfile
from pathlib import Path
from unittest.mock import patch

from django.contrib.auth import get_user_model
from django.core.files.uploadedfile import SimpleUploadedFile
from django.db import DatabaseError, IntegrityError
from django.test import RequestFactory, TestCase, override_settings
from django.urls import reverse

from .command_parser import validate_json_command_ids
from .models import GMCommand, UserCommandPermission, UserProfile
from .security_utils import get_client_ip


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
                'responseid': 301,
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
                'responseid': 202,
            },
            'cmd_b': {
                'request_id': 300,
                'responseid': 301,
            },
        }

        is_valid, error_msg = validate_json_command_ids(data)

        self.assertTrue(is_valid)
        self.assertEqual(error_msg, '')


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
                    'responseid': 5001,
                    'respone': 'OldRsp',
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
                    'responseid': 6001,
                    'respone': 'NewRsp',
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
