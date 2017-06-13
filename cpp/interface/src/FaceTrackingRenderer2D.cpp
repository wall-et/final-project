#include "FaceTrackingRenderer2D.h"
#include "FaceTrackingUtilities.h"
#include "pxccapture.h"
#include <string.h>

FaceTrackingRenderer2D::~FaceTrackingRenderer2D()
{
}

FaceTrackingRenderer2D::FaceTrackingRenderer2D(HWND window) : FaceTrackingRenderer(window) , bActivateEyeCenterCalculations(false)
{
	Reset();
}

void FaceTrackingRenderer2D::DrawGraphics(PXCFaceData* faceOutput)
{
	assert(faceOutput != NULL);
	if (!m_bitmap) return;

	const int numFaces = faceOutput->QueryNumberOfDetectedFaces();
	for (int i = 0; i < numFaces; ++i) 
	{
		PXCFaceData::Face* trackedFace = faceOutput->QueryFaceByIndex(i);		
		assert(trackedFace != NULL);
		if (FaceTrackingUtilities::IsModuleSelected(m_window, IDC_LOCATION) && trackedFace->QueryDetection() != NULL)
			DrawLocation(trackedFace);
		if (FaceTrackingUtilities::IsModuleSelected(m_window, IDC_LANDMARK) && trackedFace->QueryLandmarks() != NULL) 
			DrawLandmark(trackedFace);
		if (FaceTrackingUtilities::IsModuleSelected(m_window, IDC_POSE) || FaceTrackingUtilities::IsModuleSelected(m_window, IDC_PULSE))
			DrawPoseAndPulse(trackedFace, i);
		if (FaceTrackingUtilities::IsModuleSelected(m_window, IDC_EXPRESSIONS) && trackedFace->QueryExpressions() != NULL)
			DrawExpressions(trackedFace, i);
		if (FaceTrackingUtilities::IsModuleSelected(m_window, IDC_RECOGNITION) && trackedFace->QueryRecognition() != NULL)
			DrawRecognition(trackedFace, i);
	}
}

void FaceTrackingRenderer2D::DrawBitmap(PXCCapture::Sample* sample, bool ir)
{
	if (m_bitmap) 
	{
		DeleteObject(m_bitmap);
		m_bitmap = 0;
	}

	PXCImage* image = sample->color;
	if (ir)
		image = sample->ir;

	PXCImage::ImageInfo info = image->QueryInfo();
	PXCImage::ImageData data;
	if (image->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_RGB32, &data) >= PXC_STATUS_NO_ERROR)
	{
		HWND hwndPanel = GetDlgItem(m_window, IDC_PANEL);
		HDC dc = GetDC(hwndPanel);
		BITMAPINFO binfo;
		memset(&binfo, 0, sizeof(binfo));
		binfo.bmiHeader.biWidth = data.pitches[0]/4;
		binfo.bmiHeader.biHeight = - (int)info.height;
		binfo.bmiHeader.biBitCount = 32;
		binfo.bmiHeader.biPlanes = 1;
		binfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		binfo.bmiHeader.biCompression = BI_RGB;
		Sleep(1);
		m_bitmap = CreateDIBitmap(dc, &binfo.bmiHeader, CBM_INIT, data.planes[0], &binfo, DIB_RGB_COLORS);

		ReleaseDC(hwndPanel, dc);
		image->ReleaseAccess(&data);
	}

	DrawDistances();
}

