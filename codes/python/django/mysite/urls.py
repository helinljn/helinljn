"""
URL configuration for mysite project.

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/5.2/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  path('', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  path('', Home.as_view(), name='home')
Including another URLconf
    1. Import the include() function: from django.urls import include, path
    2. Add a URL to urlpatterns:  path('blog/', include('blog.urls'))
"""
from django.urls import include, path, re_path
from django.views.generic import RedirectView
from gmtool.views import custom_403, custom_404, custom_500

handler404 = custom_404
handler500 = custom_500
handler403 = custom_403

urlpatterns = [
    path('i18n/', include('django.conf.urls.i18n')),
    path('gmtool/', include('gmtool.urls')),
    path('', RedirectView.as_view(url='/gmtool/', permanent=False)),
    # 兜底：未匹配的URL显示自定义404页面（DEBUG=True时也生效）
    re_path(r'^.*$', custom_404),
]
