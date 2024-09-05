#pragma once
#include "stdafx.h"

// Discrete Fourier Transform
class DFT
{
private:
	fftw_plan m_plan;

	bool m_allocated_buffers;
	unsigned int m_size;
	double *m_out;
	double *m_in;
	fftw_complex *m_out_complex;

public:
	DFT();
	~DFT();

	void create(unsigned int size, double *in);
	void resize(unsigned int size);
	void execute();
	void normalize();
	double *getOutput();
	unsigned int getOutputSize();
};

// wanted
// void DFT::execute(size, in, out)