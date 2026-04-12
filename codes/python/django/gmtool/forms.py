"""Django表单定义"""
from django import forms
from django.contrib.auth.models import User
from django.utils.translation import gettext_lazy as _
from .models import Role, UserProfile


class RoleAndPhoneMixin(forms.Form):
    """角色分组和联系电话字段的共享定义"""
    role = forms.ModelChoiceField(
        label=_('Role'),
        queryset=Role.objects.filter(is_super_admin=False),
        required=False,
        widget=forms.Select(attrs={'class': 'form-select'}),
    )
    phone = forms.CharField(
        label=_('Phone'),
        max_length=20,
        required=False,
        widget=forms.TextInput(attrs={'class': 'form-control'}),
    )


class UserCreateForm(RoleAndPhoneMixin, forms.ModelForm):
    """创建用户表单 — 不允许选择超级管理员角色（超管仅通过初始化创建）"""
    password = forms.CharField(
        label=_('Password'),
        widget=forms.PasswordInput(attrs={'class': 'form-control'}),
        min_length=6,
    )
    password_confirm = forms.CharField(
        label=_('Confirm Password'),
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
            'username': _('Username'),
            'email': _('Email'),
            'is_active': _('Active'),
        }

    def clean(self):
        cleaned_data = super().clean()
        password = cleaned_data.get('password')
        password_confirm = cleaned_data.get('password_confirm')
        if password and password_confirm and password != password_confirm:
            self.add_error('password_confirm', _('Passwords do not match'))

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
        label=_('New Password (leave blank to keep unchanged)'),
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
            'username': _('Username'),
            'email': _('Email'),
            'is_active': _('Active'),
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
            self.fields['is_active'].help_text = _('You cannot disable your own account')
        # 如果编辑的目标是超级管理员，禁用角色和 is_active 字段（禁止降级）
        if self.is_target_super_admin:
            self.fields['role'].disabled = True
            self.fields['role'].help_text = _('Super admin role cannot be changed')
            self.fields['is_active'].disabled = True
            self.fields['is_active'].help_text = _('Super admin account cannot be disabled')

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
            'name': _('Role Identifier'),
            'display_name': _('Display Name'),
            'description': _('Description'),
        }
        help_texts = {
            'name': _('Cannot be "super_admin", which is reserved for the system'),
        }

    def clean_name(self):
        name = self.cleaned_data.get('name', '')
        if name == 'super_admin':
            raise forms.ValidationError(_('The role identifier "super_admin" is reserved and cannot be used'))
        return name
