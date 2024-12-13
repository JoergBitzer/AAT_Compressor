
// tester should:
// generate a test file
// process test file
// write test file in and output

#include "WaveOut.h"
#include <juce_audio_basics/juce_audio_basics.h>

#include "../CompressAlgo.h"

int main()
{


    // testsignal
    juce::AudioBuffer<float> data;
    data.clear();
    int nrchns = 2;
    int len = 1;
    float samplerate = 44100.f;

    // Algo aufbauen
    jade::CompressorAlgo algo;
    algo.setRatio(2.f);
    algo.setKneewidth(20.f);
    algo.setThreshold(-40.f);




    data.setSize(nrchns,samplerate*len);
    auto dataptr = data.getArrayOfWritePointers();
    int nrofsamples = data.getNumSamples();
    algo.prepareToPlay(samplerate,nrchns,nrofsamples);
    algo.setAttackTime_ms(10.f);
    algo.setReleaseTime_ms(100.f);
    algo.setRMSSmoothing_ms(5.f);
    for (int kk = 0; kk < nrofsamples; kk++)
    {
        dataptr[0][kk] = 0.01f;
        dataptr[0][kk] = 0.01f;

        if (kk> nrofsamples/2 && kk < 3*nrofsamples/4)
        {
            dataptr[0][kk] = 0.5f;
            dataptr[0][kk] = 0.5f;
        }
    }
    // write input
    WaveOut waveout("CompTestIn.wav",samplerate,nrchns,WaveOut::AudioFormat::INT16);
    waveout.write_file(data);

    // processing
    algo.processSamples(data);

    // write results

    WaveOut waveout2("CompTestOut.wav",samplerate,nrchns,WaveOut::AudioFormat::INT16);
    waveout2.write_file(data);

    return 0;
}