void FaceTrackingRenderer2D::DrawDistances()
{
	if(headWidthAvg == 0)
		return;

	HWND panelWindow = GetDlgItem(m_window, IDC_PANEL);
	HDC dc1 = GetDC(panelWindow);
	HDC dc2 = CreateCompatibleDC(dc1);
	if (!dc2) 
	{
		ReleaseDC(panelWindow, dc1);
		return;
	}

	SelectObject(dc2, m_bitmap);
	BITMAP bitmap; 
	GetObject(m_bitmap, sizeof(bitmap), &bitmap);
	HPEN cyan = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));

	if (!cyan)
	{
		DeleteDC(dc2);
		ReleaseDC(panelWindow, dc1);
		return;
	}

	SelectObject(dc2, cyan);

	WCHAR tempLine[64];
	int yPosition = FaceTrackingUtilities::TextHeight;
	const int xPosition = FaceTrackingUtilities::TextHeight;

	swprintf_s<sizeof(tempLine) / sizeof(WCHAR) > (tempLine, L"Head Width : %.0f mm", headWidthAvg*1000);
	TextOut(dc2, xPosition, yPosition, tempLine, std::char_traits<wchar_t>::length(tempLine));

	swprintf_s<sizeof(tempLine) / sizeof(WCHAR) > (tempLine, L"Nose Bridge : %.0f mm", noseBridgeAvg*1000);
	TextOut(dc2, xPosition, yPosition + 50, tempLine, std::char_traits<wchar_t>::length(tempLine));

	swprintf_s<sizeof(tempLine) / sizeof(WCHAR) > (tempLine, L"Eyes Center : %.1f mm", eyesCenterAvg*1000);
	TextOut(dc2, xPosition, yPosition + 100, tempLine, std::char_traits<wchar_t>::length(tempLine));

	swprintf_s<sizeof(tempLine) / sizeof(WCHAR) > (tempLine, L"Eyes Center STD: %.1f mm", eyesCenterSTD*1000);
	TextOut(dc2, xPosition, yPosition + 150, tempLine, std::char_traits<wchar_t>::length(tempLine));


	DeleteDC(dc2);
	ReleaseDC(panelWindow, dc1);
	DeleteObject(cyan);
};

void FaceTrackingRenderer2D::DrawRecognition(PXCFaceData::Face* trackedFace, const int faceId)
{
	PXCFaceData::RecognitionData* recognitionData = trackedFace->QueryRecognition();
	if(recognitionData == NULL)
		return;

	HWND panelWindow = GetDlgItem(m_window, IDC_PANEL);
	HDC dc1 = GetDC(panelWindow);

	if (!dc1)
	{
		return;
	}
	HDC dc2 = CreateCompatibleDC(dc1);
	if(!dc2) 
	{
		ReleaseDC(panelWindow, dc1);
		return;
	}

	SelectObject(dc2, m_bitmap);

	BITMAP bitmap; 
	GetObject(m_bitmap, sizeof(bitmap), &bitmap);

	HFONT hFont = CreateFont(FaceTrackingUtilities::TextHeight, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, L"MONOSPACE");
	SelectObject(dc2, hFont);

	WCHAR line1[64];
	int recognitionID = recognitionData->QueryUserID();
	if(recognitionID != -1)
	{
		swprintf_s<sizeof(line1) / sizeof(pxcCHAR)>(line1, L"Registered ID: %d",recognitionID);
	}
	else
	{
		swprintf_s<sizeof(line1) / sizeof(pxcCHAR)>(line1, L"Not Registered");
	}
	PXCRectI32 rect;
	memset(&rect, 0, sizeof(rect));
	int yStartingPosition;
	if (trackedFace->QueryDetection())
	{
		SetBkMode(dc2, TRANSPARENT);
		trackedFace->QueryDetection()->QueryBoundingRect(&rect);
		yStartingPosition = rect.y;
	}	
	else
	{		
		const int yBasePosition = bitmap.bmHeight - FaceTrackingUtilities::TextHeight;
		yStartingPosition = yBasePosition - faceId * FaceTrackingUtilities::TextHeight;
		WCHAR userLine[64];
		swprintf_s<sizeof(userLine) / sizeof(pxcCHAR)>(userLine, L" User: %d", faceId);
		wcscat_s(line1, userLine);
	}
	SIZE textSize;
	GetTextExtentPoint32(dc2, line1, (int)std::char_traits<wchar_t>::length(line1), &textSize);
	int x = rect.x + rect.w + 1;
	if(x + textSize.cx > bitmap.bmWidth)
		x = rect.x - 1 - textSize.cx;

	TextOut(dc2, x, yStartingPosition, line1, (int)std::char_traits<wchar_t>::length(line1));

	DeleteDC(dc2);
	ReleaseDC(panelWindow, dc1);
	DeleteObject(hFont);
}

