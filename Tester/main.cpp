#include <numbers>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "WaveOut.h"

int main()
{
    // generate a simple test signal 1s long
    float len_s = 1.f;
    float fs = 44100.f;
    float f0 = 440.f;
    int chns = 2;
    juce::AudioBuffer<float> data(chns, fs * len_s);

    auto dataptrs = data.getArrayOfWritePointers();
    for (int i = 0; i < data.getNumSamples(); i++)
    {
        float t = static_cast<float>(i) / fs;
        dataptrs[0][i] = 0.5f * sin(2.f * std::numbers::pi * f0 * t);
        dataptrs[1][i] = 0.5f * sin(2.f * std::numbers::pi * f0 * t);
    }

}