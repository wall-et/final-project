#include "FaceTrackingRenderer3D.h"
#include "FaceTrackingUtilities.h"
#include "pxcprojection.h"

inline int my_round(double x) {
	return int(floor(x+0.5)+0.01);
}

void DrawPoint(HDC dc, COLORREF color, int x, int y, int rad)
{
	HBRUSH hBrush = CreateSolidBrush(color);
	if (hBrush == NULL)
	{
		return;
	}
	HBRUSH hOld = (HBRUSH)SelectObject(dc, hBrush);
	Ellipse(dc, x-rad, y-rad, x+rad, y+rad);
	SelectObject(dc, hOld);
	DeleteObject(hBrush);
}

FaceTrackingRenderer3D::~FaceTrackingRenderer3D()
{
}

FaceTrackingRenderer3D::FaceTrackingRenderer3D(HWND window, PXCSession* session) : FaceTrackingRenderer(window), m_session(session)
{
}

bool FaceTrackingRenderer3D::ProjectVertex(const PXCPoint3DF32 &v, double &x, double &y, int radius)
{
	x = m_outputImageInfo.width  * (0.5 + 0.001 * v.x);
	y = m_outputImageInfo.height * (0.5 - 0.001 * v.y);

	return ((radius <= my_round(x)) && (my_round(x) < m_outputImageInfo.width-radius) && (radius <= my_round(y)) && (my_round(y) < m_outputImageInfo.height-radius));
}

void FaceTrackingRenderer3D::CalcCenterOfMass(PXCFaceData::LandmarkPoint &centerOfMass,PXCFaceData::LandmarkPoint* points)
{
	centerOfMass.world.x = 0.0;
	centerOfMass.world.y = 0.0;
	centerOfMass.world.z = 0.0;

	int numStartPointsTodevied = 62;
	for (int j=0; j<78; j++)
	{
		if (j < 53 || j >69 ||j  == 61) 
		{			
			if (points[j].confidenceWorld > 0)
			{
				centerOfMass.world.x += points[j].world.x;
				centerOfMass.world.y += points[j].world.y;
				centerOfMass.world.z += points[j].world.z;
			}
			else
			{
				numStartPointsTodevied--;
			}
		}
	}

	centerOfMass.world.x /= numStartPointsTodevied;
	centerOfMass.world.y /= numStartPointsTodevied;
	centerOfMass.world.z /= numStartPointsTodevied;
}

void FaceTrackingRenderer3D::DrawGraphics(PXCFaceData* faceOutput)
{
	assert(faceOutput != NULL);
	if (!m_bitmap) return;

	const int numFaces = faceOutput->QueryNumberOfDetectedFaces();
	for (int i = 0; i < numFaces; ++i) 
	{
		PXCFaceData::Face* trackedFace = faceOutput->QueryFaceByIndex(i);		
		assert(trackedFace != NULL);
		if (FaceTrackingUtilities::IsModuleSelected(m_window, IDC_LANDMARK) && trackedFace->QueryLandmarks() != NULL) 
			DrawLandmark(trackedFace);
		if (FaceTrackingUtilities::IsModuleSelected(m_window, IDC_POSE))
			DrawPose(trackedFace);
	}
}

