#pragma once

#include <windows.h>

class PXCSenseManager;
class PXCFaceData;

class FaceTrackingProcessor
{
public:
	FaceTrackingProcessor(HWND window);
	void Process(HWND dialogWindow);
	void RegisterUser();
	void UnregisterUser();

private:
	HWND m_window;
	bool m_registerFlag;
	bool m_unregisterFlag;
	PXCFaceData* m_output;

	void CheckForDepthStream(PXCSenseManager* pp, HWND hwndDlg);
	void PerformRegistration();
	void PerformUnregistration();
};