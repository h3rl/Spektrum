#pragma once
#include "stdafx.h"

#include <Windows.h>
#include <mmsystem.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <time.h>
#include <iostream>

#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  REFTIMES_PER_SEC/1000

#define FFT_SIZE 1024
#define FFT_SIZE_HALF FFT_SIZE/2+1
#define SAMPLE_RATE 16000

class AudioSink
{
private:
	// WASAPI stuff
	HRESULT hr;
	REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
	REFERENCE_TIME hnsActualDuration;
	UINT32 bufferFrameCount;
	UINT32 numFramesAvailable;
	IMMDeviceEnumerator* pDeviceEnumerator = NULL;
	IMMDevice* pDevice = NULL;
	IAudioClient* pAudioClient = NULL;
	IAudioCaptureClient* pCaptureClient = NULL;
	WAVEFORMATEX* pwaveformatex = NULL;
	UINT32 packetLength = 0;

	MMCKINFO chunkRiff = { 0 };
	MMCKINFO chunkData = { 0 };
	DWORD flags = 0;

	// FFTW3 stuff
	fftwf_plan fftPlan = nullptr;

	BYTE* pData = NULL;



	std::thread thread;

	bool initialized = false;
	bool bStopThread = false;

public:

	float fftInput[FFT_SIZE] = {};
	float fftOutput[FFT_SIZE] = {};

	AudioSink();
	~AudioSink();

	void init();
	void initWASAPI();
	void initFFTW3();

	void releaseFFTW3();

	void start();
	void stop();

private:

	void sinkthread();

	void Release();

};