void FaceTrackingRenderer2D::DrawExpressions(PXCFaceData::Face* trackedFace, const int faceId)
{
	PXCFaceData::ExpressionsData* expressionsData = trackedFace->QueryExpressions();
	if (!expressionsData)
		return;

	HWND panelWindow = GetDlgItem(m_window, IDC_PANEL);
	HDC dc1 = GetDC(panelWindow);
	HDC dc2 = CreateCompatibleDC(dc1);
	if (!dc2) 
	{
		ReleaseDC(panelWindow, dc1);
		return;
	}

	SelectObject(dc2, m_bitmap);
	BITMAP bitmap; 
	GetObject(m_bitmap, sizeof(bitmap), &bitmap);

	HPEN cyan = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));

	if (!cyan)
	{
		DeleteDC(dc2);
		ReleaseDC(panelWindow, dc1);
		return;
	}
	SelectObject(dc2, cyan);

	const int maxColumnDisplayedFaces = 5;
	const int widthColumnMargin = 570;
	const int rowMargin = FaceTrackingUtilities::TextHeight;
	const unsigned int yStartingPosition = faceId % maxColumnDisplayedFaces * (int)m_expressionMap.size() * FaceTrackingUtilities::TextHeight;
	const int xStartingPosition = widthColumnMargin * (faceId / maxColumnDisplayedFaces);

	WCHAR tempLine[200];
	int yPosition = yStartingPosition;
	swprintf_s<sizeof(tempLine) / sizeof(pxcCHAR)> (tempLine, L"ID: %d", trackedFace->QueryUserID());
	TextOut(dc2, xStartingPosition, yPosition, tempLine, (int)std::char_traits<wchar_t>::length(tempLine));
	yPosition += rowMargin;

	for (auto expressionIter = m_expressionMap.begin(); expressionIter != m_expressionMap.end(); expressionIter++)
	{
		PXCFaceData::ExpressionsData::FaceExpressionResult expressionResult;
		if (expressionsData->QueryExpression(expressionIter->first, &expressionResult))
		{
			int intensity = expressionResult.intensity;
			std::wstring expressionName = expressionIter->second;
			swprintf_s<sizeof(tempLine) / sizeof(WCHAR)> (tempLine, L"%s = %d", expressionName.c_str(), intensity);
			TextOut(dc2, xStartingPosition, yPosition, tempLine, (int)std::char_traits<wchar_t>::length(tempLine));
			yPosition += rowMargin;
		}
	}

	DeleteObject(cyan);
	DeleteDC(dc2);
	ReleaseDC(panelWindow, dc1);
}

