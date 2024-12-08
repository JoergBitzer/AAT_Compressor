#include <numbers>

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>

#include "WaveOut.h"
#include "../CompressAlgo.h"

int main()
{
    // generate a simple test signal 1s long
    float len_s = 1.f;
    float fs = 44100.f;
    float f0 = 440.f;
    int chns = 2;
    juce::AudioBuffer<float> data(chns, static_cast<int>(fs * len_s));

    auto dataptrs = data.getArrayOfWritePointers();
    for (int i = 0; i < data.getNumSamples(); i++)
    {
        if (i > fs/2 && i < 3*fs/4)
        {
            dataptrs[0][i] = 0.5f;
            dataptrs[1][i] = 0.5f;
        }
        else
        {
            dataptrs[0][i] = 0.01f;
            dataptrs[1][i] = 0.01f;
        }
    }

    // define compressor
    jade::CompressorAlgo comp;
    comp.prepareToPlay(fs, chns, data.getNumSamples());
    float thresh = -20.f;
    comp.setThreshold(thresh);
    float ratio = 4.f;
    comp.setRatio(ratio);
    float att = 1.f;
    comp.setAttackTime_ms(att);
    float rel = 100.f;
    comp.setReleaseTime_ms(rel);
    float tau_rms = 10.f;
    comp.setRMSSmoothing_ms(tau_rms);   
    float knee = 10.f;
    comp.setKneewidth(knee);

    WaveOut waveout("testin.wav", fs, chns, WaveOut::AudioFormat::INT16);
    waveout.write_file(data);
    // process data
    comp.processSamples(data);
    WaveOut waveout2("testout.wav", fs, chns, WaveOut::AudioFormat::INT16);
    waveout2.write_file(data);


}