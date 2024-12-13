#include <math.h>
#include "Quetscher.h"

#include "PluginProcessor.h"

QuetscherAudio::QuetscherAudio(QuetscherAudioProcessor* processor)
:SynchronBlockProcessor(), m_processor(processor)
{
}

void QuetscherAudio::prepareToPlay(double sampleRate, int max_samplesPerBlock, int max_channels)
{
    juce::ignoreUnused(max_samplesPerBlock,max_channels);
    int synchronblocksize;
    synchronblocksize = static_cast<int>(round(g_desired_blocksize_ms * sampleRate * 0.001)); // 0.001 to transform ms to seconds;
    if (g_forcePowerOf2)
    {
        int nextpowerof2 = int(log2(synchronblocksize))+1;
        synchronblocksize = int(pow(2,nextpowerof2));
    }
    prepareSynchronProcessing(max_channels,synchronblocksize);
    m_Latency += synchronblocksize;
    // here your code
    m_algo.prepareToPlay(sampleRate,max_channels,max_samplesPerBlock);
    m_algo.setAutoMakeup(false);
    m_algo.setRatio(2.f);
    m_algo.setKneewidth(20.f);
    m_algo.setThreshold(-40.f);
    m_algo.setAttackTime_ms(10.f);
    m_algo.setReleaseTime_ms(100.f);
    m_algo.setRMSSmoothing_ms(5.f);
}

int QuetscherAudio::processSynchronBlock(juce::AudioBuffer<float> & buffer, juce::MidiBuffer &midiMessages, int NrOfBlocksSinceLastProcessBlock)
{
    juce::ignoreUnused(midiMessages, NrOfBlocksSinceLastProcessBlock);

    bool somethingchanged = false;
    somethingchanged = m_Ratioparam.updateWithNotification(m_Ratio);
    if(somethingchanged)
    {
        m_algo.setRatio(m_Ratio);
    }
    somethingchanged = m_Thresholdparam.updateWithNotification(m_Threshold);
    if(somethingchanged)
    {
        m_algo.setThreshold(m_Threshold);
    }
    somethingchanged = m_Kneewidthparam.updateWithNotification(m_Kneewidth);
    if(somethingchanged)
    {
        m_algo.setKneewidth(m_Kneewidth);
    }
    somethingchanged = m_Makeupgainparam.updateWithNotification(m_Makeupgain);
    if(somethingchanged)
    {
        m_algo.setMakeupGain(m_Makeupgain);
    }
    somethingchanged = m_paramMakeupSwitch.updateWithNotification(m_MakeupSwitch);
    if(somethingchanged)
    {
        m_algo.setAutoMakeup(m_MakeupSwitch);
    }
    somethingchanged = m_Attackparam.updateWithNotification(m_Attack);
    if(somethingchanged)
    {
        m_algo.setAttackTime_ms(m_Attack);
    }
    somethingchanged = m_Releaseparam.updateWithNotification(m_Release);
    if(somethingchanged)
    {
        m_algo.setReleaseTime_ms(m_Release);
    }
    somethingchanged = m_RMSparam.updateWithNotification(m_RMS);
    if(somethingchanged)
    {
        m_algo.setRMSSmoothing_ms(m_RMS);
    }


    m_algo.processSamples(buffer);
    return 0;
}

