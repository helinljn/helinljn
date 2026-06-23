"""Django表单定义"""
import json

from django import forms
from django.contrib.auth import get_user_model
from django.conf import settings
from django.utils.translation import gettext_lazy as _

from .command_parser import validate_command_ids
from .models import GMCommand, UserProfile

User = get_user_model()

ANNOUNCEMENT_TYPE_CHOICES = [
    ('1', _('周更新公告')),
    ('2', _('常驻公告')),
    ('3', _('轮播图')),
]


def get_announcement_config_errors():
    """返回公告平台/渠道配置错误。"""
    errors = []
    if not getattr(settings, 'ANNOUNCEMENT_PLATFORMS', []):
        errors.append(_('公告平台配置为空，请检查 ANNOUNCEMENT_PLATFORMS'))
    if not getattr(settings, 'ANNOUNCEMENT_CHANNELS', []):
        errors.append(_('公告渠道配置为空，请检查 ANNOUNCEMENT_CHANNELS'))
    return errors


def _choice_pairs(values):
    return [(value, value) for value in values]


class AnnouncementBaseForm(forms.Form):
    """公告表单共享配置。"""
    Platform = forms.ChoiceField(
        label=_('平台'),
        choices=(),
        widget=forms.Select(attrs={'class': 'form-select'}),
    )
    Channel = forms.ChoiceField(
        label=_('渠道'),
        choices=(),
        widget=forms.Select(attrs={'class': 'form-select'}),
    )

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.fields['Platform'].choices = _choice_pairs(getattr(settings, 'ANNOUNCEMENT_PLATFORMS', []))
        self.fields['Channel'].choices = _choice_pairs(getattr(settings, 'ANNOUNCEMENT_CHANNELS', []))

    def clean(self):
        cleaned_data = super().clean()
        config_errors = get_announcement_config_errors()
        if config_errors:
            for error in config_errors:
                self.add_error(None, error)
        return cleaned_data


class AnnouncementQueryForm(AnnouncementBaseForm):
    """公告查询表单。"""


class AnnouncementTypedForm(AnnouncementBaseForm):
    """需要公告类型的公告表单。"""
    AnnouncementType = forms.ChoiceField(
        label=_('公告类型'),
        choices=ANNOUNCEMENT_TYPE_CHOICES,
        widget=forms.Select(attrs={'class': 'form-select'}),
    )


class AnnouncementCreateForm(AnnouncementTypedForm):
    """公告发布表单。"""
    Title = forms.CharField(
        label=_('公告标题'),
        max_length=200,
        required=False,
        widget=forms.TextInput(attrs={'class': 'form-control'}),
    )
    Content = forms.CharField(
        label=_('公告正文'),
        max_length=5000,
        required=False,
        widget=forms.Textarea(attrs={'class': 'form-control', 'rows': 6}),
    )
    Priority = forms.CharField(
        label=_('常驻公告显示优先级'),
        initial=0,
        required=False,
        widget=forms.NumberInput(attrs={'class': 'form-control', 'min': 0}),
    )

    for field_name in ('Image_1', 'ImageLink_1', 'Image_2', 'ImageLink_2', 'Image_3', 'ImageLink_3'):
        locals()[field_name] = forms.CharField(
            label=field_name,
            max_length=500,
            required=False,
            widget=forms.TextInput(attrs={'class': 'form-control'}),
        )
    del field_name

    def clean(self):
        cleaned_data = super().clean()
        announcement_type = cleaned_data.get('AnnouncementType')
        if announcement_type in ('1', '2'):
            if not cleaned_data.get('Title'):
                self.add_error('Title', _('公告标题不能为空'))
            if not cleaned_data.get('Content'):
                self.add_error('Content', _('公告正文不能为空'))

        if announcement_type == '2':
            priority = cleaned_data.get('Priority')
            if priority in (None, ''):
                cleaned_data['Priority'] = 0
            else:
                try:
                    priority = int(priority)
                except (TypeError, ValueError):
                    self.add_error('Priority', _('常驻公告显示优先级必须是整数'))
                else:
                    if priority < 0:
                        self.add_error('Priority', _('常驻公告显示优先级不能小于 0'))
                    else:
                        cleaned_data['Priority'] = priority
        else:
            cleaned_data['Priority'] = 0
        return cleaned_data

    def to_payload(self, channel=None):
        """生成单渠道远端发布 payload。"""
        announcement_type = self.cleaned_data['AnnouncementType']
        channel = channel or self.cleaned_data['Channel']
        reserve_1 = ''
        if announcement_type == '2':
            reserve_1 = json.dumps(
                {
                    'priority': self.cleaned_data.get('Priority') or 0,
                },
                ensure_ascii=False,
                separators=(',', ':'),
            )
        title = self.cleaned_data.get('Title', '')
        content = self.cleaned_data.get('Content', '')
        if announcement_type == '3':
            title = ''
            content = ''
        image_payload = {
            'Image_1': '',
            'ImageLink_1': '',
            'Image_2': '',
            'ImageLink_2': '',
            'Image_3': '',
            'ImageLink_3': '',
        }
        if announcement_type == '3':
            image_payload = {
                'Image_1': self.cleaned_data.get('Image_1', '') or '',
                'ImageLink_1': self.cleaned_data.get('ImageLink_1', '') or '',
                'Image_2': self.cleaned_data.get('Image_2', '') or '',
                'ImageLink_2': self.cleaned_data.get('ImageLink_2', '') or '',
                'Image_3': self.cleaned_data.get('Image_3', '') or '',
                'ImageLink_3': self.cleaned_data.get('ImageLink_3', '') or '',
            }

        payload = {
            'Platform': self.cleaned_data['Platform'],
            'Channel': channel,
            'AnnouncementType': announcement_type,
            'AnnouncementId': '-1',
            'Title': title,
            'Content': content,
            **image_payload,
            'Reserve_1': reserve_1,
            'Reserve_2': '',
        }
        return payload


