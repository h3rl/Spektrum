#include "stdafx.h"

#include "AudioSink.h"

#define RETURN_ON_ERROR(hres) if (FAILED(hres)) { release(); return false; }
#define BREAK_ON_ERROR(hres) if (FAILED(hres)) { break; }
#define SAFE_RELEASE(ob) if ((ob) != NULL) { (ob)->Release(); (ob) = NULL; }

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

AudioSink::AudioSink()
{
    _D("AudioSink constructor called");
}

AudioSink::~AudioSink()
{
    _D("AudioSink destructor called");
}

bool AudioSink::initWASAPI()
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

    return true;
}

bool AudioSink::initFFTW3()
{
    fftPlan = fftwf_plan_dft_r2c_1d(FFT_SIZE, this->fftInput, this->fftOutputComplex, 0);
    return true;
}

void AudioSink::releaseFFTW3()
{
    if (fftPlan)
    {
        fftwf_destroy_plan(fftPlan);
    }
}

void AudioSink::applyBlackman()
{
    const float alpha = 0.16f;
    const float a0 = (1.f - alpha) / 2.f;
    const float a1 = 0.5f;
    const float a2 = alpha / 2.f;
    for (int i = 0; i < FFT_SIZE; i++)
    {
        // Blackman window
        const float wn = a0 - a1 * cos(2.f * (float)PI * i / FFT_SIZE) + a2 * cos(4.f * (float)PI * i / FFT_SIZE);

        // apply window
        fftInput[i] = fftInput[i] * wn;
    }
}
void AudioSink::applyHamming()
{
    for (int i = 0; i < FFT_SIZE; i++)
    {
        // Hamming window
        const float wn = 0.54f - 0.46f * cos(2.f * (float)PI * i / FFT_SIZE);
        
        // apply window
        fftInput[i] = fftInput[i] * wn;
    }
}

bool AudioSink::init()
{
    if (!this->initWASAPI())
    {
        return false;
    }

    if (!this->initFFTW3())
    {
        return false;
    }

    this->m_bStopThread = false;

    // Start recording.
    hr = pAudioClient->Start();
    RETURN_ON_ERROR(hr);

    // Begin thread to write data to buffer
    thread = std::thread(&AudioSink::sinkthread, this);

    this->m_bInitialized = true;

    return true;
}

void AudioSink::sinkthread()
{
    const int bytesPerSamplePerChannel = pwaveformatex->wBitsPerSample / 8;
    const int bytesPerSample = bytesPerSamplePerChannel * pwaveformatex->nChannels;

    //_D("Bytes per sample per channel: " << bytesPerSamplePerChannel);



    while (!m_bStopThread)
    {
        // Sleep for half the buffer duration.
        //Sleep(hnsActualDuration / REFTIMES_PER_MILLISEC / 2);
        try
        {
            hr = pCaptureClient->GetNextPacketSize(&packetLength);
            BREAK_ON_ERROR(hr);

            if (packetLength == 0)
            {
                continue;
            }

            // Get the available data in the shared buffer.
            hr = pCaptureClient->GetBuffer(&pData, &numFramesAvailable, &flags, NULL, NULL);
            BREAK_ON_ERROR(hr);

            if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
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
            if (bytesPerSamplePerChannel == 8) // double
            {
                _D("double unimplemented");
                BREAK_ON_ERROR(-1);
            }
            else if (bytesPerSamplePerChannel == 4) // float
            {
                float* pfData = (float*)pData;
                for (int i = 0; i < bufferSampleCount; i += 2)
                {
                    const float left = pfData[i];
                    const float right = pfData[i + 1];
                    const float monoval = 0.5f * (right + left); // average to create mono

                    // keep track and limit the size of the raw data
                    if (m_rawmonodata.size() >= FFT_SIZE)
                    {
                        m_rawmonodata.pop_front();
                    }
                    m_rawmonodata.push_back(monoval);
                }

            }
            else if (bytesPerSamplePerChannel == 2) // short
            {
                _D("short uniplemented");
                BREAK_ON_ERROR(-1);
            }

            // copy data from deque to fft input
            std::copy(m_rawmonodata.begin(), m_rawmonodata.end(), fftInput);

            switch (config::audio::windowfunction)
            {
            case Blackman:
                this->applyBlackman();
				break;
            case Hamming:
                this->applyHamming();
                break;
            default:
                break;
            }

            // apply fft
            fftwf_execute(fftPlan);

            // fftOutputComplex is now populated with FFT_SIZE/2 complex numbers
            // find magnitude, store in fftOutput, apply time smoothing

            for (int i = 0; i < FFT_SIZE_HALF; i++)
            {
				const float real = fftOutputComplex[i][0];
				const float imag = fftOutputComplex[i][1];
				const float mag = sqrt(real * real + imag * imag);

                const float tau = config::audio::time_smoothing;
                fftOutput[i] = tau * fftOutput[i] + (1.f - tau) * mag;

                switch (config::audio::barstyle)
                {
                case Linear:
                {
                    Output[i] = fftOutput[i];
                    break;
                }
                case Db:
                {
                    // calc db
                    const float dbout = 20.f * log10(fftOutput[i]);

                    // populate byteFrequencyData
                    const float dbMax = config::audio::max_db;
                    const float dbMin = config::audio::min_db;
                    const float dbRange = dbMax - dbMin;

                    float floatval = 1.f / dbRange * (dbout - dbMin);
                    Output[i] = clamp(floatval,0.f,1.f);
                }
                }
			}

            hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
            BREAK_ON_ERROR(hr);
        }
        catch (const std::exception&)
        {
            _D("Error: " << GetLastError());
            break;
        }
    }
}

void AudioSink::stop()
{
    if (!m_bInitialized) return;

    m_bStopThread = true;
    m_bInitialized = false;
    
    // wait for thread to finish
    // thread calls release() and stops recording
    thread.join();

    release();
}

void AudioSink::release()
{
    CoTaskMemFree(pwaveformatex);
    SAFE_RELEASE(pDeviceEnumerator);
    SAFE_RELEASE(pDevice);
    SAFE_RELEASE(pAudioClient);
    SAFE_RELEASE(pCaptureClient);
    CoUninitialize();
}
