#pragma once

#include "pxcfaceconfiguration.h"
#include <windows.h>

class FaceTrackingAlertHandler : public PXCFaceConfiguration::AlertHandler
{
public:
	FaceTrackingAlertHandler(HWND hwndDlg);
	virtual void PXCAPI OnFiredAlert(const PXCFaceData::AlertData *alertData);

private:
	HWND dialogWindow;
};