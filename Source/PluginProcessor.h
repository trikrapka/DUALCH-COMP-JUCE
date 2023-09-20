/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

using namespace juce::dsp;

//==============================================================================
/**
*/
class DualChannelAudioProcessor : public AudioProcessor, public AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    DualChannelAudioProcessor();
    ~DualChannelAudioProcessor();

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    std::unique_ptr<AudioParameterChoice> lrmsModeParam;
    
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    bool canAddBus(bool isInput) const override;

    BusesProperties makeBusesProperties();

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const String getProgramName(int index) override;
    void changeProgramName(int index, const String& newName) override;

    //==============================================================================
    void getStateInformation(MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    void parameterChanged(const String& parameterID, float newValue) override;
    AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    void process(AudioBuffer<float> buffer);

    //==============================================================================
    void toggleLinking();
    void toggleLR();
    void toggleHighPass();
    void toggleLowPass();
    void toggleSidechain();
    void toggleOversampling();

    void setOversamplingFactor(float value);

    void setHighPass(float value);
    void setLowPass(float value);

    float getLeftGainReduction();
    float getRightGainReduction();

    bool applyLinking = false;

private:
    //==============================================================================
    Atomic<float> leftGainReduction = 0.0f;
    Atomic<float> rightGainReduction = 0.0f;
    //==============================================================================
    float hightPassValue = 6.0f;
    float lowPassValue = 6.0f;
    //==============================================================================
    float oversamplingFactor = 2.0f;
    //==============================================================================
    bool applyLRMode = true;
    bool applySidechain = false;
    bool applyOversampling = false;
    //==============================================================================
    bool applyHighPass = false;
    bool applyLowPass = false;
    //==============================================================================
    Gain<float> leftInputGainComputer;
    Gain<float> rightInuptGainComputer;
    Gain<float> leftOutputGainComputer;
    Gain<float> rightOutputGainComputer;
    Gain<float> sidechainGainComputer;
    Compressor<float> leftCompressor;
    Compressor<float> rightCompressor;

    Gain<float> linkInputGainComputer;
    Gain<float> linkOutputGainComputer;
    Compressor<float> linkCompressor;

    DryWetMixer<float> dryWetMixer;
    //==============================================================================
    AudioProcessorValueTreeState parameters;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DualChannelAudioProcessor)
};
