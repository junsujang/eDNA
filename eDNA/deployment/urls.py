from django.urls import path
from . import views

urlpatterns = [
    path('', views.index, name='index'),
    path('device/<int:device_id>/', views.detail, name='detail'),
    path('<int:uid>', views.set_depth, name='depth'),
    path('delete/<int:deployment_id>', views.delete_deployment, name="delete_deployment"),
    path('upload/<int:deployment_id>', views.upload_deployment_data, name="upload_deployment_data"),
    path('create/<int:device_id>', views.create_deployment, name="create_deployment")
]