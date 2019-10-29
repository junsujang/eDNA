from django import forms

class DepthForm(forms.Form):
    depth = forms.CharField(label='Depth', max_length=4)