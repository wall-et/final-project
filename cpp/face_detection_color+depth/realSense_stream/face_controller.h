#pragma once
#include <list>
#include "pxcsensemanager.h"
#include <pxcfacedata.h>
#include "util_render.h"
#include <string.h>

#define MAX_FACES 1

struct Point {
	int x, y;
};

class Face {

public:
	Face() {};
	pxcI32 faceId;

	COLORREF color;

	bool landmarksEnabled;
	std::list<Point>  points;

	bool detectionEnabled;
	PXCRectI32 faceRectangle;

	bool expressionEnabled;
	//std::string expressionType;
	//pxcI32 intensity;

	bool angleEnabled;
	//PXCFaceData::PoseEulerAngles angle;

	bool alertEnabled;
	PXCFaceData::AlertData::AlertType label;

	Face(COLORREF, int);
};

class FaceController : public UtilRender
{
public:
	FaceController(pxcCHAR *title = 0) :UtilRender(title) {};

	void SetMaxFaces(int maxFaces);
	void SetLandmark(pxcI32 faceId, PXCFaceData::LandmarkPoint *points, pxcI32 numofPoints);
	boolean TestLandmarkDepth(pxcI32 faceId, PXCFaceData::LandmarkPoint *points, pxcI32 numofPoints);
	void SetDetection(pxcI32 faceId, PXCRectI32 faceRectangle);
	//void SetExpression(pxcI32 _faceId, PXCFaceData::Face *trackedFace, pxcI32 intensity, PXCFaceData::ExpressionsData::FaceExpression expressionType);
	//void SetPose(pxcI32 _faceId, PXCFaceData::Face *trackedFace, PXCFaceData::PoseEulerAngles angle);
	//void SetAlert(pxcUID faceIdOfAlert, PXCFaceData::AlertData::AlertType label);

protected:
	virtual void DrawMore(HDC hdc, double scale_x, double scale_y);
	int maxFaces;
	Face faces[3];

};
