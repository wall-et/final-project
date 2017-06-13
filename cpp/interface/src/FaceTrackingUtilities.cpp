#include <WindowsX.h>
#include "FaceTrackingUtilities.h"
#include "resource.h"
#include <commctrl.h>

bool FaceTrackingUtilities::printDistances = false;

PXCFaceConfiguration::TrackingModeType FaceTrackingUtilities::GetCheckedProfile(HWND dialogWindow)
{
	HMENU menu = GetSubMenu(GetMenu(dialogWindow), 2);
	static pxcCHAR line[256];
	int index = GetChecked(menu);
	return s_profilesMap[index];
}

bool FaceTrackingUtilities::GetPlaybackState(HWND DialogWindow)
{
	return (GetMenuState(GetMenu(DialogWindow), ID_MODE_PLAYBACK, MF_BYCOMMAND) & MF_CHECKED) != 0;
}

bool FaceTrackingUtilities::GetRecordState(HWND hwndDlg)
{
	return (GetMenuState(GetMenu(hwndDlg), ID_MODE_RECORD, MF_BYCOMMAND) & MF_CHECKED) != 0;
}

bool FaceTrackingUtilities::IsModuleSelected(HWND hwndDlg, const int moduleID)
{
	return (Button_GetState(GetDlgItem(hwndDlg, moduleID)) & BST_CHECKED) != 0;
}

void FaceTrackingUtilities::SetStatus(HWND dialogWindow, pxcCHAR *line, StatusWindowPart part)
{
	HWND hwndStatus = GetDlgItem(dialogWindow, IDC_STATUS);
	SendMessage(hwndStatus, SB_SETTEXT, (WPARAM)(INT) part, (LPARAM) (LPSTR) line);
	UpdateWindow(dialogWindow);
}

pxcCHAR* FaceTrackingUtilities::GetCheckedModule(HWND dialogWindow)
{
	HMENU menu = GetSubMenu(GetMenu(dialogWindow), 1);
	static pxcCHAR line[256];

	GetMenuString(menu, GetChecked(menu), line, sizeof(line) / sizeof(pxcCHAR), MF_BYPOSITION);
	return line;
}

pxcCHAR* FaceTrackingUtilities::GetCheckedDevice(HWND dialogWindow)
{
	HMENU menu = GetSubMenu(GetMenu(dialogWindow), 0);
	static pxcCHAR line[256];
	GetMenuString(menu, GetChecked(menu), line, sizeof(line) / sizeof(pxcCHAR), MF_BYPOSITION);
	return line;
}

int FaceTrackingUtilities::GetChecked(HMENU menu)
{
	for (int i = 0; i < GetMenuItemCount(menu); ++i)
	{
		if (GetMenuState(menu, i, MF_BYPOSITION) & MF_CHECKED) 
			return i;
	}

	return 0;
}

PXCCapture::DeviceInfo* FaceTrackingUtilities::GetCheckedDeviceInfo(HWND hwndDlg)
{
	HMENU menu=GetSubMenu(GetMenu(hwndDlg),0);	// ID_DEVICE
	int pos = GetChecked(menu);
	if(g_deviceInfoMap.size() == 0)
		return NULL;
	else
		return &g_deviceInfoMap[pos];
}