class AnnouncementDeleteForm(AnnouncementTypedForm):
    """公告删除表单。"""
    AnnouncementId = forms.IntegerField(
        label=_('公告ID'),
        min_value=1,
        widget=forms.NumberInput(attrs={'class': 'form-control', 'min': 1}),
    )

    def clean_AnnouncementId(self):
        announcement_id = self.cleaned_data.get('AnnouncementId')
        if announcement_id == -1:
            raise forms.ValidationError(_('删除公告必须使用实际公告 ID'))
        return announcement_id

    def to_payload(self):
        return {
            'Platform': self.cleaned_data['Platform'],
            'Channel': self.cleaned_data['Channel'],
            'AnnouncementType': self.cleaned_data['AnnouncementType'],
            'AnnouncementId': str(self.cleaned_data['AnnouncementId']),
        }

# 用户表单共享的 widgets 和 labels 定义，避免 DRY 违规
_USER_WIDGETS = {
    'username': forms.TextInput(attrs={'class': 'form-control'}),
    'email': forms.EmailInput(attrs={'class': 'form-control'}),
    'is_active': forms.CheckboxInput(attrs={'class': 'form-check-input'}),
}
_USER_LABELS = {
    'username': _('用户名'),
    'email': _('邮箱'),
    'is_active': _('是否启用'),
}


class PhoneMixin(forms.Form):
    """联系电话字段共享定义"""
    phone = forms.CharField(
        label=_('联系电话'),
        max_length=20,
        required=False,
        widget=forms.TextInput(attrs={'class': 'form-control'}),
    )


class UserCreateForm(PhoneMixin, forms.ModelForm):
    """创建用户表单"""
    password = forms.CharField(
        label=_('密码'),
        widget=forms.PasswordInput(attrs={'class': 'form-control'}),
        min_length=6,
    )
    password_confirm = forms.CharField(
        label=_('确认密码'),
        widget=forms.PasswordInput(attrs={'class': 'form-control'}),
    )

    class Meta:
        model = User
        fields = ['username', 'email', 'is_active']
        widgets = _USER_WIDGETS
        labels = _USER_LABELS

    def clean(self):
        cleaned_data = super().clean()
        password = cleaned_data.get('password')
        password_confirm = cleaned_data.get('password_confirm')
        if password and password_confirm and password != password_confirm:
            self.add_error('password_confirm', _('两次密码输入不一致'))

    def save(self, commit=True):
        user = super().save(commit=False)
        user.set_password(self.cleaned_data['password'])
        if commit:
            user.save()
            UserProfile.objects.update_or_create(
                user=user,
                defaults={
                    'phone': self.cleaned_data.get('phone', ''),
                }
            )
        return user


class UserEditForm(PhoneMixin, forms.ModelForm):
    """编辑用户表单"""
    new_password = forms.CharField(
        label=_('新密码（留空不修改）'),
        required=False,
        widget=forms.PasswordInput(attrs={'class': 'form-control'}),
        min_length=6,
    )

    class Meta:
        model = User
        fields = ['username', 'email', 'is_active']
        widgets = _USER_WIDGETS
        labels = _USER_LABELS

    def __init__(self, *args, **kwargs):
        self.is_self = kwargs.pop('is_self', False)
        self.is_target_super_admin = kwargs.pop('is_target_super_admin', False)
        super().__init__(*args, **kwargs)

        try:
            profile = self.instance.userprofile
        except UserProfile.DoesNotExist:
            profile = None

        if profile is not None:
            self.fields['phone'].initial = profile.phone

        # 如果编辑的是自己，禁用 is_active 复选框
        if self.is_self:
            self.fields['is_active'].disabled = True
            self.fields['is_active'].help_text = _('不能禁用自己的账号')

        # 如果编辑的目标是超级管理员，禁用 is_active 字段（禁止禁用）
        if self.is_target_super_admin:
            self.fields['is_active'].disabled = True
            self.fields['is_active'].help_text = _('超级管理员账号不可禁用')

    def save(self, commit=True):
        user = super().save(commit=False)
        new_password = self.cleaned_data.get('new_password')
        if new_password:
            user.set_password(new_password)
        if commit:
            user.save()

            profile, _ = UserProfile.objects.get_or_create(user=user)
            phone = profile.phone if self.fields['phone'].disabled else self.cleaned_data.get('phone', '')
            profile.phone = phone
            profile.save(update_fields=['phone'])
        return user


