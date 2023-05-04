#pragma once
#include "stdafx.h"

// Size of the FFT buffer (must be a power of 2)
#define FFT_SIZE 1024*4
#define FFT_SIZE_HALF (FFT_SIZE/2 + 1)
#define SAMPLE_RATE 16000

class SoundHandler
{
public:
	static struct AudioData
	{
		float fftInput[FFT_SIZE] = {};
		float fftOutput[FFT_SIZE] = {};

		fftwf_plan fftPlan = nullptr;

	} data;

	PaStream* audiostream = nullptr;
public:
	SoundHandler();
	~SoundHandler();

	static int audioCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData);


	bool init();

	bool initPortAudio();
	bool initFFTW3();

	void stop();

private:

	bool initialized = false;
};
