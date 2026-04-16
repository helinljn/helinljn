from django.conf import settings
from django.db import migrations, models


INSERT_TRIGGER_NAME = 'gmtool_gmcommand_protocol_id_insert_guard'
UPDATE_TRIGGER_NAME = 'gmtool_gmcommand_protocol_id_update_guard'


def cleanup_superuser_command_permissions(apps, schema_editor):
    user_app_label, user_model_name = settings.AUTH_USER_MODEL.split('.')
    User = apps.get_model(user_app_label, user_model_name)
    UserCommandPermission = apps.get_model('gmtool', 'UserCommandPermission')

    superuser_ids = User.objects.filter(is_superuser=True).values_list('id', flat=True)
    UserCommandPermission.objects.filter(user_id__in=superuser_ids).delete()


def create_sqlite_protocol_id_triggers(apps, schema_editor):
    if schema_editor.connection.vendor != 'sqlite':
        return

    GMCommand = apps.get_model('gmtool', 'GMCommand')
    table = GMCommand._meta.db_table

    schema_editor.execute(f'''
        CREATE TRIGGER IF NOT EXISTS {INSERT_TRIGGER_NAME}
        BEFORE INSERT ON "{table}"
        FOR EACH ROW
        WHEN EXISTS (
            SELECT 1
            FROM "{table}"
            WHERE request_id IN (NEW.request_id, NEW.response_id)
               OR response_id IN (NEW.request_id, NEW.response_id)
        )
        BEGIN
            SELECT RAISE(ABORT, 'gmtool_gmcommand_protocol_id_conflict');
        END;
    ''')

    schema_editor.execute(f'''
        CREATE TRIGGER IF NOT EXISTS {UPDATE_TRIGGER_NAME}
        BEFORE UPDATE OF request_id, response_id ON "{table}"
        FOR EACH ROW
        WHEN EXISTS (
            SELECT 1
            FROM "{table}"
            WHERE id != NEW.id
              AND (
                    request_id IN (NEW.request_id, NEW.response_id)
                 OR response_id IN (NEW.request_id, NEW.response_id)
              )
        )
        BEGIN
            SELECT RAISE(ABORT, 'gmtool_gmcommand_protocol_id_conflict');
        END;
    ''')


def drop_sqlite_protocol_id_triggers(apps, schema_editor):
    if schema_editor.connection.vendor != 'sqlite':
        return

    schema_editor.execute(f'DROP TRIGGER IF EXISTS {INSERT_TRIGGER_NAME};')
    schema_editor.execute(f'DROP TRIGGER IF EXISTS {UPDATE_TRIGGER_NAME};')


class Migration(migrations.Migration):

    dependencies = [
        ('gmtool', '0002_remove_userprofile_role_delete_role'),
    ]

    operations = [
        migrations.RunPython(cleanup_superuser_command_permissions, migrations.RunPython.noop),
        migrations.RemoveIndex(
            model_name='gmcommand',
            name='gmcmd_reqid_idx',
        ),
        migrations.RemoveIndex(
            model_name='gmcommand',
            name='gmcmd_respid_idx',
        ),
        migrations.AlterField(
            model_name='gmcommand',
            name='field_labels',
            field=models.JSONField(
                default=dict,
                help_text='已同步字段的 id -> name 映射，用于结果展示（当前不递归展开更深层嵌套结构）',
                verbose_name='字段标签映射',
            ),
        ),
        migrations.AddConstraint(
            model_name='gmcommand',
            constraint=models.UniqueConstraint(fields=('request_id',), name='uniq_gmcmd_request_id'),
        ),
        migrations.AddConstraint(
            model_name='gmcommand',
            constraint=models.UniqueConstraint(fields=('response_id',), name='uniq_gmcmd_response_id'),
        ),
        migrations.AddConstraint(
            model_name='gmcommand',
            constraint=models.CheckConstraint(
                condition=~models.Q(request_id=models.F('response_id')),
                name='gmcmd_req_resp_distinct',
            ),
        ),

        migrations.RunPython(create_sqlite_protocol_id_triggers, drop_sqlite_protocol_id_triggers),
    ]
