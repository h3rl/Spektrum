#include "dft.h"
#include "logging.h"
#include "audio_windowing.h"
#include <cassert>

DFT::DFT()
    : m_size(0)
    , m_in(nullptr)
    , m_out(nullptr)
    , m_allocated_buffers(false)
{
}

DFT::~DFT()
{
	fftw_destroy_plan(m_plan);
	fftw_free(m_out_complex);
	delete[] m_out;
}

void DFT::execute()
{
	// Apply windowing function
	double factor = 0.025;
	// zero pad from factor to end
	unsigned int start_idx = m_size * factor;
	memset((void *)(m_in + start_idx), 0, m_size - start_idx);
	apply_windowing(AudioWindowFunction::Hamming, m_in, m_size);

	fftw_execute(m_plan);

	// Calculate abs of output
	for (unsigned int i = 0; i < m_size / 2 + 1; i++)
	{
		const double real = m_out_complex[i][0];
		const double imag = m_out_complex[i][1];
		m_out[i] = sqrt(real * real + imag * imag);
	}
}

void DFT::create(unsigned int size, double *in)
{
	m_in = in;
	resize(size);
}

void DFT::resize(unsigned int new_size)
{
	// check if multiple of 2
	if (new_size & (new_size - 1))
	{
		log_warn("DFT size faster if power of 2");
	}
	// no point in resizing buffers if the size is the same, but we still need a new plan
	if (new_size == m_size)
	{
		return;
	}

	if (m_allocated_buffers)
	{
		fftw_destroy_plan(m_plan);
		fftw_free(m_out_complex);
		delete[] m_out;
	}

	if (new_size > m_size || m_size == 0)
	{
		// resize buffers
		m_out = new double[new_size / 2 + 1];
		m_out_complex = fftw_alloc_complex(new_size / 2 + 1);
	}

	// Given n inputs, the fft calculates n/2+1 complex outputs
	m_plan = fftw_plan_dft_r2c_1d(new_size, m_in, m_out_complex, 0);
	assert_msg(m_plan, "FFTW plan creation failed");

	m_size = new_size;
	m_allocated_buffers = true;
}

void DFT::normalize()
{
	const double N = m_size;
	m_out[0] /= N;
	for (unsigned int i = 1; i < m_size / 2; i++)
	{
		m_out[i] *= 2.0 / N;
	}
	if (m_size % 2 == 0)
	{
		m_out[m_size / 2] /= N;
	}
}

double *DFT::getOutput()
{
	return m_out;
}

unsigned int DFT::getOutputSize()
{
	return m_size / 2 + 1;
}