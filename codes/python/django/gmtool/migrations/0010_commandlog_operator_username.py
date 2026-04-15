from django.db import migrations, models
from django.utils.translation import gettext_lazy as _


class Migration(migrations.Migration):

    dependencies = [
        ('gmtool', '0009_remove_redundant_gmcmd_cmdid_idx'),
    ]

    operations = [
        migrations.AddField(
            model_name='commandlog',
            name='operator_username',
            field=models.CharField(blank=True, default='', max_length=150, verbose_name=_('操作用户名')),
        ),
    ]