class AddGMCommandForm(forms.Form):
    """添加GM命令表单"""
    command_id = forms.CharField(
        label=_('命令编号'),
        max_length=20,
        widget=forms.TextInput(attrs={'class': 'form-control', 'placeholder': '例如：10226011'}),
        help_text=_('唯一的命令标识符，如 10226011'),
    )
    tab = forms.CharField(
        label=_('分组标签'),
        max_length=200,
        widget=forms.TextInput(attrs={'class': 'form-control', 'placeholder': _('e.g. 查询XXX')}),
    )
    command_name = forms.CharField(
        label=_('命令名称'),
        max_length=200,
        widget=forms.TextInput(attrs={'class': 'form-control', 'placeholder': _('命令显示名称')}),
    )
    request_name = forms.CharField(
        label=_('请求名'),
        max_length=100,
        widget=forms.TextInput(attrs={'class': 'form-control', 'placeholder': 'e.g. QueryXxxReq'}),
    )
    request_id = forms.IntegerField(
        label=_('协议请求ID'),
        widget=forms.NumberInput(attrs={'class': 'form-control', 'placeholder': '例如：4097'}),
        help_text=_('必须在所有命令中唯一（包括请求ID和响应ID）'),
    )
    request_desc = forms.CharField(
        label=_('请求描述'),
        max_length=500,
        required=False,
        widget=forms.TextInput(attrs={'class': 'form-control', 'placeholder': _('请求描述（可选）')}),
    )
    response_name = forms.CharField(
        label=_('响应名'),
        max_length=100,
        widget=forms.TextInput(attrs={'class': 'form-control', 'placeholder': 'e.g. QueryXxxRsp'}),
    )
    response_id = forms.IntegerField(
        label=_('协议响应ID'),
        widget=forms.NumberInput(attrs={'class': 'form-control', 'placeholder': '例如：4098'}),
        help_text=_('必须在所有命令中唯一，通常为请求ID + 1'),
    )
    response_desc = forms.CharField(
        label=_('响应描述'),
        max_length=500,
        required=False,
        widget=forms.TextInput(attrs={'class': 'form-control', 'placeholder': _('响应描述（可选）')}),
    )
    request_params = forms.CharField(
        label=_('请求参数定义（JSON）'),
        required=False,
        widget=forms.Textarea(attrs={'class': 'form-control', 'rows': 6, 'placeholder': '[\n  {"isnull": "false", "id": "AreaId", "type": "uint32", "name": "Channel Info"}\n]'}),
        help_text=_('请求参数定义的 JSON 数组。留空则使用默认的 AreaId/Partition/PlatId 字段。'),
    )
    response_params = forms.CharField(
        label=_('响应参数定义（JSON）'),
        required=False,
        widget=forms.Textarea(attrs={'class': 'form-control', 'rows': 6, 'placeholder': '[\n  {"isnull": "false", "id": "Result", "type": "int32", "name": "Result Id"}\n]'}),
        help_text=_('响应参数定义的 JSON 数组。留空则使用默认的 Result/RetMsg 字段。'),
    )

    def clean_command_id(self):
        command_id = self.cleaned_data.get('command_id', '')
        if GMCommand.objects.filter(command_id=command_id).exists():
            raise forms.ValidationError(_('命令 ID %(id)s 已存在') % {'id': command_id})
        return command_id

    def clean_request_params(self):
        raw = self.cleaned_data.get('request_params', '').strip()
        if not raw:
            return ''
        try:
            parsed = json.loads(raw)
            if not isinstance(parsed, list):
                raise forms.ValidationError(_('请求参数必须是 JSON 数组'))
            return raw
        except json.JSONDecodeError as e:
            raise forms.ValidationError(_('无效的 JSON: %(error)s') % {'error': str(e)})

    def clean_response_params(self):
        raw = self.cleaned_data.get('response_params', '').strip()
        if not raw:
            return ''
        try:
            parsed = json.loads(raw)
            if not isinstance(parsed, list):
                raise forms.ValidationError(_('响应参数必须是 JSON 数组'))
            return raw
        except json.JSONDecodeError as e:
            raise forms.ValidationError(_('无效的 JSON: %(error)s') % {'error': str(e)})

    def clean(self):
        cleaned_data = super().clean()
        request_id = cleaned_data.get('request_id')
        response_id = cleaned_data.get('response_id')

        if request_id is not None and response_id is not None:
            if request_id == response_id:
                self.add_error('response_id', _('请求 ID 和响应 ID 不能相同'))

            command_id = cleaned_data.get('command_id', '')
            is_valid, error_msg = validate_command_ids(command_id, request_id, response_id)
            if not is_valid:
                # 将错误信息添加到相关字段
                if str(request_id) in error_msg:
                    self.add_error('request_id', error_msg)
                if str(response_id) in error_msg:
                    self.add_error('response_id', error_msg)
                # 如果都没匹配到，添加到 request_id
                if not self.errors.get('request_id') and not self.errors.get('response_id'):
                    self.add_error('request_id', error_msg)

        return cleaned_data
