#pragma once
#include "stdafx.h"

#include <Windows.h>
#include <mmsystem.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <time.h>
#include <iostream>
#include <thread>

#include <boost/circular_buffer.hpp>

#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  REFTIMES_PER_SEC/1000

// 10 -> 1024
// 11 -> 2048
// 12 -> 4096
// 13 -> 8192
#define FFT_SIZE (int)(1 << 12)
#define FFT_SIZE_HALF FFT_SIZE/2

typedef void (*callback_ProcessPacketBuffer)(boost::circular_buffer<float>& dest, float* pData, const UINT& size);

class AudioSink
{
private:
	// WASAPI stuff
	HRESULT hr;
	REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
	REFERENCE_TIME hnsActualDuration;
	UINT32 bufferFrameCount;
	UINT32 numFramesAvailable;
	IMMDeviceEnumerator* pDeviceEnumerator = nullptr;
	IMMDevice* pDevice = nullptr;
	IAudioClient* pAudioClient = nullptr;
	IAudioCaptureClient* pCaptureClient = nullptr;
	WAVEFORMATEX* pwaveformatex = nullptr;
	UINT32 packetLength = 0;

	MMCKINFO chunkRiff = { 0 };
	MMCKINFO chunkData = { 0 };
	DWORD flags = 0;

	// FFTW3 stuff
	BYTE* pData = nullptr;

	std::thread thread;

	bool m_bInitialized = false;
	bool m_bStopThread = false;

	boost::circular_buffer<float> m_rawmonodata{ FFT_SIZE };

	float fftInput[FFT_SIZE] = {};
	fftwf_plan fftPlan = nullptr;
	fftwf_complex fftOutputComplex[FFT_SIZE_HALF] = {};
	float fftOutput[FFT_SIZE_HALF] = {};

public:
	float Output[FFT_SIZE_HALF] = {};

	callback_ProcessPacketBuffer CallbackProcessPacketBuffer = nullptr;

	AudioSink();
	~AudioSink();

	bool init();
	
	void setPbCallback(callback_ProcessPacketBuffer callback);

	void update(const sf::Time& dtTime);
	float getFreqPerSample();

private:

	void release();

	void applyWindowing();
};

extern AudioSink g_audiosink;