void QuetscherAudio::addParameter(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &paramVector)
{
    // this is just a placeholder (necessary for compiling/testing the template)
    paramVector.push_back(std::make_unique<AudioParameterFloat>(g_paramRatio.ID,
        g_paramRatio.name,
        NormalisableRange<float>(g_paramRatio.minValue, g_paramRatio.maxValue),
        g_paramRatio.defaultValue,
        AudioParameterFloatAttributes().withLabel (g_paramRatio.unitName)
                                        .withCategory (juce::AudioProcessorParameter::genericParameter)
                                        // or two additional lines with lambdas to convert data for display
                                        .withStringFromValueFunction (std::move ([](float value, int MaxLen) { value = int((value) * 10) * 0.1f;  return (String(value, MaxLen)); }))
                                        // .withValueFromStringFunction (std::move ([](const String& text) {return text.getFloatValue(); }))
                        ));
    paramVector.push_back(std::make_unique<AudioParameterFloat>(g_paramThreshold.ID,
        g_paramThreshold.name,
        NormalisableRange<float>(g_paramThreshold.minValue, g_paramThreshold.maxValue),
        g_paramThreshold.defaultValue,
        AudioParameterFloatAttributes().withLabel (g_paramThreshold.unitName)
                                        .withCategory (juce::AudioProcessorParameter::genericParameter)
                                        // or two additional lines with lambdas to convert data for display
                                        .withStringFromValueFunction (std::move ([](float value, int MaxLen) { value = int((value) * 10) * 0.1f;  return (String(value, MaxLen)); }))
                                        // .withValueFromStringFunction (std::move ([](const String& text) {return text.getFloatValue(); }))
                        ));

    paramVector.push_back(std::make_unique<AudioParameterFloat>(g_paramKneewidth.ID,
        g_paramKneewidth.name,
        NormalisableRange<float>(g_paramKneewidth.minValue, g_paramKneewidth.maxValue),
        g_paramKneewidth.defaultValue,
        AudioParameterFloatAttributes().withLabel (g_paramKneewidth.unitName)
                                        .withCategory (juce::AudioProcessorParameter::genericParameter)
                                        // or two additional lines with lambdas to convert data for display
                                        .withStringFromValueFunction (std::move ([](float value, int MaxLen) { value = int((value) * 10) * 0.1f;  return (String(value, MaxLen)); }))
                                        // .withValueFromStringFunction (std::move ([](const String& text) {return text.getFloatValue(); }))
                        ));

    paramVector.push_back(std::make_unique<AudioParameterFloat>(g_paramMakeupgain.ID,
        g_paramMakeupgain.name,
        NormalisableRange<float>(g_paramMakeupgain.minValue, g_paramMakeupgain.maxValue),
        g_paramMakeupgain.defaultValue,
        AudioParameterFloatAttributes().withLabel (g_paramMakeupgain.unitName)
                                        .withCategory (juce::AudioProcessorParameter::genericParameter)
                                        // or two additional lines with lambdas to convert data for display
                                        .withStringFromValueFunction (std::move ([](float value, int MaxLen) { value = int((value) * 10) * 0.1f;  return (String(value, MaxLen)); }))
                                        // .withValueFromStringFunction (std::move ([](const String& text) {return text.getFloatValue(); }))
                        ));
    paramVector.push_back(std::make_unique<AudioParameterBool>(g_paramMakeupSwitch.ID,
        g_paramMakeupSwitch.name,
        g_paramMakeupSwitch.defaultValue,
        AudioParameterBoolAttributes()//.withLabel (g_paramLinkLR.unitName)
                                        .withCategory (juce::AudioProcessorParameter::genericParameter)
                                        // or two additional lines with lambdas to convert data for display
                                        //.withStringFromValueFunction (std::move ([](float value, int MaxLen) { value = int((value) * 100);  return (String(value, MaxLen)); }))
                                        // .withValueFromStringFunction (std::move ([](const String& text) {return text.getFloatValue(); }))
                        ));

    paramVector.push_back(std::make_unique<AudioParameterFloat>(g_paramAttack.ID,
        g_paramAttack.name,
        NormalisableRange<float>(g_paramAttack.minValue, g_paramAttack.maxValue),
        g_paramAttack.defaultValue,
        AudioParameterFloatAttributes().withLabel (g_paramAttack.unitName)
                                        .withCategory (juce::AudioProcessorParameter::genericParameter)
                                        // or two additional lines with lambdas to convert data for display
                                        .withStringFromValueFunction (std::move ([](float value, int MaxLen) { value = int((value) * 10) * 0.1f;  return (String(value, MaxLen)); }))
                                        // .withValueFromStringFunction (std::move ([](const String& text) {return text.getFloatValue(); }))
                        ));
    paramVector.push_back(std::make_unique<AudioParameterFloat>(g_paramRelease.ID,
        g_paramRelease.name,
        NormalisableRange<float>(g_paramRelease.minValue, g_paramRelease.maxValue),
        g_paramRelease.defaultValue,
        AudioParameterFloatAttributes().withLabel (g_paramRelease.unitName)
                                        .withCategory (juce::AudioProcessorParameter::genericParameter)
                                        // or two additional lines with lambdas to convert data for display
                                        .withStringFromValueFunction (std::move ([](float value, int MaxLen) { value = int((value) * 10) * 0.1f;  return (String(value, MaxLen)); }))
                                        // .withValueFromStringFunction (std::move ([](const String& text) {return text.getFloatValue(); }))
                        ));
    paramVector.push_back(std::make_unique<AudioParameterFloat>(g_paramRMS.ID,
        g_paramRMS.name,
        NormalisableRange<float>(g_paramRMS.minValue, g_paramRMS.maxValue),
        g_paramRMS.defaultValue,
        AudioParameterFloatAttributes().withLabel (g_paramRMS.unitName)
                                        .withCategory (juce::AudioProcessorParameter::genericParameter)
                                        // or two additional lines with lambdas to convert data for display
                                        .withStringFromValueFunction (std::move ([](float value, int MaxLen) { value = int((value) * 10) * 0.1f;  return (String(value, MaxLen)); }))
                                        // .withValueFromStringFunction (std::move ([](const String& text) {return text.getFloatValue(); }))
                        ));


}

