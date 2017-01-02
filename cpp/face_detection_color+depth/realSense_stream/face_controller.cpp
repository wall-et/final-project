#include <windows.h>
#include <windowsX.h>
#include "face_controller.h"
#include <pxcfacedata.h>
#include <wchar.h>
#include <iostream>   // std::cout
#include <string>     // std::string, std::to_string

using namespace std;

#define COLOR_RED        RGB(255,0,0)
#define COLOR_GREEN      RGB(0,255,0)
#define COLOR_BLUE       RGB(0,0,255)
#define COLOR_YELLOW     RGB(255,255,0)
#define COLOR_WHITE		 RGB(255,255,255)

void FaceController::SetMaxFaces(int _maxFaces)
{
	maxFaces = _maxFaces;

	for (int i = 0; i<maxFaces; i++)
	{
		faces[i].faceId = i;
		faces[i].alertEnabled = false;
		faces[i].angleEnabled = false;//disabled for now
		faces[i].detectionEnabled = false;
		faces[i].expressionEnabled = false;//disabled for now
		faces[i].landmarksEnabled = false;

		switch (i)
		{
		case 0: faces[i].color = COLOR_GREEN; break;
		case 1: faces[i].color = COLOR_YELLOW; break;
		case 2: faces[i].color = COLOR_RED; break;
		default:
			break;
		}
	}
}

void FaceController::SetLandmark(pxcI32 _faceId, PXCFaceData::LandmarkPoint *_points, pxcI32 _numofPoints)
{
	int sum = 0;
	if (!faces[_faceId].landmarksEnabled)
	{
		faces[_faceId].landmarksEnabled = true;
		for (int i = 0; i<_numofPoints; i++)
		{
			if (i == 14 || i == 22 || i == 29) {
				wprintf_s(L"found landmark %d (%d,%d)\n with depth %d\n",i, _points[i].image.x, _points[i].image.y, _points[i].world.z);
			}
			//wprintf_s(L"found landmark (%d,%d,%d) with depth\n", _points[i].image.x, _points[i].image.y, _points[i].world.z);
			sum = sum + _points[i].world.z;
			Point _p = { (int)_points[i].image.x,(int)_points[i].image.y };
			faces[_faceId].points.push_back(_p);
		}
		//wprintf_s(L"found landmark with sum %d\n and avg %d\n", sum, (int)(sum / _numofPoints));
	}
	

}

boolean FaceController::TestLandmarkDepth(pxcI32 _faceId, PXCFaceData::LandmarkPoint *_points, pxcI32 _numofPoints)
{
	if (!faces[_faceId].landmarksEnabled)
	{
		faces[_faceId].landmarksEnabled = true;
		for (int i = 0; i<_numofPoints; i++)
		{
			//Point _p = { (int)_points[i].image.x,(int)_points[i].image.y };
			//faces[_faceId].points.push_back(_p);
			//need to check avarage distance between points or see that nose and eye are far etc.
		}
	}
	return true;
}

void FaceController::SetDetection(pxcI32 _faceId, PXCRectI32 _faceRectangle)
{
	if (!faces[_faceId].detectionEnabled)
	{
		faces[_faceId].detectionEnabled = true;
		faces[_faceId].faceRectangle = _faceRectangle;
	}
}

void FaceController::DrawMore(HDC hdc, double sx, double sy)
{

	/* Draw Landmarks */
	int radius = 1;

	for (int i = 0; i<maxFaces; i++)
	{
		if (faces[i].landmarksEnabled)
		{
			HPEN lColor = CreatePen(PS_SOLID, 2, faces[i].color);
			SelectObject(hdc, lColor);
			for (std::list<Point>::iterator itrl = faces[i].points.begin(); itrl != faces[i].points.end(); itrl++)
			{
				int x = (int)(itrl->x*sx);
				int y = (int)(itrl->y*sy);
				int rx = (int)(radius*sx);
				int ry = (int)(radius*sy);
				MoveToEx(hdc, x, y, NULL);
				Arc(hdc, x - rx, y - ry, x + rx, y + ry, x + rx, y + ry, x + rx, y + ry);

			}
			faces[i].landmarksEnabled = false;
			DeleteObject(lColor);
		}
		faces[i].points.clear();


		if (faces[i].detectionEnabled)
		{
			HPEN dColor = CreatePen(PS_SOLID, 1, faces[i].color);
			SelectObject(hdc, dColor);
			MoveToEx(hdc, faces[i].faceRectangle.x*sx, faces[i].faceRectangle.y*sy, 0);
			LineTo(hdc, faces[i].faceRectangle.x*sx, faces[i].faceRectangle.y*sy + faces[i].faceRectangle.h*sy);
			LineTo(hdc, faces[i].faceRectangle.x*sx + faces[i].faceRectangle.w*sx, faces[i].faceRectangle.y*sy + faces[i].faceRectangle.h*sy);
			LineTo(hdc, faces[i].faceRectangle.x*sx + faces[i].faceRectangle.w*sx, faces[i].faceRectangle.y*sy);
			LineTo(hdc, faces[i].faceRectangle.x*sx, faces[i].faceRectangle.y*sy);
			DeleteObject(dColor);
			faces[i].detectionEnabled = false;
		}


	}
}