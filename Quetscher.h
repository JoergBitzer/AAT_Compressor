#pragma once

#include <vector>
#include <juce_audio_processors/juce_audio_processors.h>

#include "tools/SynchronBlockProcessor.h"
#include "tools/AudioProcessParameter.h"
#include "PluginSettings.h"
#include "CompressAlgo.h"

class QuetscherAudioProcessor;

// This is how we define our parameter as globals to use it in the audio processor as well as in the editor
const struct
{
	const std::string ID = "RatioID";
	const std::string name = "Ratio";
	const std::string unitName = "";
	const float minValue = 1.f;
	const float maxValue = 20.f;
	const float defaultValue = 2.f;
}g_paramRatio;

const struct
{
	const std::string ID = "ThresholdID";
	const std::string name = "Threshold";
	const std::string unitName = " dB";
	const float minValue = -70.f;
	const float maxValue = 0.f;
	const float defaultValue = -20.f;
}g_paramThreshold;

const struct
{
	const std::string ID = "KneewidthID";
	const std::string name = "Kneewidth";
	const std::string unitName = " dB";
	const float minValue = 0.f;
	const float maxValue = 20.f;
	const float defaultValue = 5.f;
}g_paramKneewidth;

const struct
{
	const std::string ID = "MakeupgainID";
	const std::string name = "Makeupgain";
	const std::string unitName = " dB";
	const float minValue = 0.f;
	const float maxValue = 40.f;
	const float defaultValue = 5.f;
}g_paramMakeupgain;

const struct
{
	const std::string ID = "MakeupSwitchID";
	const std::string name = "MakeupSwitch";
	const bool defaultValue = true;
}g_paramMakeupSwitch;

const struct
{
	const std::string ID = "AttackID";
	const std::string name = "Attack";
	const std::string unitName = " ms";
	const float minValue = 1.f;
	const float maxValue = 500.f;
	const float defaultValue = 5.f;
}g_paramAttack;

const struct
{
	const std::string ID = "ReleaseID";
	const std::string name = "Release";
	const std::string unitName = " ms";
	const float minValue = 10.f;
	const float maxValue = 1000.f;
	const float defaultValue = 100.f;
}g_paramRelease;

const struct
{
	const std::string ID = "RMSID";
	const std::string name = "RMS";
	const std::string unitName = " ms";
	const float minValue = 0.05f;
	const float maxValue = 10.f;
	const float defaultValue = 100.f;
}g_paramRMS;


class QuetscherAudio : public SynchronBlockProcessor
{
public:
    QuetscherAudio(QuetscherAudioProcessor* processor);
    void prepareToPlay(double sampleRate, int max_samplesPerBlock, int max_channels);
    virtual int processSynchronBlock(juce::AudioBuffer<float>&, juce::MidiBuffer& midiMessages, int NrOfBlocksSinceLastProcessBlock);

    // parameter handling
  	void addParameter(std::vector < std::unique_ptr<juce::RangedAudioParameter>>& paramVector);
    void prepareParameter(std::unique_ptr<juce::AudioProcessorValueTreeState>&  vts);
    
    // some necessary info for the host
    int getLatency(){return m_Latency;};

	float getGainReduction(){m_gainreduction = m_algo.getGainReduction();  return m_gainreduction;};

private:
	QuetscherAudioProcessor* m_processor;
    int m_Latency = 0;

	jade::CompressorAlgo m_algo;

	jade::AudioProcessParameter<float> m_Ratioparam;
	float m_Ratio = g_paramRatio.defaultValue;

	jade::AudioProcessParameter<float> m_Thresholdparam;
	float m_Threshold = g_paramThreshold.defaultValue;

	jade::AudioProcessParameter<float> m_Kneewidthparam;
	float m_Kneewidth = g_paramKneewidth.defaultValue;

	jade::AudioProcessParameter<float> m_Makeupgainparam;
	float m_Makeupgain = g_paramMakeupgain.defaultValue;

    jade::AudioProcessParameter<bool> m_paramMakeupSwitch;
	bool m_MakeupSwitch = g_paramMakeupSwitch.defaultValue;

	jade::AudioProcessParameter<float> m_Attackparam;
	float m_Attack = g_paramAttack.defaultValue;

	jade::AudioProcessParameter<float> m_Releaseparam;
	float m_Release = g_paramRelease.defaultValue;

	jade::AudioProcessParameter<float> m_RMSparam;
	float m_RMS = g_paramRMS.defaultValue;

	float m_gainreduction = 0.f;

};

class QuetscherGUI : public juce::Component, juce::Timer
{
public:
	QuetscherGUI(QuetscherAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts);
	~QuetscherGUI(){stopTimer();};
	void paint(juce::Graphics& g) override;
	void resized() override;
	void timerCallback() override;

private:
	QuetscherAudioProcessor& m_processor;
    juce::AudioProcessorValueTreeState& m_apvts; 

	juce::Slider m_RatioSlider;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_RatioAttachment;
	juce::Slider m_ThresholdSlider;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_ThresholdAttachment;
	juce::Slider m_KneewidthSlider;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_KneewidthAttachment;
	juce::Slider m_MakeupgainSlider;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_MakeupgainAttachment;

	juce::ToggleButton m_MakeupSwitch;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> m_MakeupSwitchAttachment;

	juce::Slider m_AttackSlider;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_AttackAttachment;

	juce::Slider m_ReleaseSlider;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_ReleaseAttachment;

	juce::Slider m_RMSSlider;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_RMSAttachment;

	float m_gainreduction;
};