void FaceTrackingRenderer3D::DrawBitmap(PXCCapture::Sample* sample, bool ir)
{
	PXCImage *imageDepth = sample->depth;
	assert(imageDepth);
	PXCImage::ImageInfo imageDepthInfo = imageDepth->QueryInfo();

	m_outputImageInfo.width = 1024;
	m_outputImageInfo.height = 1024;
	m_outputImageInfo.format = PXCImage::PIXEL_FORMAT_RGB32;
	m_outputImageInfo.reserved = 0;

	m_outputImage = m_session->CreateImage(&m_outputImageInfo);
	assert(m_outputImage);

	PXCImage::ImageData imageDepthData;
	if (imageDepth->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_RGB32, &imageDepthData) >= PXC_STATUS_NO_ERROR)
	{
		memset(&m_outputImageData, 0, sizeof(m_outputImageData));
		pxcStatus status = m_outputImage->AcquireAccess(PXCImage::ACCESS_WRITE, PXCImage::PIXEL_FORMAT_RGB32, &m_outputImageData);
		if (status < PXC_STATUS_NO_ERROR) return;

		int stridePixels = m_outputImageData.pitches[0];
		pxcBYTE *pixels = reinterpret_cast<pxcBYTE*> (m_outputImageData.planes[0]);
		memset(pixels, 0, stridePixels * m_outputImageInfo.height);

		// get access to depth data
		PXCPoint3DF32* vertices = new PXCPoint3DF32[imageDepthInfo.width * imageDepthInfo.height];
		
		PXCProjection* projection(m_senseManager->QueryCaptureManager()->QueryDevice()->CreateProjection());
		if (!projection)
		{
			if (vertices) delete[] vertices;
			return;
		}

		projection->QueryVertices(imageDepth, vertices);
		projection->Release();
		int strideVertices = imageDepthInfo.width;

		// render vertices
		int numVertices = 0;
		for (int y = 0; y < imageDepthInfo.height; y++)
		{
			const PXCPoint3DF32 *verticesRow = vertices + y * strideVertices;
			for (int x = 0; x < imageDepthInfo.width; x++)
			{
				const PXCPoint3DF32 &v = verticesRow[x];
				if (v.z <= 0.0f)
				{
					continue;
				}

				double ix = 0, iy = 0;
				if (ProjectVertex(v, ix, iy))
				{
					pxcBYTE *ptr = m_outputImageData.planes[0];
					ptr += my_round(iy) * m_outputImageData.pitches[0];
					ptr += my_round(ix) * 4;
					ptr[0] = pxcBYTE(255.0f * 0.5f);
					ptr[1] = pxcBYTE(255.0f * 0.5f);
					ptr[2] = pxcBYTE(255.0f * 0.5f);
					ptr[3] = pxcBYTE(255.0f);
				}

				numVertices++;
			}
		}
		if (vertices) delete[] vertices;

		if (m_bitmap)
		{
			DeleteObject(m_bitmap);
			m_bitmap = 0;
		}

		HWND hwndPanel = GetDlgItem(m_window, IDC_PANEL);
		HDC dc = GetDC(hwndPanel);
		BITMAPINFO binfo;
		memset(&binfo, 0, sizeof(binfo));
		binfo.bmiHeader.biWidth = m_outputImageData.pitches[0] / 4;
		binfo.bmiHeader.biHeight = -(int)m_outputImageInfo.height;
		binfo.bmiHeader.biBitCount = 32;
		binfo.bmiHeader.biPlanes = 1;
		binfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		binfo.bmiHeader.biCompression = BI_RGB;
		Sleep(1);
		m_bitmap = CreateDIBitmap(dc, &binfo.bmiHeader, CBM_INIT, m_outputImageData.planes[0], &binfo, DIB_RGB_COLORS);

		ReleaseDC(hwndPanel, dc);

		m_outputImage->ReleaseAccess(&m_outputImageData);
		imageDepth->ReleaseAccess(&imageDepthData);
		m_outputImage->Release();
	}
}

void FaceTrackingRenderer3D::DrawLandmark(PXCFaceData::Face* trackedFace)
{
	const PXCFaceData::LandmarksData *landmarkData = trackedFace->QueryLandmarks();

	if (!landmarkData)
		return;

	HWND panelWindow = GetDlgItem(m_window, IDC_PANEL);
	HDC dc1 = GetDC(panelWindow);
	HDC dc2 = CreateCompatibleDC(dc1);

	if (!dc2) 
	{
		ReleaseDC(panelWindow, dc1);
		return;
	}

	HFONT hFont = CreateFont(16, 8, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, L"MONOSPACE");

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

	PXCFaceData::LandmarkPoint* points = new PXCFaceData::LandmarkPoint[numPoints];

	for (int l = 0; l < numPoints; l++) //initialize Array
	{
		points[l].world.x = 0.0;
		points[l].world.y = 0.0;
		points[l].world.z = 0.0;
	}

	landmarkData->QueryPoints(points); //data set for all landmarks in frame

	//convert depth data is to millimeters
	for (int l = 0; l < numPoints; l++)
	{
		points[l].world.x *= 1000.0f;
		points[l].world.y *= 1000.0f;
		points[l].world.z *= 1000.0f;
	}

	for (int j = 0; j < numPoints; j++)
	{
		double ix = 0, iy = 0;

		if(ProjectVertex(points[j].world, ix, iy, 1))
		{
			if (points[j].confidenceWorld > 0)
			{
				DrawPoint(dc2, RGB(255, 255, 0), my_round(ix), my_round(iy), 3);
			}
			else
			{
				DrawPoint(dc2, RGB(255, 0, 0), my_round(ix), my_round(iy), 3);
			}
		}
	}
		
	if (points) delete[] points;

	DeleteObject(hFont);
	DeleteDC(dc2);
	ReleaseDC(panelWindow, dc1);
}

