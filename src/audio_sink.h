#pragma once
#include "stdafx.h"

#include <Windows.h>

typedef void (*pfn_ProcessPacketBuffer)(float *pData, unsigned int size);

namespace AudioSink
{

extern bool initialized;
extern unsigned int samplerate;
extern unsigned int buffer_size;
extern double *buffer;

void init();
void update(const sf::Time &dtTime);
void release();

void set_process_packet_buffer_callback(pfn_ProcessPacketBuffer callback);
void resize_buffers(unsigned int new_size);

}; // namespace AudioSink