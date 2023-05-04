#include "SoundHandler.h"

SoundHandler::AudioData SoundHandler::data = {
    {},
    {},
    nullptr
};

SoundHandler::SoundHandler()
{
    //init();
}

SoundHandler::~SoundHandler()
{
    stop();
}

bool SoundHandler::init()
{
    if (initialized)
    {
        _D("PortAudio is already initialized");
        return false;
    }

    if (!initFFTW3())
        return false;

    if (!initPortAudio())
        return false;

    initialized = true;
    return true;
}

bool SoundHandler::initPortAudio()
{
    PaError err;

    // Initialize PortAudio
    err = Pa_Initialize();
    if (err != paNoError)
    {
        _D("Error initializing PortAudio: " << Pa_GetErrorText(err));
        return false;
    }


    // TODO: add selection of output devices
    // For opening nondefault device
    //PaStreamParameters outputParameters;
    //outputParameters.device = Pa_GetDefaultOutputDevice();
    //outputParameters.channelCount = 0;
    //outputParameters.hostApiSpecificStreamInfo = 0;
    //outputParameters.sampleFormat = 0;
    //outputParameters.suggestedLatency = 0;
    //const int framesPerBuffer = 1024;
    //err = Pa_OpenStream(&audiostream, nullptr, &outputParameters, SAMPLE_RATE, paFramesPerBufferUnspecified, paNoFlag, audioCallback, &data);

    const PaDeviceInfo* devinfo = Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice());

    _D("DEVICE INFO");
    _D("maxOutputChannels: " << devinfo->maxOutputChannels);
    _D("maxInputChannels: " << devinfo->maxInputChannels);
    _D("defaultSampleRate: " << devinfo->defaultSampleRate);
    _D("name: " << devinfo->name << std::endl);


    // 2 input channels to listen to stereo input
    // Open the default input stream
    err = Pa_OpenDefaultStream(&audiostream, 2, devinfo->maxOutputChannels, paFloat32, devinfo->defaultSampleRate, FFT_SIZE, audioCallback, &data);
    if (err != paNoError) {
        _D("Error opening PortAudio stream: " << Pa_GetErrorText(err));
        Pa_Terminate();
        return false;
    }

    // Start the stream
    err = Pa_StartStream(audiostream);
    if (err != paNoError) {
        _D("Error starting PortAudio stream: " << Pa_GetErrorText(err));
        Pa_CloseStream(audiostream);
        Pa_Terminate();
        return false;
    }

    return true;
}


float fft_plan_out[FFT_SIZE];
float fft_plan_in[FFT_SIZE];

bool SoundHandler::initFFTW3()
{
    // Create FFTW plans for forward FFTs
    data.fftPlan = fftwf_plan_r2r_1d(FFT_SIZE, fft_plan_in, fft_plan_out, fftwf_r2r_kind::FFTW_R2HC, FFTW_ESTIMATE);

    if (!data.fftPlan)
    {
        _D("Error initializing fftplan: ");
        return false;
    }

    return true;
}


void SoundHandler::stop()
{
    PaError err;

    if (!initialized)
    {
        return;
    }

    // Stop and close the stream
    err = Pa_StopStream(audiostream);
    if (err != paNoError) {
        _D("Error stopping PortAudio stream: " << Pa_GetErrorText(err));
    }
    err = Pa_CloseStream(audiostream);
    if (err != paNoError) {
        _D("Error closing PortAudio stream: " << Pa_GetErrorText(err));
    }

    // Terminate PortAudio
    Pa_Terminate();

    initialized = false;
}

using std::endl;
using std::cout;

int SoundHandler::audioCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
{
    // Cast the input buffer to a float pointer
    const float* in = static_cast<const float*>(inputBuffer);

    AudioData* ad = (AudioData*)userData;

    if (ad == nullptr)
    {
        _D("UserData is nullptr");
        return paAbort;
    }

    //_D(framesPerBuffer); // 1136


    // Check that we have some input
    //for (int i = 0; i < 25; i++)
    //{
    //    cout << in[i] << " ";
    //}
    //cout << endl;

    // Copy the input buffer to the FFT input buffer and apply a windowing function
    for (int i = 0; i < FFT_SIZE; i++) {
        // apply Hann window to reduce spectral leakage
        ad->fftInput[i] = in[i] * (0.5f - 0.5f * cos(2.f * (float)PI * i / (FFT_SIZE - 1)));
    }

    // Perform the forward FFT on the input buffer
    fftwf_execute_r2r(ad->fftPlan, ad->fftInput, ad->fftOutput);

    // Print the first 10 frequency bins for debugging purposes
    //for (int i = 0; i < FFT_SIZE/2+1; i++) {
    //    cout << ad->fftOutput[i] << " ";
    //}
    //cout << endl << endl;

    // Return paContinue to continue streaming audio
    return paContinue;
}