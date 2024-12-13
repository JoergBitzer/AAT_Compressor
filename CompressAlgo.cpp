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
    m_sidechainsig.clear();
    m_sidechainsig.setSize(1,m_nrofSamplesPerBlock);

}

int jade::CompressorAlgo::processSamples(juce::AudioBuffer<float>& data)
{
    computeSidechainSignal(data, m_sidechainsig);
    int nrofsamples = data.getNumSamples();
    int nrofchns = data.getNumChannels();
    auto dataptr = data.getArrayOfWritePointers();
    auto sidechainptr = m_sidechainsig.getWritePointer(0);
    
    // add here a delay (for look ahead)
    
    for (int kk = 0; kk< nrofsamples; kk++)
    {
        for (int cc = 0; cc < nrofchns; cc++)
        {
            dataptr[cc][kk] *= sidechainptr[kk];
        }
    }

    return 0;
}

int jade::CompressorAlgo::computeSidechainSignal(const juce::AudioBuffer<float> &data, juce::AudioBuffer<float> &sidechain)
{
    int nrofsamples = data.getNumSamples();
    int nrofchns = data.getNumChannels();

    auto dataptr = data.getArrayOfReadPointers();
    auto sidechainptr = sidechain.getWritePointer(0);


    for (int kk = 0; kk < nrofsamples;kk++)
    {
    // combine chns
        float in;
        float max = 0.f;
        for (int cc = 0; cc < nrofchns; cc++)
        {
            float absin = fabs(dataptr[cc][kk]);
            if (absin> max)
                max = absin;
            
            // alternative is mean
            // max += dataptr[cc][kk]
            // alternative is mean of absoulte value over channels
            // max += absin
            // outside the loop divide by numberofchannelns
        }
        in = max;
    // RMS 
        m_rmsold = m_alpha_rms*m_rmsold + (1.f - m_alpha_rms)*in*in;

    // log
        float rms_log = 10.f*log10f(m_rmsold + 0.00000001f);

    // compressor curve
        float gainreduction = 0.f;
        float gain = computeGain(rms_log, gainreduction);

    // smoothing att/rel
        if (gain < m_smoothedgain)
        {
            m_smoothedgain = m_alpha_att*m_smoothedgain + (1.f - m_alpha_att)*gain;
            m_smoothedgainreduction = m_alpha_att*m_smoothedgainreduction 
                                        + (1.f-m_alpha_att)*gainreduction;
        }
        else
        {
            m_smoothedgain = m_alpha_rel*m_smoothedgain + (1.f - m_alpha_rel)*gain;
            m_smoothedgainreduction = m_alpha_rel*m_smoothedgainreduction 
                                        + (1.f-m_alpha_rel)*gainreduction;
        }

    // lin
        sidechainptr[kk] = powf(10.f,m_smoothedgain/20.f);
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
    m_processmakeup = -(1.f - 1.f/m_ratio) * m_threshold;
}

float jade::CompressorAlgo::tau2alpha(float tau)
{
    float alpha = expf((-1.f)/(tau*0.001*m_fs));
    return alpha;
}
