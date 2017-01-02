#include <windows.h>
#include <windowsX.h>
#include <wchar.h>
#include "pxcsensemanager.h"
#include "util_render.h"  //SDK provided utility class used for rendering (packaged in libpxcutils.lib)
#include <pxcfaceconfiguration.h>
#include <pxcfacedata.h>
#include "face_render.h"
#include <iostream>   // std::cout
#include <string>
#include "face_controller.h"
#include "pxcfacemodule.h"

using namespace std;

// maximum number of frames to process if user did not close the rendering window
#define MAX_FRAMES 5000
#define MAX_FACES 2


//int wmain(int argc, WCHAR* argv[]) {
int main(int argc, char* argv[]){
	
	pxcStatus sts;

	// initialize the util render 
	//UtilRender *renderColor = new UtilRender(L"COLOR STREAM");
	UtilRender *renderDepth = new UtilRender(L"DEPTH STREAM");

	FaceController *controller = new FaceController(L"FaceDetection");
	controller->SetMaxFaces(MAX_FACES);
	
	// create the PXCSenseManager
	PXCSenseManager *psm = 0;
	psm = PXCSenseManager::CreateInstance();
	if (!psm) {
		wprintf_s(L"Unable to create the PXCSenseManager\n");
		return 1;
	}

	// select the color stream of size 640x480 and depth stream of size 320x240
	psm->EnableStream(PXCCapture::STREAM_TYPE_COLOR, 640, 480);
	psm->EnableStream(PXCCapture::STREAM_TYPE_DEPTH, 640, 480);

	//face module initializing & configuration
	sts = psm->EnableFace(NULL);
	if (sts < PXC_STATUS_NO_ERROR) {
		wprintf_s(L"Unable to enable Face Analysis\n");
		return 2;
	}
	PXCFaceModule* faceAnalyzer;
	faceAnalyzer = psm->QueryFace();
	if (!psm) {
		wprintf_s(L"Unable to retrieve face results\n");
		return 3;
	}

	// initialize the PXCSenseManager
	if (psm->Init() != PXC_STATUS_NO_ERROR) return 2;

	//mirroring the image to match reality
	psm->QueryCaptureManager()->QueryDevice()->SetMirrorMode(PXCCapture::Device::MirrorMode::MIRROR_MODE_HORIZONTAL);

	PXCFaceData* outputData = faceAnalyzer->CreateOutput();
	PXCFaceConfiguration* config = faceAnalyzer->CreateActiveConfiguration();
	
	config->SetTrackingMode(PXCFaceConfiguration::TrackingModeType::FACE_MODE_COLOR_PLUS_DEPTH);
	config->EnableAllAlerts();
	config->landmarks.isEnabled = true;
	config->landmarks.maxTrackedFaces = MAX_FACES;
	config->detection.isEnabled = true;
	config->detection.maxTrackedFaces = MAX_FACES;
	config->pose.isEnabled = true;
	config->pose.maxTrackedFaces = MAX_FACES;
	//config->QueryExpressions()->Enable();
	//config->QueryExpressions()->EnableAllExpressions();
	//config->QueryExpressions()->properties.maxTrackedFaces = MAX_FACES;
	config->ApplyChanges();

	// stream data
	int fnum = 0; //frame counter
	PXCImage *image = NULL, *depth;
	while (psm->AcquireFrame(true) >= PXC_STATUS_NO_ERROR) {

		// increment frame counter since a frame is acquired
		fnum++;

		//update the output data to the latest availible
		outputData->Update();

		/* Detection Structs */
		PXCFaceData::DetectionData *detectionData;
		PXCRectI32 rectangle;
		pxcF32 faceAvgDepth;

		/* Landmark Structs */
		PXCFaceData::LandmarksData* landmarkData;
		PXCFaceData::LandmarkPoint* landmarkPoints;
		pxcI32 numPoints;

		// get number of detected faces
		pxcU16 numOfFaces = outputData->QueryNumberOfDetectedFaces();

		//loop through all detected faces
		for (pxcU16 i = 0; i < numOfFaces; i++)
		{
			// get face data by time of appearence
			PXCFaceData::Face *trackedFace = outputData->QueryFaceByIndex(i);
			if (trackedFace != NULL)
			{

				/* Query Detection Data */
				detectionData = trackedFace->QueryDetection();
				if (detectionData != NULL)
				{
					/* Get rectangle of the detected face and render */
					if (detectionData->QueryBoundingRect(&rectangle))
						controller->SetDetection(i, rectangle);

					//if (detectionData->QueryFaceAverageDepth(&faceAvgDepth))
						//wprintf_s(L"found depth data %d /n",faceAvgDepth);
						//need to check this data with landmark data
				}

				/* Query Landmark Data */
				landmarkData = trackedFace->QueryLandmarks();
				if (landmarkData != NULL)
				{
					
					boolean realDepth = false;

					/* Get number of points from Landmark data*/
					numPoints = landmarkData->QueryNumPoints();
					
					//wprintf_s(L"found landmark data %d/n",numPoints);
					/* Create an Array with the number of points */
					landmarkPoints = new PXCFaceData::LandmarkPoint[numPoints];

					/* Query Points from Landmark Data and render */
					if (landmarkData->QueryPoints(landmarkPoints)) {
						//realDepth = controller->TestLandmarkDepth(i, landmarkPoints, numPoints);
						//if (true) {
							controller->SetLandmark(i, landmarkPoints, numPoints);
						//}
					}
					else {
						wprintf_s(L"stuck here from some reason");
					}
				}
				else {
					wprintf_s(L"Unable to retrieve landmarks data\n");
				}


			}

		}

		// retrieve all available image samples
		PXCCapture::Sample *sample = psm->QuerySample();

		// get the color data
		image = sample->color;
		depth = sample->depth;

		// render the frame
		if (!controller->RenderFrame(image)) break;
		if (!renderDepth->RenderFrame(depth)) break;

		// iterate through Alerts
		PXCFaceData::AlertData alertData;
		for (int i = 0; i <outputData->QueryFiredAlertsNumber(); i++)
		{
			if (outputData->QueryFiredAlertData(i, &alertData) == PXC_STATUS_NO_ERROR)
			{
				// Display last alert - see AlertData::Label for all available alerts
				switch (alertData.label)
				{
				case PXCFaceData::AlertData::ALERT_NEW_FACE_DETECTED:
				{
					wprintf_s(L"Last Alert: Face %d Detected\n", alertData.faceId);
					break;
				}
				case PXCFaceData::AlertData::ALERT_FACE_LOST:
				{
					wprintf_s(L"Last Alert: Face %d Lost\n", alertData.faceId);
					break;
				}
				}

			}
		}

		// release or unlock the current frame to fetch the next frame
		psm->ReleaseFrame();
	}

	//controller->Release();
	config->Release();

/*
	PXCImage *colorIm, *depthIm;
	for (int i = 0; i<MAX_FRAMES; i++) {

		// This function blocks until all streams are ready (depth and color)
		// if false streams will be unaligned
		if (psm->AcquireFrame(true)<PXC_STATUS_NO_ERROR) break;

		// retrieve all available image samples
		PXCCapture::Sample *sample = psm->QuerySample();

		// retrieve the image or frame by type from the sample
		colorIm = sample->color;
		depthIm = sample->depth;

		// render the frame
		if (!renderColor->RenderFrame(colorIm)) break;
		if (!renderDepth->RenderFrame(depthIm)) break;

		// release or unlock the current frame to fetch the next frame
		psm->ReleaseFrame();
	}
*/

	// delete the UtilRender instance
	//renderColor->Release();
	//renderDepth->Release();


	// close the last opened streams and release any session and processing module instances
	psm->Close();
	psm->Release();

	return 0;

}

