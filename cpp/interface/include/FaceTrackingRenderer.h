#pragma once

#include <windows.h>
#include <WindowsX.h>
#include <wchar.h>
#include <string>
#include <assert.h>
#include <map>
#include "resource.h"
#include "pxcfacedata.h"
#include "FaceTrackingFrameRateCalculator.h"
#include "pxcsensemanager.h"

typedef void (*OnFinishedRenderingCallback)();

class PXCImage;

class FaceTrackingRenderer
{
public:
	enum RendererType { R2D, R3D };
	FaceTrackingRenderer(HWND window);
	virtual ~FaceTrackingRenderer();

	void SetNumberOfLandmarks(int numLandmarks);
	void SetOutput(PXCFaceData* output);
	void SetSenseManager(PXCSenseManager* senseManager);
	PXCSenseManager* GetSenseManager();
	virtual void DrawBitmap(PXCCapture::Sample* sample, bool ir) = 0;
	void Render();

protected:
	static const int LANDMARK_ALIGNMENT = -3;
	int m_numLandmarks;
	PXCFaceData* m_currentFrameOutput;
	PXCSenseManager* m_senseManager;
	FaceTrackingFrameRateCalculator m_frameRateCalcuator;
	PXCFaceData::LandmarkPoint* m_landmarkPoints;
	std::map<PXCFaceData::ExpressionsData::FaceExpression, std::wstring> m_expressionMap;

	HWND m_window;
	HBITMAP m_bitmap;

	virtual void DrawGraphics(PXCFaceData* faceOutput) = 0;
	virtual void DrawLandmark(PXCFaceData::Face* trackedFace) = 0;
	void DrawFrameRate();
	void RefreshUserInterface();
	RECT GetResizeRect(RECT rectangle, BITMAP bitmap);
	std::map<PXCFaceData::ExpressionsData::FaceExpression, std::wstring> InitExpressionsMap();
};
