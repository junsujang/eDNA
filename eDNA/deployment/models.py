from django.db import models

class Device(models.Model):
    device_id = models.IntegerField()

    def __str__(self):
        return str(self.device_id)

# A deployment ism associated with a single device
class Deployment(models.Model):
    device = models.ForeignKey(Device, on_delete=models.CASCADE)
    eDNA_UID = models.CharField(max_length=32)
    deployment_date = models.DateTimeField('date deployed')
    depth = models.IntegerField(default=0)
    pump_wait = models.IntegerField(default=0)
    flow_volume = models.IntegerField(default=0)
    flow_duration = models.IntegerField(default=0)
    has_data = models.BooleanField(default=False)

    
    def __str__(self):
        return self.eDNA_UID