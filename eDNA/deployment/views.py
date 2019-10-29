import os

from django.shortcuts import render, get_object_or_404, reverse
from django.http import HttpResponse, HttpResponseRedirect, Http404, FileResponse
from django.template import loader
from django.utils.encoding import smart_str

from .models import Device, Deployment
from .forms import DepthForm

def index(request):
    deployment_list =  Deployment.objects.order_by('has_data', 'pk')
    context = {'deployment_list': deployment_list}
    template = loader.get_template('deployment/index.html')
    return HttpResponse(template.render(context, request))

def detail(request, deployment_id):
    try:
        deployment = Deployment.objects.get(pk=deployment_id)
        response = HttpResponse()
        response['depth'] = deployment.depth
        response['pump_wait'] = deployment.pump_wait
        response['flow_volume'] = deployment.flow_volume
        response['flow_duration'] = deployment.flow_duration
    except Device.DoesNotExist:
        raise Http404("Deployment does not exist")
    return render(request, 'deployment/detail.html', {"device": device})


def set_depth(request, uid):
    if request.method == "POST":
        deployment = get_object_or_404(Deployment, eDNA_UID = uid)
        deployment.depth = int(request.POST['depth'])
        deployment.pump_wait = int(request.POST['pump_wait'])
        deployment.flow_volume = int(request.POST['flow_volume'])
        deployment.flow_duration = int(request.POST['flow_duration'])
        deployment.save()
        return HttpResponseRedirect(reverse('index', args=()))
    elif request.method == "GET":
        uid = request.GET['eDNA_UID']
        deployment = get_object_or_404(Deployment, eDNA_UID = uid)
        if deployment.has_data:
            file_name = "{}.txt".format(deployment.eDNA_UID)
            parent_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
            file_dir = os.path.join(parent_dir, 'eDNA', 'data', file_name)
            f_open = open(file_dir, 'rb')
            response = FileResponse(f_open, as_attachment=True)
            return response #response

def delete_deployment(request, deployment_id):
    if request.method == "POST":
        deployment = get_object_or_404(Deployment, pk=deployment_id)
        deployment.delete()
        return HttpResponseRedirect(reverse('index', args=()))


def upload_deployment_data(request, deployment_id):
    if request.method == "POST":
        deployment = get_object_or_404(Deployment, pk=deployment_id)
        if (deployment.has_data == False):
            data = request.FILES['data']
            parent_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
            file_name = "{}.txt".format(deployment.eDNA_UID)
            new_file = os.path.join(parent_dir, 'eDNA', 'data', file_name)
            with open(new_file, 'wb+') as dest:
                for chunk in data.chunks():
                    dest.write(chunk)
            deployment.has_data = True

        return HttpResponseRedirect(reverse('index', args=()))

def create_deployment(request, device_id):
    if request.method == "POST":
        device = Device.objects.get_or_create(
            device_id = device_id
        )
        eDNA_UID = request.POST['eDNA_UID']
        Deployment = Deployment.objects.get_or_create(
            device=device,
            eDNA_UID = eDNA_UID
        )
        return HTTPResponse(status=200)

