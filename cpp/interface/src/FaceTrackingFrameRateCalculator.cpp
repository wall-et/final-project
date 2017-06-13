#include "FaceTrackingFrameRateCalculator.h"

FaceTrackingFrameRateCalculator::FaceTrackingFrameRateCalculator() : frameRate(0), currentlyCalclatedFrameRate(0), currentTime(), isFrameRateReady(false)
{
	QueryPerformanceCounter(&previousTime);
	QueryPerformanceFrequency(&frequency);
}

bool FaceTrackingFrameRateCalculator::IsFrameRateReady()
{
	return isFrameRateReady;
}

int FaceTrackingFrameRateCalculator::GetFrameRate()
{
	isFrameRateReady = false;
	return frameRate;
}

void FaceTrackingFrameRateCalculator::Tick()
{
	QueryPerformanceCounter(&currentTime);
	++currentlyCalclatedFrameRate;
	if (currentTime.QuadPart - previousTime.QuadPart > frequency.QuadPart)
	{
		isFrameRateReady = true;
		previousTime = currentTime;
		frameRate = currentlyCalclatedFrameRate;
		currentlyCalclatedFrameRate = 0;
	}
}

