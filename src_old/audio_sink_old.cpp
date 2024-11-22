#include "stdafx.h"

#include "logging.h"

#include "config.h"
#include "audio_sink.h"

#include <comdef.h>

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

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

namespace Audio
{

Sink::Sink()
{
}

Sink::~Sink()
{
	release();
}

bool Sink::init()
{
	HRESULT hr;

	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	RETURN_ON_ERROR(hr);

	hr = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator,
	                      (void **)&pDeviceEnumerator);
	RETURN_ON_ERROR(hr);

	hr = pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
	RETURN_ON_ERROR(hr);

	hr = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void **)&pAudioClient);
	RETURN_ON_ERROR(hr);

	hr = pAudioClient->GetMixFormat(&pwaveformatex);
	RETURN_ON_ERROR(hr);

	//pwaveformatex->wFormatTag = WAVE_FORMAT_PCM;
	//pwaveformatex->nChannels = 2;
	//pwaveformatex->nSamplesPerSec = 16000;
	//pwaveformatex->wBitsPerSample = 16;
	//pwaveformatex->nBlockAlign = pwaveformatex->nChannels * pwaveformatex->wBitsPerSample / 8;
	//pwaveformatex->nAvgBytesPerSec = pwaveformatex->nSamplesPerSec * pwaveformatex->nBlockAlign;
	//pwaveformatex->cbSize = 0;

	_D("Samplerate: " << pwaveformatex->nSamplesPerSec);
	_D("channelcount: " << pwaveformatex->nChannels);

	// because we downsample later, divide by 2
	state.audio_samplerate = pwaveformatex->nSamplesPerSec / 2;

	if (pwaveformatex->nChannels != 2)
	{
		_D("Only stereo supported so far");
		RETURN_ON_ERROR(-1);
	}

	WAVEFORMATEX *pwfx = NULL;

	hr = pAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, pwaveformatex, &pwfx);
	RETURN_ON_ERROR(hr);

	hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, hnsRequestedDuration, 0,
	                              pwaveformatex, NULL);
	RETURN_ON_ERROR(hr);

	// Get the size of the allocated buffer.
	hr = pAudioClient->GetBufferSize(&bufferFrameCount);
	RETURN_ON_ERROR(hr);

	hr = pAudioClient->GetService(IID_IAudioCaptureClient, (void **)&pCaptureClient);
	RETURN_ON_ERROR(hr);

	// Calculate the actual duration of the allocated buffer.
	//hnsActualDuration = (float)REFTIMES_PER_SEC * bufferFrameCount / pwaveformatex->nSamplesPerSec;

	fftPlan = fftw_plan_dft_r2c_1d(FFT_SIZE, this->fftInput, this->fftOutputComplex, 0);

	//this->m_bStopThread = false;

	// Start recording.
	hr = pAudioClient->Start();
	RETURN_ON_ERROR(hr);

	// Begin thread to write data to buffer
	//thread = std::thread(&Sink::sinkthread, this);

	this->m_bInitialized = true;

	return true;
}

void Sink::set_process_packet_buffer_callback(pfn_ProcessPacketBuffer callback)
{
	p_process_packet_buffer_callback = callback;
}

