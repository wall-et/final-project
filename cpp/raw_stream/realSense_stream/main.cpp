#include <windows.h>
#include <windowsX.h>
#include <wchar.h>
#include "pxcsensemanager.h"
#include "util_render.h"  //SDK provided utility class used for rendering (packaged in libpxcutils.lib)
#include <pxcfaceconfiguration.h>
#include <pxcfacedata.h>
#include <iostream>   // std::cout
#include <string>

using namespace std;

// maximum number of frames to process if user did not close the rendering window
#define MAX_FRAMES 5000
#define MAX_FACES 2

//int wmain(int argc, WCHAR* argv[]) {
int main(int argc, char* argv[]){
	
	// initialize the util render 
	UtilRender *renderColor = new UtilRender(L"COLOR STREAM");
	UtilRender *renderDepth = new UtilRender(L"DEPTH STREAM");
	
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

	// initialize the PXCSenseManager
	if (psm->Init() != PXC_STATUS_NO_ERROR) return 2;

	//mirroring the image to match reality
	psm->QueryCaptureManager()->QueryDevice()->SetMirrorMode(PXCCapture::Device::MirrorMode::MIRROR_MODE_HORIZONTAL);

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