void FaceTrackingRenderer2D::DrawPoseAndPulse(PXCFaceData::Face* trackedFace, const int faceId)
{
	const PXCFaceData::PoseData* poseData = trackedFace->QueryPose();
	pxcBool poseAnglesExist;
	PXCFaceData::PoseEulerAngles angles;

	if (poseData == NULL) 
		poseAnglesExist = 0;
	else
		poseAnglesExist = poseData->QueryPoseAngles(&angles);

	HWND panelWindow = GetDlgItem(m_window, IDC_PANEL);
	HDC dc1 = GetDC(panelWindow);
	HDC dc2 = CreateCompatibleDC(dc1);
	if (!dc2) 
	{
		ReleaseDC(panelWindow, dc1);
		return;
	}

	SelectObject(dc2, m_bitmap);
	BITMAP bitmap; 
	GetObject(m_bitmap, sizeof(bitmap), &bitmap);
	HPEN cyan = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));

	if (!cyan)
	{
		DeleteDC(dc2);
		ReleaseDC(panelWindow, dc1);
		return;
	}

	SelectObject(dc2, cyan);

	const int maxColumnDisplayedFaces = 5;
	const int widthColumnMargin = 570;
	const int rowMargin = FaceTrackingUtilities::TextHeight;
	const int yStartingPosition = faceId % maxColumnDisplayedFaces * 6 * FaceTrackingUtilities::TextHeight; 
	const int xStartingPosition = bitmap.bmWidth - 64 - - widthColumnMargin * (faceId / maxColumnDisplayedFaces);

	WCHAR tempLine[64];
	int yPosition = yStartingPosition;
	swprintf_s<sizeof(tempLine) / sizeof(pxcCHAR)> (tempLine, L"ID: %d", trackedFace->QueryUserID());
	TextOut(dc2, xStartingPosition, yPosition, tempLine, (int)std::char_traits<wchar_t>::length(tempLine));
	if (poseAnglesExist)
	{
		if (poseData->QueryConfidence() > 0)
		{
			SetTextColor(dc2, RGB(0, 0, 0));	
		}
		else
		{
			SetTextColor(dc2, RGB(255, 0, 0));	
		}
		if ((int)angles.yaw > 10 || (int)angles.yaw < -10) {
			wprintf_s(L"fix yout yaw - don't look to the side\n");
			SetTextColor(dc2, RGB(255, 0, 0));
			//confidenceCounter = 0;
		}
		yPosition += rowMargin;
		swprintf_s<sizeof(tempLine) / sizeof(WCHAR) > (tempLine, L"Yaw : %.0f", angles.yaw);
		TextOut(dc2, xStartingPosition, yPosition, tempLine, (int)std::char_traits<wchar_t>::length(tempLine));

		SetTextColor(dc2, RGB(0, 0, 0));
		if ((int)angles.pitch > 15 || (int)angles.pitch < -10) {
			SetTextColor(dc2, RGB(255, 0, 0));
			//wprintf_s(L"fix yout pitch - don't look up or down\n");
			//confidenceCounter = 0;
		}
		yPosition += rowMargin;
		swprintf_s<sizeof(tempLine) / sizeof(WCHAR) > (tempLine, L"Pitch: %.0f", angles.pitch);
		TextOut(dc2, xStartingPosition, yPosition, tempLine, (int)std::char_traits<wchar_t>::length(tempLine));
		
		SetTextColor(dc2, RGB(0, 0, 0));
		if ((int)angles.roll > 10 || (int)angles.roll < -10) {
			SetTextColor(dc2, RGB(255, 0, 0));
			wprintf_s(L"fix yout roll - don't lean to the side\n");
			//confidenceCounter = 0;
		}

		yPosition += rowMargin;
		swprintf_s<sizeof(tempLine) / sizeof(WCHAR) > (tempLine, L"Roll : %.0f ", angles.roll);
		TextOut(dc2, xStartingPosition, yPosition, tempLine, (int)std::char_traits<wchar_t>::length(tempLine));
		
	}	
	else
	{
		SetTextColor(dc2, RGB(255, 0, 0));	
	}

	const PXCFaceData::PulseData* pulse = trackedFace->QueryPulse();
	if (pulse != NULL)
	{	
		pxcF32 hr = pulse->QueryHeartRate();	
		yPosition += rowMargin;	
		swprintf_s<sizeof(tempLine) / sizeof(WCHAR) > (tempLine, L"HR: %f", hr);

		TextOut(dc2, xStartingPosition, yPosition, tempLine, (int)std::char_traits<wchar_t>::length(tempLine));
	}

	DeleteObject(cyan);
	DeleteDC(dc2);
	ReleaseDC(panelWindow, dc1);
}

template <int dim> double FaceTrackingRenderer2D::CalculateDistance(PXCPoint3DF32 p1, PXCPoint3DF32 p2)
{
	if(dim == 2)
		return sqrt(pow(p2.x - p1.x, 2.0f) + pow(p2.y - p1.y, 2.0f) );
	else if (dim == 3)
		return sqrt(pow(p2.x - p1.x, 2.0f) + pow(p2.y - p1.y, 2.0f) + pow(p2.z - p1.z, 2.0f));
	else
		throw("Distance must be calculated in either 2 or 3 dimensions");
}


