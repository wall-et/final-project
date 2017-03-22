#include <windows.h>
#include <windowsX.h>
#include "face_controller.h"
#include <pxcfacedata.h>
#include <wchar.h>
#include <iostream>   // std::cout
#include <string>     // std::string, std::to_string
#include <thread>
#include <vector>

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
	confidenceFlag = true;
	if (!faces[_faceId].landmarksEnabled)
	{
		faces[_faceId].landmarksEnabled = true;
		for (int i = 0; i<_numofPoints; i++)
		{
			//if (i == 14 || i == 22 || i == 29) {
				//wprintf_s(L"found landmark %d (%6.4lf,%6.4lf)\n with depth %6.4lf\n",i, _points[i].image.x, _points[i].image.y, _points[i].world.z);
				//cout << ("found landmark %d (%d,%d)\n with depth %6.4lf\n", i, _points[i].image.x, _points[i].image.y, _points[i].world.z) << endl;
				//cout << "my first cpp program " << endl;
			//}
			//wprintf_s(L"found landmark (%d,%d,%d) with depth\n", _points[i].image.x, _points[i].image.y, _points[i].world.z);
			//sum = sum + _points[i].world.z;
			Point _p = { (int)_points[i].image.x,(int)_points[i].image.y };
			faces[_faceId].points.push_back(_p);
			if ((int)_points[i].confidenceWorld != 100 && confidenceFlag) {
				confidenceFlag = false;
			}
		}
		//float x = ((_points[29].world.x- _points[30].world.x)*(_points[29].world.x - _points[30].world.x));
		//float y = ((_points[29].world.y - _points[30].world.y)*(_points[29].world.y - _points[30].world.y));
		//float z = ((_points[29].world.z - _points[30].world.z)*(_points[29].world.z - _points[30].world.z));
		//float root = sqrtf((x + y + z));
		//wprintf_s(L"found landmark  avg %10.8lf\n" ,root);
		
		//wprintf_s(L"found landmark 29 confidenceWorld %d\n", _points[29].confidenceWorld);
		//wprintf_s(L"found landmark 30 confidenceWorld %d\n", _points[30].confidenceImage);
	}
	

}
boolean FaceController::calculateDistances() {
	return true;
}

boolean FaceController::TestLandmarkDepth(pxcI32 _faceId, PXCFaceData::LandmarkPoint *_points, pxcI32 _numofPoints)
{
	if (!faces[_faceId].landmarksEnabled)
	{
		//faces[_faceId].landmarksEnabled = true;
		for (int i = 0; i<_numofPoints; i++)
		{
			//Point _p = { (int)_points[i].image.x,(int)_points[i].image.y };
			//faces[_faceId].points.push_back(_p);
			//need to check avarage distance between points or see that nose and eye are far etc.
		}
	}
	return true;
}
void FaceController::calculateAllDistances(PXCFaceData::LandmarkPoint *_points) {
	float distances[78][78];//[numofpoints];
	//std::vector<std::thread> threads;
	std::thread threads[78];
	for (int i = 0; i<78; i++)
	{
		//std::vector<std::thread> threads;
		for (int i = 0; i < 78; ++i) {
			//threads[i]=(controller->calculatePointDistances, landmarkPoints, distances[i], landmarkPoints[i].world.x, landmarkPoints[i].world.y, landmarkPoints[i].world.z);
			//threads.push_back(std::thread(calculatePointDistances, _points, distances[i], _points[i].world.x, _points[i].world.y, _points[i].world.z));
			//threads[i] = std::thread(&(calculatePointDistances, _points, distances[i], _points[i].world.x, _points[i].world.y, _points[i].world.z));
			calculatePointDistances(_points, distances[i], _points[i].world.x, _points[i].world.y, _points[i].world.z);
		}
		//for (auto& th : threads) th.join();
		//for (int i = 0; i<5; ++i)
		//	threads[i].join();
	}

	/*
	wprintf_s(L"found landmark 0 - %6.4lf\n", distances[0][0]);
	wprintf_s(L"found landmark 20 - %6.4lf\n", distances[10][20]);
	wprintf_s(L"found landmark 30 - %6.4lf\n", distances[20][30]);
	wprintf_s(L"found landmark 40 - %6.4lf\n", distances[30][40]);
	wprintf_s(L"found landmark 50 - %6.4lf\n", distances[40][50]);
	wprintf_s(L"found landmark 60 - %6.4lf\n", distances[50][60]);
	*/

}
void FaceController::calculatePointDistances(PXCFaceData::LandmarkPoint *_points, float * _distances,float worldx, float worldy, float worldz) {
	//wprintf_s(L"called function with dist x %6.4lf\n", worldx);
	for (int i = 0; i<78; i++)
	{
		float x = ((_points[i].world.x - worldx)*(_points[i].world.x - worldx));
		float y = ((_points[i].world.y - worldy)*(_points[i].world.y - worldy));
		float z = ((_points[i].world.z - worldz)*(_points[i].world.z - worldz));
		float root = sqrtf((x + y + z));
		*(_distances+i) = root;
	}
	
	//wprintf_s(L"found landmark  avg %10.8lf\n", root);


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