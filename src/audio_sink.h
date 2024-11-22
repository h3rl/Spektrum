#pragma once
#include "stdafx.h"

// Todo: make a way to request the timeduration the buffer should have

enum class AudioType : uint8_t
{
	Original,
	Kick
};

namespace AudioSink
{
void init();
void update();
void release();

float get_buffer_duration();
unsigned int get_buffer_size();
void get_buffer(float *buffer, unsigned int size); // returns original audio data
void get_buffer(float *buffer, unsigned int size, AudioType type); // request last "size" samples
unsigned int get_samplerate();
}; // namespace AudioSink