void FaceTrackingRenderer2D::Reset()
{
	headWidthAvg = 0;
	noseBridgeAvg = 0;
	eyesCenterAvg = 0;
	eyesCenterSqrAvg = 0;
	frameNum = 0;
	fn = 0;
	currHeadWidthAvg = 0;
	currNoseBridgeAvg = 0;
	currEyesCenterAvg = 0;
	currEyesCenterSqrAvg = 0;

	memset(arrEyesCenter, 0, sizeof(arrEyesCenter));
	memset(arrEyesCenterAveSample, 0, sizeof(arrEyesCenterAveSample));
	memset(arrEyesCenterSqrAveSample, 0, sizeof(arrEyesCenterSqrAveSample));
}

void FaceTrackingRenderer2D::DrawLandmark(PXCFaceData::Face* trackedFace)
{
	const PXCFaceData::LandmarksData* landmarkData = trackedFace->QueryLandmarks();
	if (landmarkData == NULL)
		return;

	HWND panelWindow = GetDlgItem(m_window, IDC_PANEL);
	HDC dc1 = GetDC(panelWindow);
	HDC dc2 = CreateCompatibleDC(dc1);

	if (!dc2) 
	{
		ReleaseDC(panelWindow, dc1);
		return;
	}

	HFONT hFont = CreateFont(8, 0, 0, 0, FW_LIGHT, 0, 0, 0, 0, 0, 0, 2, 0, L"MONOSPACE");

	if (!hFont)
	{
		DeleteDC(dc2);
		ReleaseDC(panelWindow, dc1);
		return;
	}


	SetBkMode(dc2, TRANSPARENT);

	SelectObject(dc2, m_bitmap);
	SelectObject(dc2, hFont);

	BITMAP bitmap;
	GetObject(m_bitmap, sizeof(bitmap), &bitmap);

	pxcI32 numPoints = landmarkData->QueryNumPoints();
	if (numPoints != m_numLandmarks)
	{
		DeleteObject(hFont);
		DeleteDC(dc2);
		ReleaseDC(panelWindow, dc1);
		return;
	}

	landmarkData->QueryPoints(m_landmarkPoints);
	for (int i = 0; i < numPoints; ++i) 
	{
		int x = (int)m_landmarkPoints[i].image.x + LANDMARK_ALIGNMENT;
		int y = (int)m_landmarkPoints[i].image.y + LANDMARK_ALIGNMENT;		
		if (m_landmarkPoints[i].confidenceImage)
		{
			SetTextColor(dc2, RGB(255, 255, 255));
			TextOut(dc2, x, y, L"•", 1);
		}
		else
		{
			SetTextColor(dc2, RGB(255, 0, 0));
			TextOut(dc2, x, y, L"x", 1);
		}
	}


	if(bActivateEyeCenterCalculations)
	{
		double headWidth  = CalculateDistance<2>(m_landmarkPoints[53].world, m_landmarkPoints[69].world);
		double noseBridge = CalculateDistance<2>(m_landmarkPoints[10].world, m_landmarkPoints[18].world);
		double eyesCenter = CalculateDistance<2>(m_landmarkPoints[76].world, m_landmarkPoints[77].world);


		arrEyesCenter[frameNum % sizeData] = eyesCenter;

		currHeadWidthAvg     = (currHeadWidthAvg*frameNum + headWidth) / (frameNum + 1);
		currNoseBridgeAvg    = (currNoseBridgeAvg*frameNum + noseBridge) / (frameNum + 1);
	
		if(frameNum >= (sizeData-1) )
		{
			double dummyEyesCenterAvg = 0;
			for(int nI = 0; nI < sizeData; ++nI)
			{
				double dCenter = arrEyesCenter[nI];
				dummyEyesCenterAvg  += (1.0/dCenter);
				//dummyEyesCenterAvg  += dCenter;
			}
			eyesCenter    = double(sizeData)/dummyEyesCenterAvg;
			currEyesCenterAvg    = (currEyesCenterAvg*fn + eyesCenter) / (fn + 1);
			currEyesCenterSqrAvg = (currEyesCenterSqrAvg*fn + eyesCenter*eyesCenter) / (fn + 1);

			//and now for the samples:
			arrEyesCenterAveSample   [fn % sizeSample] = currEyesCenterAvg   ;
			arrEyesCenterSqrAveSample[fn % sizeSample] = currEyesCenterSqrAvg;
			
			fn++;
		}
	
		frameNum++;
	}

	DeleteObject(hFont);
	DeleteDC(dc2);
	ReleaseDC(panelWindow, dc1);
}

