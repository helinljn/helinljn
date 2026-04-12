"""Django表单定义"""
import json

from django import forms
from django.contrib.auth.models import User
from django.utils.translation import gettext_lazy as _
from .models import Role, UserProfile


class RoleAndPhoneMixin(forms.Form):
    """角色分组和联系电话字段的共享定义"""
    role = forms.ModelChoiceField(
        label=_('角色'),
        queryset=Role.objects.filter(is_super_admin=False),
        required=False,
        widget=forms.Select(attrs={'class': 'form-select'}),
    )
    phone = forms.CharField(
        label=_('联系电话'),
        max_length=20,
        required=False,
        widget=forms.TextInput(attrs={'class': 'form-control'}),
    )


class UserCreateForm(RoleAndPhoneMixin, forms.ModelForm):
    """创建用户表单 — 不允许选择超级管理员角色（超管仅通过初始化创建）"""
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
        widgets = {
            'username': forms.TextInput(attrs={'class': 'form-control'}),
            'email': forms.EmailInput(attrs={'class': 'form-control'}),
            'is_active': forms.CheckboxInput(attrs={'class': 'form-check-input'}),
        }
        labels = {
            'username': _('用户名'),
            'email': _('邮箱'),
            'is_active': _('是否启用'),
        }

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
                    'role': self.cleaned_data.get('role'),
                    'phone': self.cleaned_data.get('phone', ''),
                }
            )
        return user


class UserEditForm(RoleAndPhoneMixin, forms.ModelForm):
    """编辑用户表单 — 禁止提升/降级超级管理员权限"""
    new_password = forms.CharField(
        label=_('新密码（留空不修改）'),
        required=False,
        widget=forms.PasswordInput(attrs={'class': 'form-control'}),
        min_length=6,
    )

    class Meta:
        model = User
        fields = ['username', 'email', 'is_active']
        widgets = {
            'username': forms.TextInput(attrs={'class': 'form-control'}),
            'email': forms.EmailInput(attrs={'class': 'form-control'}),
            'is_active': forms.CheckboxInput(attrs={'class': 'form-check-input'}),
        }
        labels = {
            'username': _('用户名'),
            'email': _('邮箱'),
            'is_active': _('是否启用'),
        }

    def __init__(self, *args, **kwargs):
        self.is_self = kwargs.pop('is_self', False)
        self.is_target_super_admin = kwargs.pop('is_target_super_admin', False)
        super().__init__(*args, **kwargs)
        # 填充当前角色和电话
        if self.instance and hasattr(self.instance, 'userprofile'):
            self.fields['role'].initial = self.instance.userprofile.role
            self.fields['phone'].initial = self.instance.userprofile.phone
        # 如果编辑的是自己，禁用 is_active 复选框
        if self.is_self:
            self.fields['is_active'].disabled = True
            self.fields['is_active'].help_text = _('不能禁用自己的账号')
        # 如果编辑的目标是超级管理员，禁用角色和 is_active 字段（禁止降级）
        if self.is_target_super_admin:
            self.fields['role'].disabled = True
            self.fields['role'].help_text = _('超级管理员角色不可更改')
            self.fields['is_active'].disabled = True
            self.fields['is_active'].help_text = _('超级管理员账号不可禁用')

    def save(self, commit=True):
        user = super().save(commit=False)
        new_password = self.cleaned_data.get('new_password')
        if new_password:
            user.set_password(new_password)
        if commit:
            user.save()
            UserProfile.objects.update_or_create(
                user=user,
                defaults={
                    'role': self.cleaned_data.get('role'),
                    'phone': self.cleaned_data.get('phone', ''),
                }
            )
        return user


class RoleForm(forms.ModelForm):
    """角色表单 — 不允许创建或编辑超级管理员角色（仅通过初始化创建）"""
    class Meta:
        model = Role
        fields = ['name', 'display_name', 'description']
        widgets = {
            'name': forms.TextInput(attrs={'class': 'form-control'}),
            'display_name': forms.TextInput(attrs={'class': 'form-control'}),
            'description': forms.Textarea(attrs={'class': 'form-control', 'rows': 3}),
        }
        labels = {
            'name': _('角色标识'),
            'display_name': _('显示名称'),
            'description': _('角色描述'),
        }
        help_texts = {
            'name': _('不能为"super_admin"，该标识为系统保留'),
        }

    def clean_name(self):
        name = self.cleaned_data.get('name', '')
        if name == 'super_admin':
            raise forms.ValidationError(_('角色标识"super_admin"为系统保留，不可使用'))
        return name


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
        from .models import GMCommand
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

            from .command_parser import validate_command_ids
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
