#pragma once
#include "stdafx.h"

#define MAX_DFT_SIZE ((uint16_t)1 << 14)

class DFT
{
private:
	fftwf_plan m_plan;
	unsigned int m_last_dft_size;
	fftwf_complex *m_out_complex;

	void release();

public:
	DFT();
	~DFT();

	// specify the size of the input data, out comes size/2+1 data points
	void execute(unsigned int dft_size, float *in_data, float *out_data);
};