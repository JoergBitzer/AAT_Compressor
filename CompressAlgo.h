

#include <juce_audio_processors/juce_audio_processors.h>

namespace jade
{
class CompressorAlgo
{
public:
    CompressorAlgo();
    void prepareToPlay(float fs, size_t chns, size_t maxnrofsamples);
    int processSamples(juce::AudioBuffer<float>& data);
    int computeSideSignal(const juce::AudioBuffer<float>& data, juce::AudioBuffer<float>& outsidesignal);
    void reset();
    void setSamplingrate(float fs);
    void setThreshold(float thresh){m_threshold = thresh; if(m_autoMakeup) computeAutoMakeup();};
    void setRatio(float ratio){m_ratio = ratio; if(m_autoMakeup) computeAutoMakeup();};
    void setMakeupGain(float makeup) {if (!m_autoMakeup) m_makeupgain = makeup;};
    void setKneewidth(float kneewidth) {m_kneewidth = kneewidth;};
    void setAttackTime_ms(float att) {m_att_tau_ms = att; m_alpha_att = tau2alpha(m_att_tau_ms);};
    void setReleaseTime_ms(float rel) {m_rel_tau_ms = rel; m_alpha_rel = tau2alpha(m_rel_tau_ms);};
    void setRMSSmoothing_ms(float tau) {m_tau_rms_ms = tau; m_alpha_rms = tau2alpha(m_tau_rms_ms);};



protected:
    float m_fs = 44100.f;
    size_t m_chns = 2;
    size_t m_nrofSamplesPerBlock = 1024;
    // static characteristic curve
    float m_ratio = 1.f;
    float m_threshold = 0.f;
    bool m_autoMakeup = true;
    float m_makeupgain = 0.f;
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

    // computeGain
    float computeGain(float rms_log);
    juce::AudioBuffer<float> m_sideSignal;

    // smoothing rms
    float m_rmssquared_old = 0.f;

    // smoothing gain
    float m_gain_old = 0.f;

};



}