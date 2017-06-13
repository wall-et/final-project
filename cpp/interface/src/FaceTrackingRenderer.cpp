#include "FaceTrackingRenderer.h"
#include "FaceTrackingUtilities.h"
#include "pxccapture.h"
#include <map>

FaceTrackingRenderer::~FaceTrackingRenderer()
{
	if (m_landmarkPoints != NULL)
		delete[] m_landmarkPoints;
}

FaceTrackingRenderer::FaceTrackingRenderer(HWND window) : m_window(window)
{
	m_landmarkPoints = NULL;
	m_senseManager = NULL;
	m_expressionMap = InitExpressionsMap();
}

void FaceTrackingRenderer::SetOutput(PXCFaceData* output)
{
	m_currentFrameOutput = output;
}

void FaceTrackingRenderer::SetSenseManager(PXCSenseManager* senseManager)
{
	m_senseManager = senseManager;
	m_senseManager->QueryCaptureManager()->QueryDevice()->SetMirrorMode(PXCCapture::Device::MirrorMode::MIRROR_MODE_HORIZONTAL);
}

PXCSenseManager* FaceTrackingRenderer::GetSenseManager()
{
	return m_senseManager;
}

void FaceTrackingRenderer::Render()
{
	DrawFrameRate();
	DrawGraphics(m_currentFrameOutput);
	RefreshUserInterface();
}

void FaceTrackingRenderer::DrawFrameRate()
{
	m_frameRateCalcuator.Tick();
	if (m_frameRateCalcuator.IsFrameRateReady())
	{
		int fps = m_frameRateCalcuator.GetFrameRate();

		pxcCHAR line[1024];
		swprintf_s<1024>(line, L"Rate (%d fps)", fps);
		FaceTrackingUtilities::SetStatus(m_window, line, statusPart);
	}
}

void FaceTrackingRenderer::RefreshUserInterface()
{
	if (!m_bitmap) return;

	HWND panel = GetDlgItem(m_window, IDC_PANEL);
	RECT rc;
	GetClientRect(panel, &rc);

	HDC dc = GetDC(panel);
	if(!dc)
	{
		return;
	}

	HBITMAP bitmap = CreateCompatibleBitmap(dc, rc.right, rc.bottom);

	if(!bitmap)
	{

		ReleaseDC(panel, dc);
		return;
	}
	HDC dc2 = CreateCompatibleDC(dc);
	if (!dc2)
	{
		DeleteObject(bitmap);
		ReleaseDC(m_window,dc);
		return;
	}
	SelectObject(dc2, bitmap);
	SetStretchBltMode(dc2, COLORONCOLOR);

	/* Draw the main window */
	HDC dc3 = CreateCompatibleDC(dc);

	if (!dc3)
	{
		DeleteDC(dc2);
		DeleteObject(bitmap);
		ReleaseDC(m_window,dc);
		return;
	}

	SelectObject(dc3, m_bitmap);
	BITMAP bm;
	GetObject(m_bitmap, sizeof(BITMAP), &bm);

	bool scale = Button_GetState(GetDlgItem(m_window, IDC_SCALE)) & BST_CHECKED;
	if (scale)
	{
		RECT rc1 = GetResizeRect(rc, bm);
		StretchBlt(dc2, rc1.left, rc1.top, rc1.right, rc1.bottom, dc3, 0, 0,bm.bmWidth, bm.bmHeight, SRCCOPY);	
	} 
	else
	{
		BitBlt(dc2, 0, 0, rc.right, rc.bottom, dc3, 0, 0, SRCCOPY);
	}

	DeleteDC(dc3);
	DeleteDC(dc2);
	ReleaseDC(m_window,dc);

	HBITMAP bitmap2 = (HBITMAP)SendMessage(panel, STM_GETIMAGE, 0, 0);
	if (bitmap2) DeleteObject(bitmap2);
	SendMessage(panel, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bitmap);
	InvalidateRect(panel, 0, TRUE);
	 DeleteObject(bitmap);
}

RECT FaceTrackingRenderer::GetResizeRect(RECT rectangle, BITMAP bitmap)
{
	RECT resizedRectangle;
	float sx = (float)rectangle.right / (float)bitmap.bmWidth;
	float sy = (float)rectangle.bottom / (float)bitmap.bmHeight;
	float sxy = sx < sy ? sx : sy;
	resizedRectangle.right = (int)(bitmap.bmWidth * sxy);
	resizedRectangle.left = (rectangle.right - resizedRectangle.right) / 2 + rectangle.left;
	resizedRectangle.bottom = (int)(bitmap.bmHeight * sxy);
	resizedRectangle.top = (rectangle.bottom - resizedRectangle.bottom) / 2 + rectangle.top;
	return resizedRectangle;
}

std::map<PXCFaceData::ExpressionsData::FaceExpression, std::wstring> FaceTrackingRenderer::InitExpressionsMap()
{
	std::map<PXCFaceData::ExpressionsData::FaceExpression, std::wstring> map;
	map[PXCFaceData::ExpressionsData::EXPRESSION_SMILE] =  std::wstring(L"Smile");
	map[PXCFaceData::ExpressionsData::EXPRESSION_MOUTH_OPEN] = std::wstring(L"Mouth Open");
	map[PXCFaceData::ExpressionsData::EXPRESSION_KISS] = std::wstring(L"Kiss");
	map[PXCFaceData::ExpressionsData::EXPRESSION_EYES_TURN_LEFT] = std::wstring(L"Eyes Turn Left");
	map[PXCFaceData::ExpressionsData::EXPRESSION_EYES_TURN_RIGHT] = std::wstring(L"Eyes Turn Right");
	map[PXCFaceData::ExpressionsData::EXPRESSION_EYES_UP] = std::wstring(L"Eyes Up");
	map[PXCFaceData::ExpressionsData::EXPRESSION_EYES_DOWN] = std::wstring(L"Eyes Down");
	map[PXCFaceData::ExpressionsData::EXPRESSION_BROW_RAISER_LEFT] = std::wstring(L"Brow Raised Left");
	map[PXCFaceData::ExpressionsData::EXPRESSION_BROW_RAISER_RIGHT] = std::wstring(L"Brow Raised Right");
	map[PXCFaceData::ExpressionsData::EXPRESSION_BROW_LOWERER_LEFT] = std::wstring(L"Brow Lowered Left");
	map[PXCFaceData::ExpressionsData::EXPRESSION_BROW_LOWERER_RIGHT] = std::wstring(L"Brow Lowered Right");
	map[PXCFaceData::ExpressionsData::EXPRESSION_EYES_CLOSED_LEFT] = std::wstring(L"Closed Eye Left");
	map[PXCFaceData::ExpressionsData::EXPRESSION_EYES_CLOSED_RIGHT] = std::wstring(L"Closed Eye Right");
	map[PXCFaceData::ExpressionsData::EXPRESSION_TONGUE_OUT] = std::wstring(L"Tongue Out");
	map[PXCFaceData::ExpressionsData::EXPRESSION_PUFF_RIGHT] = std::wstring(L"Puff Right Cheek");
	map[PXCFaceData::ExpressionsData::EXPRESSION_PUFF_LEFT] = std::wstring(L"Puff Left Cheek");
	return map;
}

void FaceTrackingRenderer::SetNumberOfLandmarks(int numLandmarks)
{
	m_numLandmarks = numLandmarks;
	if (m_landmarkPoints != NULL)
		delete[] m_landmarkPoints;
	m_landmarkPoints = new PXCFaceData::LandmarkPoint[numLandmarks];
}