#include "dft.h"
#include "logging.h"
#include "audio_windowing.h"
#include <cassert>

DFT::DFT()
    : m_last_dft_size(0)
    , m_out_complex(nullptr)
{
}

DFT::~DFT()
{
	fftwf_destroy_plan(m_plan);
	fftwf_free(m_out_complex);
}

void DFT::execute(unsigned int dft_size, float *in_data, float *out_data)
{
	if (dft_size != m_last_dft_size)
	{
		fftwf_destroy_plan(m_plan);
		fftwf_free(m_out_complex);
		m_out_complex = fftwf_alloc_complex(dft_size / 2 + 1);
		m_plan = fftwf_plan_dft_r2c_1d(dft_size, in_data, m_out_complex, 0);
	}

	fftwf_execute(m_plan);

	// Calculate abs of output
	for (unsigned int i = 0; i < dft_size / 2 + 1; i++)
	{
		const float real = m_out_complex[i][0];
		const float imag = m_out_complex[i][1];
		const float mag = sqrt(real * real + imag * imag);
		out_data[i] = mag / dft_size;
	}
}