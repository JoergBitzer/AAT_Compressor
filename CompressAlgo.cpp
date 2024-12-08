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
    m_sideSignal.setSize(1, m_nrofSamplesPerBlock);

}

int jade::CompressorAlgo::processSamples(juce::AudioBuffer<float>& data)
{

    
    computeSideSignal(data, m_sideSignal);

    int nrofSamples = data.getNumSamples();
    int nrofChns = data.getNumChannels();
    auto dataptrs = data.getArrayOfWritePointers();
    auto sideptr = m_sideSignal.getReadPointer(0);
    for (int i = 0; i < nrofSamples; i++)
    {
        for (int j = 0; j < nrofChns; j++)
        {
            dataptrs[j][i] *= sideptr[i];
        }
    }
    return 0;
}

int jade::CompressorAlgo::computeSideSignal(const juce::AudioBuffer<float> &data, juce::AudioBuffer<float> &outsidesignal)
{
    int nrofSamples = data.getNumSamples();
    int nrofChns = data.getNumChannels();
    auto dataptrs = data.getArrayOfReadPointers();
    auto sideptr = outsidesignal.getWritePointer(0);
    for (int i = 0; i < nrofSamples; i++)
    {
        // find max in chns
        float maxval = 0.f;
        for (int j = 0; j < nrofChns; j++)
        {
            if (fabs(dataptrs[j][i]) > maxval)
                maxval = fabs(dataptrs[j][i]);
        }
        // smoothing for RMS
        m_rmssquared_old = (m_alpha_rms) * m_rmssquared_old + (1.f-m_alpha_rms) * maxval * maxval;
        // to log scale
        float rms_log = 10.f * log10f(m_rmssquared_old + 1e-10);
        // compute gain
        float gain = computeGain(rms_log);
        // smooth gain
        if (gain < m_gain_old)
            m_gain_old = m_alpha_att * m_gain_old + (1.f-m_alpha_att) * gain;
        else
            m_gain_old =  m_alpha_rel * m_gain_old + (1.f - m_alpha_rel)* gain;
        
        // back to linear scale
        float gain_lin = powf(10.f, m_gain_old/20.f);
 
        // store side signal
        sideptr[i] = gain_lin;

    }
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
    float alpha = expf((-1.f)/(tau*0.001*m_fs));
    return alpha;
}

float jade::CompressorAlgo::computeGain(float rms_log)
{
    float output = 0.f;
    if (rms_log - m_threshold < -m_kneewidth/2)
        output = rms_log;
    else
    {
        if (rms_log - m_threshold > m_kneewidth/2)
            output = m_threshold + (rms_log - m_threshold) / m_ratio;
        else
            output =  rms_log + (1/m_ratio - 1) * (rms_log - m_threshold + m_kneewidth/2)* (rms_log - m_threshold + m_kneewidth/2) / (2*m_kneewidth);
    }
    output += m_makeupgain;
    float gain = output - rms_log;
    return gain;
}
