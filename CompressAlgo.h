

#include <juce_audio_processors/juce_audio_processors.h>

namespace jade
{
class CompressorAlgo
{
public:
    CompressorAlgo();
    void prepareToPlay(float fs, size_t chns, size_t maxnrofsamples);
    int processSamples(juce::AudioBuffer<float>& data);
    int computeSidechainSignal(const juce::AudioBuffer<float>& data, juce::AudioBuffer<float>& sidechain);
    void reset();
    void setSamplingrate(float fs);
    void setThreshold(float thresh){m_threshold = thresh; if(m_autoMakeup) computeAutoMakeup();};
    void setRatio(float ratio){m_ratio = ratio; if(m_autoMakeup) computeAutoMakeup();};
    void setMakeupGain(float makeup) {m_makeupgain = makeup; if (m_autoMakeup) computeAutoMakeup();
    else  m_processmakeup = m_makeupgain; };
    void setAutoMakeup(bool setmakeup){m_autoMakeup = setmakeup; if (m_autoMakeup) computeAutoMakeup();
    else  m_processmakeup = m_makeupgain;};
    void setKneewidth(float kneewidth) {m_kneewidth = kneewidth;};
    void setAttackTime_ms(float att) {m_att_tau_ms = att; m_alpha_att = tau2alpha(m_att_tau_ms);};
    void setReleaseTime_ms(float rel) {m_rel_tau_ms = rel; m_alpha_rel = tau2alpha(m_rel_tau_ms);};
    void setRMSSmoothing_ms(float tau) {m_tau_rms_ms = tau; m_alpha_rms = tau2alpha(m_tau_rms_ms);};

    float getGainReduction(){return m_gainreductionsmoothed;};

protected:
    float m_fs = 44100.f;
    size_t m_chns = 2;
    size_t m_nrofSamplesPerBlock = 1024;
    // static characteristic curve
    float m_ratio = 1.f;
    float m_threshold = 0.f;
    bool m_autoMakeup = true;
    float m_makeupgain = 0.f;
    float m_processmakeup = 0.f;
    float m_kneewidth  = 0.f;
    void computeAutoMakeup();

    // dynamics
    float m_att_tau_ms = 1.f;
    float m_rel_tau_ms = 100.f;
    float m_alpha_att;
    float m_alpha_rel;
    float m_tau_rms_ms;
    float m_alpha_rms;
    float tau2alpha(float tau);

    // sidechain
    juce::AudioBuffer<float> m_sidechainsig;
    float m_rmsold = 0.f;
    float m_smoothedgain = 0.f;

    // gain reduction
    float m_gainreduction = 0.f;
    float m_gainreductionsmoothed = 0.f;
    

    float computeGain(float input)
    {
        float output = 0.f;
        if (input - m_threshold < - m_kneewidth/2.f)
            output = input;
        else
        {
            if (input - m_threshold > m_kneewidth/2.f)
                output = m_threshold + (input - m_threshold) / m_ratio;
            else
                output =  input + (1.f/m_ratio - 1.f) * (input - m_threshold + m_kneewidth/2.f)*(input - m_threshold + m_kneewidth/2.f) / (2.f*m_kneewidth);
        }        
        output += m_processmakeup;
        float gain = output - input;
        m_gainreduction = m_processmakeup - gain;
        return gain;
    }
};



}