void Sink::update(const sf::Time &dtTime)
{
	if (!m_bInitialized)
		return;

	const int bytesPerSamplePerChannel = pwaveformatex->wBitsPerSample / 8;
	const int bytesPerSample = bytesPerSamplePerChannel * pwaveformatex->nChannels;

	bool bufferCleared = false;

	while (true)
	{
		// get next packet
		hr = pCaptureClient->GetNextPacketSize(&packetLength);
		BREAK_ON_ERROR(hr);

		if (packetLength == 0)
		{
			break;
		}

		// Sleep for half the buffer duration.
		//Sleep(hnsActualDuration / REFTIMES_PER_MILLISEC / 2);

		// Get the available data in the shared buffer.
		hr = pCaptureClient->GetBuffer(&pData, &numFramesAvailable, &flags, NULL, NULL);
		bufferCleared = false;
		BREAK_ON_ERROR(hr);

		if (flags & 0b111)
		{
			pData = NULL; // Tell CopyData to write silence.
			break;
		}

		//_D("Num frames available: " << numFramesAvailable);
		const int bufferSampleCount = numFramesAvailable * pwaveformatex->nChannels;

		// NOTE:
		// audioframe = samplesizeinbytes * numberofchannels

		//std::cout << "bytesPerSamplePerChannel: " << bytesPerSamplePerChannel << std::endl;
		//std::cout << "bytesPerSample: " << bytesPerSample << std::endl;
		//std::cout << "bufferSampleCount: " << bufferSampleCount << std::endl;
		//std::cout << "numFramesAvailable: " << numFramesAvailable << std::endl;
		//std::cout << "waveformat" << pwaveformatex->wFormatTag << std::endl;
		/*
		bytesPerSample: 8
		bufferSampleCount: 960
		numFramesAvailable: 480
		bytesPerSamplePerChannel: 4
		*/

		if (bytesPerSamplePerChannel != 4)
		{
			_D("Unimplemented sample size: " << bytesPerSamplePerChannel << " byte value");
			return;
		}

		if (CallbackProcessPacketBuffer == nullptr)
		{
			_D("Callbackfuntion not defined");
			return;
		}

		CallbackProcessPacketBuffer(m_rawmonodata, (float *)pData, bufferSampleCount);

		// copy data from deque to fft input
		std::copy(m_rawmonodata.begin(), m_rawmonodata.end(), fftInput);

		applyWindowing();

		// apply fft
		fftw_execute(fftPlan);

		// fftOutputComplex is now populated with FFT_SIZE/2 complex numbers
		// find magnitude, store in fftOutput, apply time smoothing

		for (int i = 0; i < FFT_SIZE_HALF; i++)
		{
			const float real = fftOutputComplex[i][0];
			const float imag = fftOutputComplex[i][1];
			const float mag = sqrt(real * real + imag * imag);

			const float timeconst = config.audio.time_smoothing;
			const float seconds = dtTime.asSeconds();

			const float tau = 1.0f - std::exp(-seconds / timeconst);
			fftOutput[i] = tau * fftOutput[i] + (1.f - tau) * mag;

			switch (config.audio.barstyle)
			{
			case Linear:
			{
				Output[i] = fftOutput[i];
				break;
			}
			case Logarithmic:
			{
				// calc db
				const float dbout = 20.f * log10(fftOutput[i]);

				// populate byteFrequencyData
				const float dbMax = config.audio.max_db;
				const float dbMin = config.audio.min_db;
				const float dbRange = dbMax - dbMin;

				float doubleval = 1.f / dbRange * (dbout - dbMin);
				Output[i] = clamp(doubleval, 0.f, 1.f);
			}
			}
		}

		hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
		BREAK_ON_ERROR(hr);

		bufferCleared = true;
	}

	if (!bufferCleared)
	{
		// clear buffer
		pCaptureClient->ReleaseBuffer(numFramesAvailable);
	}
}

float Sink::get_frequencies_per_sample()
{
	return (float)state.audio_samplerate / (float)FFT_SIZE_HALF;
}

#define SAFE_RELEASE(ob) \
	if (ob)              \
	{                    \
		(ob)->Release(); \
		ob = nullptr;    \
	}

void Sink::release()
{
	if (!m_bInitialized)
		return;
	m_bInitialized = false;

	if (fftPlan)
	{
		fftw_destroy_plan(fftPlan);
		fftPlan = nullptr;
	}

	CoTaskMemFree(pwaveformatex);
	SAFE_RELEASE(pDeviceEnumerator);
	SAFE_RELEASE(pDevice);
	SAFE_RELEASE(pAudioClient);
	SAFE_RELEASE(pCaptureClient);
	CoUninitialize();
}
}