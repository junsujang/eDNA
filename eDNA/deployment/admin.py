from django.contrib import admin

from .models import Device, Deployment

admin.site.register(Device)
admin.site.register(Deployment)