/*
boolean PrintConnectedDevices(){
	//PXCMSession session = PXCMSession.CreateInstance();
	//PXCMSession.ImplDesc desc = new PXCMSession.ImplDesc();
	//PXCMSession.ImplDesc outDesc = new PXCMSession.ImplDesc();
	//desc.group = EnumSet.of(PXCMSession.ImplGroup.IMPL_GROUP_SENSOR);
	//desc.subgroup = EnumSet.of(PXCMSession.ImplSubgroup.IMPL_SUBGROUP_VIDEO_CAPTURE);

	PXCSession *session = PXCSession::CreateInstance();
	PXCSession::ImplDesc desc;
	PXCSession::ImplDesc outDesc;
	//desc.group = 
	if (session == NULL) {
		wprintf_s(L"Session not created by PXCSenseManager\n");
		return 2;
	}
	// query the session version
	PXCSession::ImplVersion ver;
	ver = session->QueryVersion();
	// print version to console
	wprintf_s(L" Hello Intel RSSDK Version %d.%d \n", ver.major, ver.minor);
	// enumerate all available modules that are automatically loaded with the RSSDK
	for (int i = 0;; i++) {
		PXCSession::ImplDesc desc;
		if (session->QueryImpl(0, i, &desc) < PXC_STATUS_NO_ERROR) break;
		// Print the module friendly name and iuid (interface unique ID)
		wprintf_s(L"Module[%d]: %s\n", i, desc.friendlyName);
		wprintf_s(L" iuid=%x\n", desc.iuid);

		PXCCapture *capture;
		if (session->CreateImpl())
			continue;
	}

}
*/