void QuetscherAudio::prepareParameter(std::unique_ptr<juce::AudioProcessorValueTreeState> &vts)
{
    m_Ratioparam.prepareParameter(vts->getRawParameterValue(g_paramRatio.ID));
    m_Thresholdparam.prepareParameter(vts->getRawParameterValue(g_paramThreshold.ID));
    m_Kneewidthparam.prepareParameter(vts->getRawParameterValue(g_paramKneewidth.ID));
    m_Makeupgainparam.prepareParameter(vts->getRawParameterValue(g_paramMakeupgain.ID));
    m_paramMakeupSwitch.prepareParameter(vts->getRawParameterValue(g_paramMakeupSwitch.ID));

    m_Attackparam.prepareParameter(vts->getRawParameterValue(g_paramAttack.ID));
    m_Releaseparam.prepareParameter(vts->getRawParameterValue(g_paramRelease.ID));
    m_RMSparam.prepareParameter(vts->getRawParameterValue(g_paramRMS.ID));
}


QuetscherGUI::QuetscherGUI(QuetscherAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts)
:m_processor(p) ,m_apvts(apvts)
{
    m_RatioSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    m_RatioSlider.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 70, 20);
    m_RatioSlider.setRange(g_paramRatio.minValue, g_paramRatio.maxValue);
    m_RatioSlider.setTextValueSuffix(g_paramRatio.unitName);
    auto val = m_apvts.getRawParameterValue(g_paramRatio.ID);
    m_RatioSlider.setValue(*val);
    m_RatioAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(m_apvts, g_paramRatio.ID, m_RatioSlider);
    addAndMakeVisible(m_RatioSlider);
    // m_RatioSlider.onValueChange = [this] {m_IRDisplay.setRatio(m_RatioSlider.getValue());};

    m_ThresholdSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    m_ThresholdSlider.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 70, 20);
    m_ThresholdSlider.setRange(g_paramThreshold.minValue, g_paramThreshold.maxValue);
    m_ThresholdSlider.setTextValueSuffix(g_paramThreshold.unitName);
    val = m_apvts.getRawParameterValue(g_paramThreshold.ID);
    m_ThresholdSlider.setValue(*val);
    m_ThresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(m_apvts, g_paramThreshold.ID, m_ThresholdSlider);
    addAndMakeVisible(m_ThresholdSlider);

    // m_RatioSlider.onValueChange = [this] {m_IRDisplay.setRatio(m_RatioSlider.getValue());};
    m_KneewidthSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    m_KneewidthSlider.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 70, 20);
    m_KneewidthSlider.setRange(g_paramKneewidth.minValue, g_paramKneewidth.maxValue);
    m_KneewidthSlider.setTextValueSuffix(g_paramKneewidth.unitName);
    val = m_apvts.getRawParameterValue(g_paramKneewidth.ID);
    m_KneewidthSlider.setValue(*val);
    m_KneewidthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(m_apvts, g_paramKneewidth.ID, m_KneewidthSlider);
    addAndMakeVisible(m_KneewidthSlider);
    // m_RatioSlider.onValueChange = [this] {m_IRDisplay.setRatio(m_RatioSlider.getValue());};

    m_MakeupgainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    m_MakeupgainSlider.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 70, 20);
    m_MakeupgainSlider.setRange(g_paramMakeupgain.minValue, g_paramMakeupgain.maxValue);
    m_MakeupgainSlider.setTextValueSuffix(g_paramMakeupgain.unitName);
    val = m_apvts.getRawParameterValue(g_paramMakeupgain.ID);
    m_MakeupgainSlider.setValue(*val);
    m_MakeupgainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(m_apvts, g_paramMakeupgain.ID, m_MakeupgainSlider);
    addAndMakeVisible(m_MakeupgainSlider);
    // m_RatioSlider.onValueChange = [this] {m_IRDisplay.setRatio(m_RatioSlider.getValue());};
    
    m_MakeupSwitch.setButtonText("Auto Makeup");
    m_MakeupSwitch.setToggleState(g_paramMakeupSwitch.defaultValue, false);
    m_MakeupSwitchAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(m_apvts, g_paramMakeupSwitch.ID, m_MakeupSwitch);
    addAndMakeVisible(m_MakeupSwitch);

    m_AttackSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    m_AttackSlider.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 70, 20);
    m_AttackSlider.setRange(g_paramAttack.minValue, g_paramAttack.maxValue);
    m_AttackSlider.setTextValueSuffix(g_paramAttack.unitName);
    val = m_apvts.getRawParameterValue(g_paramAttack.ID);
    m_AttackSlider.setValue(*val);
    m_AttackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(m_apvts, g_paramAttack.ID, m_AttackSlider);
    addAndMakeVisible(m_AttackSlider);
    // m_RatioSlider.onValueChange = [this] {m_IRDisplay.setRatio(m_RatioSlider.getValue());};
    
    m_ReleaseSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    m_ReleaseSlider.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 70, 20);
    m_ReleaseSlider.setRange(g_paramRelease.minValue, g_paramRelease.maxValue);
    m_ReleaseSlider.setTextValueSuffix(g_paramRelease.unitName);
    val = m_apvts.getRawParameterValue(g_paramRelease.ID);
    m_ReleaseSlider.setValue(*val);
    m_ReleaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(m_apvts, g_paramRelease.ID, m_ReleaseSlider);
    addAndMakeVisible(m_ReleaseSlider);
    // m_RatioSlider.onValueChange = [this] {m_IRDisplay.setRatio(m_RatioSlider.getValue());};

    m_RMSSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    m_RMSSlider.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 70, 20);
    m_RMSSlider.setRange(g_paramRMS.minValue, g_paramRMS.maxValue);
    m_RMSSlider.setTextValueSuffix(g_paramRMS.unitName);
    val = m_apvts.getRawParameterValue(g_paramRMS.ID);
    m_RMSSlider.setValue(*val);
    m_RMSAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(m_apvts, g_paramRMS.ID, m_RMSSlider);
    addAndMakeVisible(m_RMSSlider);
    // m_RatioSlider.onValueChange = [this] {m_IRDisplay.setRatio(m_RatioSlider.getValue());};
    startTimerHz(30);
}