void FaceTrackingRenderer2D::DrawLocation(PXCFaceData::Face* trackedFace)
{
	const PXCFaceData::DetectionData* detectionData = trackedFace->QueryDetection();
	if (detectionData == NULL) 
		return;	

	HWND panelWindow = GetDlgItem(m_window, IDC_PANEL);
	HDC dc1 = GetDC(panelWindow);
	HDC dc2 = CreateCompatibleDC(dc1);

	if (!dc2) 
	{
		ReleaseDC(panelWindow, dc1);
		return;
	}

	SelectObject(dc2, m_bitmap);

	BITMAP bitmap;
	GetObject(m_bitmap, sizeof(bitmap), &bitmap);

	HPEN cyan = CreatePen(PS_SOLID, 3, RGB(255 ,255 , 0));

	if (!cyan)
	{
		DeleteDC(dc2);
		ReleaseDC(panelWindow, dc1);
		return;
	}
	SelectObject(dc2, cyan);

	PXCRectI32 rectangle;
	pxcBool hasRect = detectionData->QueryBoundingRect(&rectangle);
	if (!hasRect)
	{
		DeleteObject(cyan);
		DeleteDC(dc2);
		ReleaseDC(panelWindow, dc1);
		return;
	}

	MoveToEx(dc2, rectangle.x, rectangle.y, 0);
	LineTo(dc2, rectangle.x, rectangle.y + rectangle.h);
	LineTo(dc2, rectangle.x + rectangle.w, rectangle.y + rectangle.h);
	LineTo(dc2, rectangle.x + rectangle.w, rectangle.y);
	LineTo(dc2, rectangle.x, rectangle.y);

	WCHAR line[64];
	swprintf_s<sizeof(line)/sizeof(pxcCHAR)>(line,L"%d",trackedFace->QueryUserID());
	TextOut(dc2,rectangle.x, rectangle.y, line, (int)std::char_traits<wchar_t>::length(line));
	DeleteObject(cyan);

	DeleteDC(dc2);
	ReleaseDC(panelWindow, dc1);
}

void FaceTrackingRenderer2D::CalcDistances()
{
	headWidthAvg = 0;
	noseBridgeAvg = 0;
	eyesCenterAvg    = 0.0;
	eyesCenterSqrAvg = 0.0;
	eyesCenterSTD    = 0.0;

	if(bActivateEyeCenterCalculations)
	{
		headWidthAvg = currHeadWidthAvg;
		noseBridgeAvg = currNoseBridgeAvg;

		if(fn > sizeSample)
		{
			for(int nSample = 0; nSample < sizeSample; ++nSample)
			{
				eyesCenterAvg     += arrEyesCenterAveSample   [nSample];
				eyesCenterSqrAvg  += arrEyesCenterSqrAveSample[nSample];
			}

			eyesCenterAvg /= double(sizeSample);
			eyesCenterSqrAvg /= double(sizeSample);
			eyesCenterSTD = ::sqrt(eyesCenterSqrAvg - eyesCenterAvg*eyesCenterAvg);
		}
	}
}