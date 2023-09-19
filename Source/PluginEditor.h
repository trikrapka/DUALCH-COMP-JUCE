/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "juce_core/juce_core.h"


#include "gui/include/LabeledSlider.h"
#include "gui/include/Meter.h"
#include "gui/include/MeterBackground.h"
#include "gui/include/DualChannelLookAndFeel.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class DualChannelAudioProcessorEditor : public AudioProcessorEditor, Timer, Slider::Listener,
    Button::Listener,
    private AsyncUpdater
{
public:
    DualChannelAudioProcessorEditor(DualChannelAudioProcessor&, AudioProcessorValueTreeState&);
    ~DualChannelAudioProcessorEditor();

    //==============================================================================
    void paint(Graphics&) override;
    void resized() override;
    void timerCallback() override;
    void initWidgets();
    void sliderValueChanged(juce::Slider* slider);

    void handleAsyncUpdate() override;

    void buttonClicked(Button* button) override;
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DualChannelAudioProcessor& processor;
    AudioProcessorValueTreeState& valueTreeState;

    DualChannelLookAndFeel LAF;

    Colour backgroundApp;

    //Widgets
    LabeledSlider firstInputGainSlider;
    LabeledSlider firstAttackSlider;
    LabeledSlider firstReleaseSlider;
    LabeledSlider firstThresholdSlider;
    LabeledSlider firstRatioSlider;
    LabeledSlider firstOutGainSlider;

    LabeledSlider secondInputGainSlider;
    LabeledSlider secondAttackSlider;
    LabeledSlider secondReleaseSlider;
    LabeledSlider secondThresholdSlider;
    LabeledSlider secondRatioSlider;
    LabeledSlider secondOutGainSlider;

    Meter leftGainMeter;
    Meter rightGainMeter;

    LabeledSlider highPassSlider;
    ToggleButton highPassToggle;

    LabeledSlider lowPassSlider;
    ToggleButton lowPassToggle;

    LabeledSlider sidechainSlider;
    ToggleButton sidechainToggle;

    LabeledSlider mixSlider;
    ToggleButton lrToggle;
    ToggleButton linkingToggle;

    bool linkingApplied; //private bool to refer to applyLinking bool
    double clicked;
    bool isChannel1Active = true;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DualChannelAudioProcessorEditor)
};
