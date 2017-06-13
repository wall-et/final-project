#pragma once

#include "windows.h"

class FaceTrackingFrameRateCalculator
{
private:
	LARGE_INTEGER frequency;
	LARGE_INTEGER previousTime;
	LARGE_INTEGER currentTime;
	int currentlyCalclatedFrameRate;
	int frameRate;
	bool isFrameRateReady;

public:
	FaceTrackingFrameRateCalculator();
	bool IsFrameRateReady();
	int GetFrameRate();
	void Tick();
};
