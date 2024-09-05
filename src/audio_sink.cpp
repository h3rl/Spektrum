#include "audio_sink.h"
#include "logging.h"

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

#define RETURN_ON_ERROR(hres)  \
	if (FAILED(hres))          \
	{                          \
		PRINT_COM_ERROR(hres); \
		return false;          \
	}
#define BREAK_ON_ERROR(hres)   \
	if (FAILED(hres))          \
	{                          \
		PRINT_COM_ERROR(hres); \
		break;                 \
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
bool initialized = false;
unsigned int samplerate = 0;
unsigned int buffer_size = 1024;
double *buffer = nullptr; // contains audio_data, idx=0 is latest sample

// ringbuffer to store audio data
boost::circular_buffer<double> ring_buffer;

// WASAPI stuff
REFERENCE_TIME hns_actual_duration;
UINT32 bufferFrameCount;
UINT32 numFramesAvailable;
IMMDeviceEnumerator *pDeviceEnumerator = nullptr;
IMMDevice *pDevice = nullptr;
IAudioClient *pAudioClient = nullptr;
IAudioCaptureClient *pCaptureClient = nullptr;
WAVEFORMATEX *pwaveformatex = nullptr;

pfn_ProcessPacketBuffer p_process_packet_buffer_callback = nullptr;

void init()
{
	HRESULT hr;

	if (initialized)
	{
		log_err("Already initialized");
		return;
	}

	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	assert_msg(SUCCEEDED(hr), "CoInitializeEx failed");

	hr = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator,
	                      (void **)&pDeviceEnumerator);
	assert_msg(SUCCEEDED(hr), "CoCreateInstance failed");

	hr = pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
	assert_msg(SUCCEEDED(hr), "pDeviceEnumerator->GetDefaultAudioEndpoint failed");

	hr = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void **)&pAudioClient);
	assert_msg(SUCCEEDED(hr), "pDevice->Activate failed");

	hr = pAudioClient->GetMixFormat(&pwaveformatex);
	assert_msg(SUCCEEDED(hr), "pAudioClient->GetMixFormat failed");

	//pwaveformatex->wFormatTag = WAVE_FORMAT_PCM;
	//pwaveformatex->nChannels = 2;
	//pwaveformatex->nSamplesPerSec = 16000;
	//pwaveformatex->wBitsPerSample = 16;
	//pwaveformatex->nBlockAlign = pwaveformatex->nChannels * pwaveformatex->wBitsPerSample / 8;
	//pwaveformatex->nAvgBytesPerSec = pwaveformatex->nSamplesPerSec * pwaveformatex->nBlockAlign;
	//pwaveformatex->cbSize = 0;

	// because we downsample later, divide by 2
	// samplerate = pwaveformatex->nSamplesPerSec / 2;
	samplerate = pwaveformatex->nSamplesPerSec;

	log_msg_ln("Samplerate:  {}", pwaveformatex->nSamplesPerSec);
	log_msg_ln("channelcount: {}", pwaveformatex->nChannels);

	assert_msg(pwaveformatex->nChannels == 2, "Only stereo supported so far");

	WAVEFORMATEX *pwfx = NULL;

	hr = pAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, pwaveformatex, &pwfx);
	assert_msg(SUCCEEDED(hr), "pAudioClient->IsFormatSupported failed");

	hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, hns_requested_duration, 0,
	                              pwaveformatex, NULL);
	assert_msg(SUCCEEDED(hr), "pAudioClient->Initialize failed");

	// Get the size of the allocated buffer.
	hr = pAudioClient->GetBufferSize(&bufferFrameCount);
	assert_msg(SUCCEEDED(hr), "pAudioClient->GetBufferSize failed");

	// Resize ringbuffer
	resize_buffers(bufferFrameCount * pwaveformatex->nChannels);

	hr = pAudioClient->GetService(IID_IAudioCaptureClient, (void **)&pCaptureClient);
	assert_msg(SUCCEEDED(hr), "pAudioClient->GetService failed");

	// Start recording.
	hr = pAudioClient->Start();
	assert_msg(SUCCEEDED(hr), "pAudioClient->Start failed");

	initialized = true;
}

void set_process_packet_buffer_callback(pfn_ProcessPacketBuffer callback)
{
	p_process_packet_buffer_callback = callback;
}

void process_packet_buffer(float *pData, const unsigned int size)
{
	// There can only be two channels at this point: left and right (because of assertions)
	for (unsigned int i = 0; i < size; i += 2)
	{
		// average l/r to create mono, maintaining samplerate
		const double downsampled = 0.5 * (pData[i] + pData[i + 1]);
		// alternatively average 4 samples to reduce samplerate by 2
		ring_buffer.push_front(downsampled);
	}
}

void update(const sf::Time &dtTime)
{
	assert_msg(initialized, "Not initialized");

	const int bytesPerSamplePerChannel = pwaveformatex->wBitsPerSample / 8;
	const int bytesPerSample = bytesPerSamplePerChannel * pwaveformatex->nChannels;

	if (bytesPerSamplePerChannel != 4)
	{
		log_err("Unimplemented sample size: {}", bytesPerSamplePerChannel);
		return;
	}

	HRESULT hr;
	bool bufferCleared = false;

	BYTE *pData = nullptr;
	UINT32 num_frames_available = 0;
	DWORD flags = 0;

	// Get the available data in the shared buffer.
	while (SUCCEEDED(pCaptureClient->GetBuffer(&pData, &num_frames_available, &flags, NULL, NULL)))
	{
		if (num_frames_available == 0)
		{
			break;
		}

		if (flags != 0)
		{
			// TODO: handle flags (AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY | AUDCLNT_BUFFERFLAGS_SILENT | AUDCLNT_BUFFERFLAGS_TIMESTAMP_ERROR)
			if (flags & AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY)
			{
				flags &= ~AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY;
				//log_warn("GetBuffer returned AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY");
				// continue to prevent glitching
				pCaptureClient->ReleaseBuffer(num_frames_available);
				continue;
			}
			if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
			{
				flags &= ~AUDCLNT_BUFFERFLAGS_SILENT;
				log_warn("GetBuffer returned AUDCLNT_BUFFERFLAGS_SILENT");
			}
			if (flags & AUDCLNT_BUFFERFLAGS_TIMESTAMP_ERROR)
			{
				flags &= ~AUDCLNT_BUFFERFLAGS_TIMESTAMP_ERROR;
				log_warn("GetBuffer returned AUDCLNT_BUFFERFLAGS_TIMESTAMP_ERROR");
			}
			if (flags != 0)
			{
				log_err("GetBuffer returned unhandled flags: {}", flags);
			}
		}
		const int buffer_sample_count = num_frames_available * pwaveformatex->nChannels;

		// Do stuff with buffer
		// if (p_process_packet_buffer_callback != nullptr)
		// {
		// 	p_process_packet_buffer_callback((float *)pData, buffer_sample_count);
		// }
		//log_msg_ln("Buffer sample count: {}", buffer_sample_count);
		process_packet_buffer((float *)pData, buffer_sample_count);

		pCaptureClient->ReleaseBuffer(num_frames_available);
	}

	// copy ringbuffer to buffer
	std::copy(ring_buffer.begin(), ring_buffer.end(), buffer);
}

void resize_buffers(unsigned int new_size)
{
	if (buffer)
	{
		delete[] buffer;
	}

	ring_buffer.resize(new_size);
	buffer = new double[new_size];

	buffer_size = new_size;
	log_msg_ln("Buffer resized to {}", new_size);
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
}; // namespace AudioSink