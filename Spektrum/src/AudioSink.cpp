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
    hnsActualDuration = (double)REFTIMES_PER_SEC * bufferFrameCount / pwaveformatex->nSamplesPerSec;

    return true;
}

bool AudioSink::initFFTW3()
{
    fftPlan = fftwf_plan_r2r_1d(FFT_SIZE, nullptr, nullptr, fftwf_r2r_kind::FFTW_R2HC, FFTW_ESTIMATE);
    return true;
}

void AudioSink::releaseFFTW3()
{
    if (fftPlan)
    {
        fftwf_destroy_plan(fftPlan);
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
        Sleep(hnsActualDuration / REFTIMES_PER_MILLISEC / 2);
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
            if (bytesPerSamplePerChannel == 4) // float
            {
                float* pfData = (float*)pData;
                ZeroMemory(fftInput, sizeof(float) * FFT_SIZE);
                for (int i = 0; i < bufferSampleCount; i++)
                {
                    fftInput[i] = pfData[i];
                }

			}
            else if (bytesPerSamplePerChannel == 2) // short
            {
                _D("short uniplemented");
                BREAK_ON_ERROR(-1);
			}
            else if (bytesPerSamplePerChannel == 8) // double
            {
                _D("double unimplemented");
                BREAK_ON_ERROR(-1);
            }

            //TODO: USE DATA
            //for (int i = 0; i < inpsize; i++)
            //{
            //    // apply Hann window to reduce spectral leakage
            //    fftInput[i] = fftInput[i] * (0.5f - 0.5f * cos(2.f * (float)PI * i / (FFT_SIZE - 1)));
            //}

            // Perform the forward FFT on the input buffer
            fftwf_execute_r2r(fftPlan, fftInput, fftOutput);

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