void QuetscherGUI::paint(juce::Graphics &g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId).brighter(0.3f));

    g.setColour (juce::Colours::white);
    g.setFont (12.0f);
    
    juce::String text2display = "Quetscher V " + juce::String(PLUGIN_VERSION_MAJOR) + "." + juce::String(PLUGIN_VERSION_MINOR) + "." + juce::String(PLUGIN_VERSION_PATCH);
    g.drawFittedText (text2display, getLocalBounds(), juce::Justification::bottomLeft, 1);

    //g.setColour (juce::Colours::lightgreen);
    //g.setFont (20.0f);
    //g.drawText(juce::String(m_gainreduction),getWidth()-100,100,100,60,juce::Justification::centred);

    // Anzeige zeichnen
    float scalefactor = m_processor.getScaleFactor();
    int width = getWidth();
    int height = getHeight();
    int xstart = width-scalefactor*50;
    int ystart = scalefactor*50;
    int displaywidth = 30*scalefactor;
    int displayheight = height - (ystart + 20)*scalefactor;

    g.setColour (juce::Colours::darkgrey.darker(0.8));
    g.fillRect(xstart, ystart,displaywidth,displayheight);     

    // anzeige
    float maxgainred = 25;

    if (m_gainreduction>5.f)
        g.setColour (juce::Colours::green.brighter(0.6));


    if (m_gainreduction>=maxgainred)
        m_gainreduction = maxgainred;
    
    float gainred_nomalized = m_gainreduction/maxgainred; 

    g.setColour (juce::Colours::green.brighter(0.4));
    g.fillRect(xstart, ystart,displaywidth,static_cast<int> (displayheight*gainred_nomalized));     


}

