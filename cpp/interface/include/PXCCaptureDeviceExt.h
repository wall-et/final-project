#pragma once
#include "pxccapture.h"

class PXCCaptureDeviceExt {
public:
	PXCCaptureDeviceExt(PXCCapture::Device *device1):device(device1) {}
	pxcStatus QueryProperty(PXCCapture::Device::Property label, pxcF32 *value)     { return device->QueryProperty(label,value);    }
	pxcStatus SetPropertyAuto(PXCCapture::Device::Property label, pxcBool ifauto)  { return device->SetPropertyAuto(label,ifauto); }
	pxcStatus SetProperty(PXCCapture::Device::Property label, pxcF32 value)        { return device->SetProperty(label,value);      }
    pxcStatus PXCAPI QueryPropertyInfo(PXCCapture::Device::Property label, PXCCapture::Device::PropertyInfo* propertyInfo) {
        return device->QueryPropertyInfo(label,propertyInfo);
    }
protected:
	PXCCapture::Device *device;
};