void FaceTrackingRenderer3D::DrawPose(PXCFaceData::Face* trackedFace)
{
	HWND panelWindow = GetDlgItem(m_window, IDC_PANEL);
	HDC dc1 = GetDC(panelWindow);
	HDC dc2 = CreateCompatibleDC(dc1);

	if (!dc2) 
	{
		ReleaseDC(panelWindow, dc1);
		return;
	}

	HFONT hFont = CreateFont(28, 18, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, L"MONOSPACE");

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

	const PXCFaceData::PoseData *poseData = trackedFace->QueryPose();

	if(poseData == NULL)
	{
		DeleteObject(hFont);
		DeleteDC(dc2);
		ReleaseDC(panelWindow, dc1);
		return;
	}
	PXCFaceData::HeadPosition outFaceCenterPoint;
	poseData->QueryHeadPosition(&outFaceCenterPoint);
	if (outFaceCenterPoint.confidence == 0)
	{
		DeleteObject(hFont);
		DeleteDC(dc2);
		ReleaseDC(panelWindow, dc1);
		return;
	}

	double headCenter_x = 0, headCenter_y = 0;	
	if(ProjectVertex(outFaceCenterPoint.headCenter, headCenter_x, headCenter_y, 2))
	{
		if(poseData->QueryConfidence() > 0 && outFaceCenterPoint.confidence > 0)
		{
			DrawPoint(dc2, RGB(0, 0, 255), my_round(headCenter_x), my_round(headCenter_y), 8);
		}
	
		const PXCFaceData::LandmarksData *landmarkData = trackedFace->QueryLandmarks();

		if (!landmarkData)
		{
			DeleteObject(hFont);
			DeleteDC(dc2);
			ReleaseDC(panelWindow, dc1);
			return;
		}

		PXCFaceData::LandmarkPoint* points = new PXCFaceData::LandmarkPoint[landmarkData->QueryNumPoints()];
		landmarkData->QueryPoints(points); //data set for all landmarks in frame

		points[29].world.x *= 1000.0f;
		points[29].world.y *= 1000.0f;
		points[29].world.z *= 1000.0f;

		double noseTip_x = 0, noseTip_y = 0;

		if(ProjectVertex(points[29].world, noseTip_x, noseTip_y, 1))
		{
			PXCPoint3DF32 direction;
			direction.x = (float)(noseTip_x - headCenter_x);
			direction.y = (float)(noseTip_y - headCenter_y);
			
			HPEN lineColor;
			
			if (poseData->QueryConfidence() > 0)
			{
				lineColor = CreatePen(PS_SOLID, 3, RGB(0 ,255 ,255));
			}
			else
			{
				lineColor = CreatePen(PS_SOLID, 3, RGB(255 ,0 , 0));
			}

			if (!lineColor)
			{
				DeleteObject(hFont);
				DeleteDC(dc2);
				ReleaseDC(panelWindow, dc1);
				return;
			}
			SelectObject(dc2, lineColor);

			MoveToEx(dc2, my_round(noseTip_x), my_round(noseTip_y), 0);
			LineTo(dc2, my_round(noseTip_x + 1.2 * direction.x), my_round(noseTip_y + 1.2 * direction.y));

			DeleteObject(lineColor);
		}
	}
	
	DeleteObject(hFont);
	DeleteDC(dc2);
	ReleaseDC(panelWindow, dc1);
}