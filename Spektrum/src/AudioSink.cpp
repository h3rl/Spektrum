#include "stdafx.h"

#include "AudioSink.h"

#define EXIT_ON_ERROR(hres)  \
				  if (FAILED(hres)) { Release(); assert("audiosinkerror"); }
#define SAFE_RELEASE(punk)  \
                  if ((punk) != NULL)  \
                    { (punk)->Release(); (punk) = NULL; }

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

AudioSink::AudioSink()
{
	//this->init();
}

AudioSink::~AudioSink()
{
	this->stop();
}

void AudioSink::initWASAPI()
{
    HRESULT hr;

    hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    EXIT_ON_ERROR(hr);

    hr = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&pDeviceEnumerator);
    EXIT_ON_ERROR(hr);

    hr = pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    EXIT_ON_ERROR(hr);

    hr = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&pAudioClient);
    EXIT_ON_ERROR(hr);

    hr = pAudioClient->GetMixFormat(&pwaveformatex);
    EXIT_ON_ERROR(hr);

    //pwaveformatex->wFormatTag = WAVE_FORMAT_PCM;
    //pwaveformatex->nChannels = 2;
    //pwaveformatex->nSamplesPerSec = 16000;
    //pwaveformatex->wBitsPerSample = 16;
    //pwaveformatex->nBlockAlign = pwaveformatex->nChannels * pwaveformatex->wBitsPerSample / 8;
    //pwaveformatex->nAvgBytesPerSec = pwaveformatex->nSamplesPerSec * pwaveformatex->nBlockAlign;
    //pwaveformatex->cbSize = 0;

    WAVEFORMATEX* pwfx = NULL;

    hr = pAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, pwaveformatex, &pwfx);
    EXIT_ON_ERROR(hr);

    hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, hnsRequestedDuration, 0, pwaveformatex, NULL);
    EXIT_ON_ERROR(hr);

    // Get the size of the allocated buffer.
    hr = pAudioClient->GetBufferSize(&bufferFrameCount);
    EXIT_ON_ERROR(hr);

    hr = pAudioClient->GetService(IID_IAudioCaptureClient, (void**)&pCaptureClient);
    EXIT_ON_ERROR(hr);

    // Calculate the actual duration of the allocated buffer.
    hnsActualDuration = (double)REFTIMES_PER_SEC * bufferFrameCount / pwaveformatex->nSamplesPerSec;
}

void AudioSink::initFFTW3()
{
    fftPlan = fftwf_plan_r2r_1d(FFT_SIZE, nullptr, nullptr, fftwf_r2r_kind::FFTW_R2HC, FFTW_ESTIMATE);
}

void AudioSink::releaseFFTW3()
{
    if (fftPlan)
    {
        fftwf_destroy_plan(fftPlan);
    }
}


void AudioSink::init()
{
	if (this->initialized)
	{
		return;
	}

    this->initWASAPI();
    this->initFFTW3();

	this->initialized = true;
    this->bStopThread = false;
}

void AudioSink::sinkthread()
{
    const int bytesPerSamplePerChannel = pwaveformatex->wBitsPerSample / 8;
    const int bytesPerSample = bytesPerSamplePerChannel * pwaveformatex->nChannels;

    _D("Bytes per sample per channel: " << bytesPerSamplePerChannel);

    while (bStopThread == false)
    {
        // Sleep for half the buffer duration.
        //Sleep(hnsActualDuration / REFTIMES_PER_MILLISEC / 2);
        while (true)
        {
            try
            {
                hr = pCaptureClient->GetNextPacketSize(&packetLength);
                EXIT_ON_ERROR(hr);

                if (packetLength == 0)
                {
                    break;
                }

                pData = NULL;

                // Get the available data in the shared buffer.
                hr = pCaptureClient->GetBuffer(&pData, &numFramesAvailable, &flags, NULL, NULL);
                EXIT_ON_ERROR(hr);

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
					std::cout << "short uniplemented" << std::endl;
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
                EXIT_ON_ERROR(hr);
            }
            catch (const std::exception&)
            {
                _D("Error: " << GetLastError());
                break;
            }
        }
	
    }

    hr = pAudioClient->Stop();  // Stop recording.
    EXIT_ON_ERROR(hr);

    Release();
}

void AudioSink::start()
{
    hr = pAudioClient->Start();  // Start recording.
    EXIT_ON_ERROR(hr);

    // Begin thread to write data to buffer
    thread = std::thread(&AudioSink::sinkthread, this);
}

void AudioSink::stop()
{
    bStopThread = true;
    
    // wait for thread to finish
    // thread calls Release() and stops recording
    thread.join();

	CoUninitialize();
}

void AudioSink::Release()
{
    CoTaskMemFree(pwaveformatex);
    SAFE_RELEASE(pDeviceEnumerator);
    SAFE_RELEASE(pDevice);
    SAFE_RELEASE(pAudioClient);
    SAFE_RELEASE(pCaptureClient);
}
