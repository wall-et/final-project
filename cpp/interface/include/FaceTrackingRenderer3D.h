#pragma once

#include <memory>
#include "FaceTrackingRenderer.h"
#include "service/pxcsessionservice.h"

class FaceTrackingRenderer3D : public FaceTrackingRenderer
{
public:
	FaceTrackingRenderer3D(HWND window, PXCSession* session);
	virtual ~FaceTrackingRenderer3D();

	void DrawBitmap(PXCCapture::Sample* sample, bool ir);

private:
	void DrawGraphics(PXCFaceData* faceOutput);
	void DrawLandmark(PXCFaceData::Face* trackedFace);
	bool ProjectVertex(const PXCPoint3DF32 &v, double &x, double &y, int radius = 0);
	void CalcCenterOfMass(PXCFaceData::LandmarkPoint &centerOfMass,PXCFaceData::LandmarkPoint* points);
	void DrawPose(PXCFaceData::Face* trackedFace);

	PXCSession* m_session;
	PXCImage::ImageInfo m_outputImageInfo;
	PXCImage* m_outputImage;
	PXCImage::ImageData m_outputImageData;
};