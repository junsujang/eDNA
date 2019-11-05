# Generated by Django 2.2.6 on 2019-10-30 00:33

import datetime
from django.db import migrations, models
from django.utils.timezone import utc


class Migration(migrations.Migration):

    dependencies = [
        ('deployment', '0004_deployment_has_data'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='deployment',
            name='deployment_id',
        ),
        migrations.AlterField(
            model_name='deployment',
            name='deployment_date',
            field=models.DateTimeField(default=datetime.datetime(2019, 10, 30, 0, 33, 30, 384752, tzinfo=utc), verbose_name='date deployed'),
        ),
        migrations.AlterField(
            model_name='device',
            name='device_id',
            field=models.IntegerField(),
        ),
    ]