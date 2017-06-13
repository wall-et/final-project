#pragma once

#include "FaceTrackingRenderer.h"


class FaceTrackingRenderer2D : public FaceTrackingRenderer
{
public:
	FaceTrackingRenderer2D(HWND window);
	virtual ~FaceTrackingRenderer2D();

	void DrawBitmap(PXCCapture::Sample* sample, bool ir);
	void CalcDistances();
	void Reset();
	void SetActivateEyeCenterCalculations(bool bValue) {bActivateEyeCenterCalculations = bValue;}


protected:
	static const int sizeData    = 100;
	static const int sizeSample  =  10;

private:
	void DrawDistances();
	void DrawGraphics(PXCFaceData* faceOutput);
	void DrawLandmark(PXCFaceData::Face* trackedFace);
	void DrawLocation(PXCFaceData::Face* trackedFace);
	void DrawPoseAndPulse(PXCFaceData::Face* trackedFace, const int faceId);
	void DrawExpressions(PXCFaceData::Face* trackedFace, const int faceId);		
	void DrawRecognition(PXCFaceData::Face* trackedFace, const int faceId);

	template <int dim> double CalculateDistance(PXCPoint3DF32 p1, PXCPoint3DF32 p2);

	double currHeadWidthAvg;
	double currNoseBridgeAvg;
	double currEyesCenterAvg;
	double currEyesCenterSqrAvg;

	double arrEyesCenter[sizeData];
	double arrEyesCenterAveSample   [sizeSample];
	double arrEyesCenterSqrAveSample[sizeSample];

	double headWidthAvg;
	double noseBridgeAvg;
	double eyesCenterAvg;
	double eyesCenterSqrAvg;
	double eyesCenterSTD;

	int frameNum;
	int fn; //sample frame number (this starts from when frameNum == sizeData 


	bool bActivateEyeCenterCalculations;
};

