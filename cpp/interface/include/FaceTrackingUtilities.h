#pragma once

#include <windows.h>
#include <map>
#include "pxcdefs.h"
#include "pxcfaceconfiguration.h"
#include "pxccapture.h"

enum StatusWindowPart { statusPart, alertPart };
extern std::map<int, PXCFaceConfiguration::TrackingModeType> s_profilesMap;
extern std::map<int, PXCCapture::DeviceInfo> g_deviceInfoMap;

class FaceTrackingUtilities
{
public:
	static int GetChecked(HMENU menu);
	static pxcCHAR* GetCheckedDevice(HWND dialogWindow);
	static PXCCapture::DeviceInfo* GetCheckedDeviceInfo(HWND dialogWindow);
	static pxcCHAR* GetCheckedModule(HWND dialogWindow);
	static void SetStatus(HWND dialogWindow, pxcCHAR *line, StatusWindowPart part);
	static bool IsModuleSelected(HWND hwndDlg, const int moduleID);
	static bool GetRecordState(HWND hwndDlg);
	static bool GetPlaybackState(HWND DialogWindow);
	static PXCFaceConfiguration::TrackingModeType GetCheckedProfile(HWND dialogWindow);
	static const int TextHeight = 16;
	static bool printDistances;
};