void QuetscherGUI::resized()
{
	auto r = getLocalBounds();
    
    // if you have to place several components, use scaleFactor
    //int width = r.getWidth();
	//float scaleFactor = float(width)/g_minGuiSize_x;

    // use the given canvas in r
    int height = getHeight();
    int width = getWidth();

    float scaleFactor = m_processor.getScaleFactor();
// layout
    int startx = r.getX() ;
    int starty = r.getY() ;
    int knobwidth = 80 * scaleFactor;
    int knobheight = 80 * scaleFactor;
    int distance_y = 20 * scaleFactor;
    int distance_x = 10 * scaleFactor;
    m_RatioSlider.setBounds(startx,starty,knobwidth,knobheight);
    m_ThresholdSlider.setBounds(startx + 1*(knobwidth + distance_x) ,starty ,knobwidth,knobheight);
    m_KneewidthSlider.setBounds(startx + 2*(knobwidth + distance_x) ,starty ,knobwidth,knobheight);
    m_MakeupgainSlider.setBounds(startx + 3*(knobwidth + distance_x) ,starty ,knobwidth,knobheight);

    m_AttackSlider.setBounds(startx,starty + distance_y + knobheight ,knobwidth,knobheight);
    m_ReleaseSlider.setBounds(startx + 1*(knobwidth + distance_x) ,starty + distance_y + knobheight ,knobwidth,knobheight);
    m_RMSSlider.setBounds(startx + 2*(knobwidth + distance_x) ,starty + distance_y + knobheight ,knobwidth,knobheight);


    int buttonWidth = 60*scaleFactor;
    int buttonHeight = 30*scaleFactor;
    m_MakeupSwitch.setBounds(startx + 4*(knobwidth + distance_x) ,starty,buttonWidth,buttonHeight);


}

void QuetscherGUI::timerCallback()
{
    m_gainreduction = m_processor.m_algo.getGainReduction();
    repaint();
}
