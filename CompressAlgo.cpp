#include "CompressAlgo.h"

jade::CompressorAlgo::CompressorAlgo()
{
}

void jade::CompressorAlgo::prepareToPlay(float fs, size_t chns, size_t maxnrofsamples)
{
    m_fs = fs;
    setSamplingrate(m_fs);
    m_chns = chns;
    m_nrofSamplesPerBlock = maxnrofsamples;

}

int jade::CompressorAlgo::processSamples(juce::AudioBuffer<float> data)
{
    return 0;
}

void jade::CompressorAlgo::reset()
{
}

void jade::CompressorAlgo::setSamplingrate(float fs)
{
    m_fs = fs;
    m_alpha_att = tau2alpha(m_att_tau_ms);
    m_alpha_rel = tau2alpha(m_rel_tau_ms);
    m_alpha_rms = tau2alpha(m_tau_rms_ms);

}

void jade::CompressorAlgo::computeAutoMakeup()
{
    m_makeupgain = -(1.f - 1.f/m_ratio) * m_threshold;
}

float jade::CompressorAlgo::tau2alpha(float tau)
{
    float alpha = 1.f - expf((-1.f)/(tau*0.001*m_fs));
    return alpha;
}
