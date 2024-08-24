#include "stdafx.h"

#include "AudioSink.h"

#include <comdef.h>

#define PRINT_COM_ERROR(hr) _com_error ___err(hr); LPCTSTR ___errMsg = ___err.ErrorMessage(); _D(___errMsg);


#define RETURN_ON_ERROR(hres) if (FAILED(hres)) { PRINT_COM_ERROR(hres); return false; }
#define BREAK_ON_ERROR(hres) if (FAILED(hres)) { PRINT_COM_ERROR(hres); break; }

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

AudioSink::AudioSink()
{
}

AudioSink::~AudioSink()
{
    release();
}

bool AudioSink::init()
{
    HRESULT hr;

    hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    RETURN_ON_ERROR(hr);

    hr = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&pDeviceEnumerator);
    RETURN_ON_ERROR(hr);

    hr = pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    RETURN_ON_ERROR(hr);

    hr = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&pAudioClient);
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

    WAVEFORMATEX* pwfx = NULL;

    hr = pAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, pwaveformatex, &pwfx);
    RETURN_ON_ERROR(hr);

    hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, hnsRequestedDuration, 0, pwaveformatex, NULL);
    RETURN_ON_ERROR(hr);

    // Get the size of the allocated buffer.
    hr = pAudioClient->GetBufferSize(&bufferFrameCount);
    RETURN_ON_ERROR(hr);

    hr = pAudioClient->GetService(IID_IAudioCaptureClient, (void**)&pCaptureClient);
    RETURN_ON_ERROR(hr);

    // Calculate the actual duration of the allocated buffer.
    //hnsActualDuration = (double)REFTIMES_PER_SEC * bufferFrameCount / pwaveformatex->nSamplesPerSec;

    fftPlan = fftwf_plan_dft_r2c_1d(FFT_SIZE, this->fftInput, this->fftOutputComplex, 0);

    //this->m_bStopThread = false;

    // Start recording.
    hr = pAudioClient->Start();
    RETURN_ON_ERROR(hr);

    // Begin thread to write data to buffer
    //thread = std::thread(&AudioSink::sinkthread, this);

    this->m_bInitialized = true;

    return true;
}

void AudioSink::setPbCallback(callback_ProcessPacketBuffer callback)
{
    CallbackProcessPacketBuffer = callback;
}

#define N (FFT_SIZE - 1)
void AudioSink::applyWindowing()
{
    const AudioWindowFunction wf = config.audio.windowfunction;
    float* data = this->fftInput;

    if (wf == AudioWindowFunction::None || wf == AudioWindowFunction::Rect)
    {
        return;
    }
    else if (wf == AudioWindowFunction::Blackman)
    {
        static const float a0 = 0.42f;
        static const float a1 = 0.5f;
        static const float a2 = 0.08f;
        for (int i = 0; i < FFT_SIZE; i++)
        {
            data[i] *= a0 - a1 * cos(2.f * PIf * i / N) + a2 * cos(4.f * PIf * i / N);
        }

    }
    else if (wf == AudioWindowFunction::Hamming)
    {
        for (int i = 0; i < FFT_SIZE; i++)
        {
            data[i] *= 0.54f - 0.46f * cosf(2 * PIf * i / N);
        }
    }
    else if (wf == AudioWindowFunction::Hann)
    {
        for (int i = 0; i < FFT_SIZE; i++)
        {
            data[i] *= 0.5f - 0.5f * cosf(2 * PIf * i / N);
        }
    }
    else if (wf == AudioWindowFunction::Triangle)
    {
        for (int i = 0; i < FFT_SIZE; i++)
        {
            data[i] *= 1.f - fabsf(2.f * (i - 0.5f * N) / N);
        }
    }
    else if (wf == AudioWindowFunction::Welch)
    {
        for (int i = 0; i < FFT_SIZE; i++)
        {
            data[i] *= 1.f - powf((i - N / 2.f) / (N / 2.f), 2.f);
        }
    }
    else if (wf == AudioWindowFunction::FlatTop)
    {
        static const float a0 = 0.21557895f;
        static const float a1 = 0.41663158f;
        static const float a2 = 0.277263158f;
        static const float a3 = 0.083578947f;
        static const float a4 = 0.006947368f;

        for (int i = 0; i < FFT_SIZE; i++)
        {
            data[i] *= a0 - a1 * cosf(2 * PIf * i / N) + a2 * cosf(4 * PIf * i / N) - a3 * cosf(6 * PIf * i / N) + a4 * cosf(8 * PIf * i / N);
        }
    }
    else if (wf == AudioWindowFunction::BlackmanHarris)
    {
        static const float a0 = 0.35875f;
        static const float a1 = 0.48829f;
        static const float a2 = 0.14128f;
        static const float a3 = 0.01168f;
        for (int i = 0; i < FFT_SIZE; i++)
        {
            data[i] *= a0 - a1 * cosf(2 * PIf * i / N) + a2 * cosf(4 * PIf * i / N) - a3 * cosf(6 * PIf * i / N);
        }
    }
    else
    {
        _D("Unknown window function");
    }
}
#undef N

void AudioSink::update(const sf::Time& dtTime)
{
    if (!m_bInitialized) return;

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
            pData = NULL;  // Tell CopyData to write silence.
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

        CallbackProcessPacketBuffer(m_rawmonodata, (float*)pData, bufferSampleCount);

        // copy data from deque to fft input
        std::copy(m_rawmonodata.begin(), m_rawmonodata.end(), fftInput);

        applyWindowing();

        // apply fft
        fftwf_execute(fftPlan);

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

                float floatval = 1.f / dbRange * (dbout - dbMin);
                Output[i] = clamp(floatval, 0.f, 1.f);
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

float AudioSink::getFreqPerSample()
{
    return (float)state.audio_samplerate / (float)FFT_SIZE_HALF;
}

#define SAFE_RELEASE(ob) if (ob) { (ob)->Release(); ob = nullptr; }

void AudioSink::release()
{
    if(!m_bInitialized) return;
    m_bInitialized = false;

    if (fftPlan)
    {
        fftwf_destroy_plan(fftPlan);
        fftPlan = nullptr;
    }

    CoTaskMemFree(pwaveformatex);
    SAFE_RELEASE(pDeviceEnumerator);
    SAFE_RELEASE(pDevice);
    SAFE_RELEASE(pAudioClient);
    SAFE_RELEASE(pCaptureClient);
    CoUninitialize();

}

AudioSink g_audiosink;