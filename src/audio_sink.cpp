#include "audio_sink.h"
#include "dft.h"
#include "logging.h"
#include "filter.h"

#include <Windows.h>
#include <mmsystem.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <cassert>
#include <comdef.h>
#include <cstddef>
#include <minwindef.h>
#include <winerror.h>
#include <winnt.h>
#include <boost/circular_buffer.hpp>

#define PRINT_COM_ERROR(hr)                    \
	_com_error ___err(hr);                     \
	LPCTSTR ___errMsg = ___err.ErrorMessage(); \
	log_err("{}", ___errMsg);

#define assert_msg_hr(hr, format, ...)  \
	if (FAILED(hr))                     \
	{                                   \
		log_err(format, ##__VA_ARGS__); \
		PRINT_COM_ERROR(hr);            \
		assert(FAILED(hr));             \
	}

#define REFTIMES_PER_SEC 10000000
#define REFTIMES_PER_MILLISEC REFTIMES_PER_SEC / 1000

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

namespace AudioSink
{

const REFERENCE_TIME hns_requested_duration = REFTIMES_PER_SEC;

// Globals
unsigned int samplerate = 0;

// Locals
bool initialized = false;
// ringbuffer to store audio data
boost::circular_buffer<float> audio_buffer((size_t)MAX_DFT_SIZE);
boost::circular_buffer<float> audio_buffer_50_100((size_t)MAX_DFT_SIZE);

// WASAPI stuff
UINT32 bufferFrameCount;
UINT32 numFramesAvailable;
IMMDeviceEnumerator *pDeviceEnumerator = nullptr;
IMMDevice *pDevice = nullptr;
IAudioClient *pAudioClient = nullptr;
IAudioCaptureClient *pCaptureClient = nullptr;
WAVEFORMATEX *pwaveformatex = nullptr;

void init()
{
	HRESULT hr;

	if (initialized)
	{
		log_err("AudioSink::init was called, but it is already initialized!");
		return;
	}

	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	assert_msg_hr(hr, "CoInitializeEx failed");

	hr = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator,
	                      (void **)&pDeviceEnumerator);
	assert_msg_hr(hr, "CoCreateInstance failed");

	hr = pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
	assert_msg_hr(hr, "pDeviceEnumerator->GetDefaultAudioEndpoint failed");

	hr = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void **)&pAudioClient);
	assert_msg_hr(hr, "pDevice->Activate failed");

	hr = pAudioClient->GetMixFormat(&pwaveformatex);
	assert_msg_hr(hr, "pAudioClient->GetMixFormat failed");

	samplerate = pwaveformatex->nSamplesPerSec;

	log_msg_ln("AudioSink Properties:");
	log_msg_ln("\tSamplerate: {}hz", pwaveformatex->nSamplesPerSec);
	log_msg_ln("\tnChannels: {}", pwaveformatex->nChannels);

	assert_msg(pwaveformatex->nChannels == 2, "Only stereo supported so far");

	WAVEFORMATEX *pwfx = NULL;

	hr = pAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, pwaveformatex, &pwfx);
	assert_msg_hr(hr, "pAudioClient->IsFormatSupported failed");

	hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, hns_requested_duration, 0,
	                              pwaveformatex, NULL);
	assert_msg_hr(hr, "pAudioClient->Initialize failed");

	// Get the size of the allocated buffer.
	hr = pAudioClient->GetBufferSize(&bufferFrameCount);
	assert_msg_hr(hr, "pAudioClient->GetBufferSize failed");

	// Resize ringbuffer to fit all the samples for the requested duration
	//unsigned int new_size = bufferFrameCount; // do not account for channels, its mono
	//audio_buffer.resize(new_size);
	//log_msg_ln("AudioSink: Buffer resized to {}", new_size);

	audio_buffer.resize(MAX_DFT_SIZE);
	audio_buffer_50_100.resize(MAX_DFT_SIZE);

	// Fill with zeros
	std::fill(audio_buffer.begin(), audio_buffer.end(), 0.0f);
	std::fill(audio_buffer_50_100.begin(), audio_buffer_50_100.end(), 0.0f);

	hr = pAudioClient->GetService(IID_IAudioCaptureClient, (void **)&pCaptureClient);
	assert_msg_hr(hr, "pAudioClient->GetService failed");

	// Start recording.
	hr = pAudioClient->Start();
	assert_msg_hr(hr, "pAudioClient->Start failed");

	initialized = true;
}

