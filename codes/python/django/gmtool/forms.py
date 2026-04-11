"""Django表单定义"""
from django import forms
from django.contrib.auth.models import User
from django.utils.translation import gettext_lazy as _
from .models import Role, UserProfile


class RoleAndPhoneMixin(forms.Form):
    """角色分组和联系电话字段的共享定义"""
    role = forms.ModelChoiceField(
        label=_('Role'),
        queryset=Role.objects.all(),
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
    """创建用户表单"""
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
    """编辑用户表单"""
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
        super().__init__(*args, **kwargs)
        # 填充当前角色和电话
        if self.instance and hasattr(self.instance, 'userprofile'):
            self.fields['role'].initial = self.instance.userprofile.role
            self.fields['phone'].initial = self.instance.userprofile.phone
        # 如果编辑的是自己，禁用 is_active 复选框
        if self.is_self:
            self.fields['is_active'].disabled = True
            self.fields['is_active'].help_text = _('You cannot disable your own account')

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
    """角色表单"""
    class Meta:
        model = Role
        fields = ['name', 'display_name', 'description', 'is_super_admin']
        widgets = {
            'name': forms.TextInput(attrs={'class': 'form-control'}),
            'display_name': forms.TextInput(attrs={'class': 'form-control'}),
            'description': forms.Textarea(attrs={'class': 'form-control', 'rows': 3}),
            'is_super_admin': forms.CheckboxInput(attrs={'class': 'form-check-input'}),
        }
        labels = {
            'name': _('Role Identifier'),
            'display_name': _('Display Name'),
            'description': _('Description'),
            'is_super_admin': _('Super Admin'),
        }

    def clean_is_super_admin(self):
        """防止创建多个超级管理员角色（系统设计仅允许一个 super_admin）"""
        is_super = self.cleaned_data.get('is_super_admin', False)
        if is_super:
            # 编辑时：如果当前实例已经是 super_admin，允许保留
            if self.instance and self.instance.pk and self.instance.is_super_admin:
                return is_super
            # 创建或从非超管改为超管时：检查是否已存在超管角色
            qs = Role.objects.filter(is_super_admin=True)
            if self.instance and self.instance.pk:
                qs = qs.exclude(pk=self.instance.pk)
            if qs.exists():
                raise forms.ValidationError(_('A super admin role already exists. Only one is allowed.'))
        return is_super
