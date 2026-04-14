from django.db import migrations


class Migration(migrations.Migration):

    dependencies = [
        ('gmtool', '0008_add_indexes_and_role_constraint'),
    ]

    operations = [
        migrations.RemoveIndex(
            model_name='gmcommand',
            name='gmcmd_cmdid_idx',
        ),
    ]