void process_packet_buffer(void *pData, const unsigned int size)
{
	float *p = (float *)pData;
	// There can only be two channels at this point: left and right (because of assertions)
	for (unsigned int i = 0; i < size; i += 2)
	{
		// average l/r to create mono, maintaining samplerate
		const float downsampled = 0.5 * (p[i] + p[i + 1]);
		// alternatively average 4 samples to reduce samplerate by 2
		audio_buffer.push_front(downsampled);

		//audio_buffer_50_100.push_front(Filter::butterworth(audio_buffer, audio_buffer_50_100, BWType::BP_50_100));
	}
}

void update()
{
	assert_msg(initialized, "AudioSink::update was called, but it is not initialized!");

	const int bytesPerSamplePerChannel = pwaveformatex->wBitsPerSample / 8;
	const int bytesPerSample = bytesPerSamplePerChannel * pwaveformatex->nChannels;

	if (bytesPerSamplePerChannel != 4)
	{
		log_err("AudioSink: Unimplemented sample size: {}", bytesPerSamplePerChannel);
		return;
	}

	BYTE *pData = nullptr;
	UINT32 num_frames_available = 0;
	DWORD flags = 0;

	// Get the available data in the shared buffer.
	while (SUCCEEDED(pCaptureClient->GetBuffer(&pData, &num_frames_available, &flags, NULL, NULL)))
	{
		if (num_frames_available == 0)
		{
			// Docs says we do not need to release in this case
			break;
		}

		// Parse flags
		if (flags != 0)
		{
			if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
			{
				// The docs say we cant read from the stream when this flag is set,
				// so we just push zeros to prevent problems
				for (size_t i = 0; i < num_frames_available; i++)
				{
					audio_buffer.push_front(0.0f);
				}
			}
			// in case of AUDCLNT_BUFFERFLAGS_TIMESTAMP_ERROR or AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY
			// we just skip the current buffer and continue to prevent glitching

			pCaptureClient->ReleaseBuffer(num_frames_available);
			continue;
		}
		const int buffer_sample_count = num_frames_available * pwaveformatex->nChannels;

		// Do stuff with the audio buffer
		process_packet_buffer((void *)pData, buffer_sample_count);

		// Release the buffer
		pCaptureClient->ReleaseBuffer(num_frames_available);
	}
}

#define SAFE_RELEASE(ob) \
	if (ob)              \
	{                    \
		(ob)->Release(); \
		ob = nullptr;    \
	}

void release()
{
	CoTaskMemFree(pwaveformatex);
	SAFE_RELEASE(pDeviceEnumerator);
	SAFE_RELEASE(pDevice);
	SAFE_RELEASE(pAudioClient);
	SAFE_RELEASE(pCaptureClient);
	CoUninitialize();
}

float get_buffer_duration()
{
	unsigned int buffer_size = get_buffer_size();
	return (float)buffer_size / samplerate;
}

unsigned int get_buffer_size()
{
	assert_msg(audio_buffer.capacity() > 0, "AudioSink::get_buffer_size called, but audio_buffer is empty!");
	return audio_buffer.capacity();
}

void get_buffer(float *buffer, unsigned int size)
{
	if (!buffer)
	{
		return;
	}

	std::copy(audio_buffer.begin(), audio_buffer.begin() + size, buffer);
}

void get_buffer(float *buffer, unsigned int size, AudioType type)
{
	if (!buffer)
	{
		return;
	}

	boost::circular_buffer<float> *target = nullptr;
	if (type == AudioType::Original)
	{
		target = &audio_buffer;
	}
	else if (type == AudioType::Kick)
	{
		target = &audio_buffer_50_100;
	}
	else
	{
		log_err("AudioSink::get_buffer called with unknown type");
		return;
	}

	if (target)
	{
		std::copy(target->begin(), target->begin() + size, buffer);
	}
}

unsigned int get_samplerate()
{
	return samplerate;
}

}; // namespace AudioSink