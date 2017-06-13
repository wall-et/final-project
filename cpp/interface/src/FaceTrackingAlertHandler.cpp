#include "FaceTrackingAlertHandler.h"
#include "FaceTrackingUtilities.h"

FaceTrackingAlertHandler::FaceTrackingAlertHandler(HWND hwndDlg) : dialogWindow(hwndDlg)
{

}

void PXCAPI FaceTrackingAlertHandler::OnFiredAlert(const PXCFaceData::AlertData *alertData)
{
	switch(alertData->label)
	{
	case PXCFaceData::AlertData::ALERT_NEW_FACE_DETECTED:
		FaceTrackingUtilities::SetStatus(dialogWindow, L"ALERT_NEW_FACE_DETECTED", alertPart);
		break;
	case PXCFaceData::AlertData::ALERT_FACE_OUT_OF_FOV:
		FaceTrackingUtilities::SetStatus(dialogWindow, L"ALERT_FACE_OUT_OF_FOV", alertPart);
		break;
	case PXCFaceData::AlertData::ALERT_FACE_BACK_TO_FOV:
		FaceTrackingUtilities::SetStatus(dialogWindow, L"ALERT_FACE_BACK_TO_FOV", alertPart);
		break;
	case PXCFaceData::AlertData::ALERT_FACE_OCCLUDED:
		FaceTrackingUtilities::SetStatus(dialogWindow, L"ALERT_FACE_OCCLUDED", alertPart);
		break;
	case PXCFaceData::AlertData::ALERT_FACE_NO_LONGER_OCCLUDED:
		FaceTrackingUtilities::SetStatus(dialogWindow, L"ALERT_FACE_NO_LONGER_OCCLUDED", alertPart);
		break;
	case PXCFaceData::AlertData::ALERT_FACE_LOST:
		FaceTrackingUtilities::SetStatus(dialogWindow, L"ALERT_FACE_LOST", alertPart);
		break;
	default:
		FaceTrackingUtilities::SetStatus(dialogWindow, L"UNKNOWN_ALERT", alertPart);
		break;
	}
}
