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

    void process(AudioBlock<float> block);

    //==============================================================================
    void toggleLinking();
    void toggleLR();
    void toggleHighPass();
    void toggleLowPass();
    void toggleSidechain();
    void toggleOversampling();

    void setOversamplingFactor(int value);

    void setHighPass(float value);
    void setLowPass(float value);

    float getLeftGainReduction();
    float getRightGainReduction();
    
    bool applyLinking = false;
    bool applySidechain = false;
    float cutoffFrquencyHP;
    float cutoffFrquencyLP;

private:
    //==============================================================================
    Atomic<float> leftGainReduction;
    Atomic<float> rightGainReduction;
    //==============================================================================
    int oversamplingFactor = 1;
    //==============================================================================
    float hightPassValue = 6.0f;
    float lowPassValue = 6.0f;
    float highPassFreqValue = 20.0f;
    float lowPassFreqValue = 20.0f;
    //==============================================================================
    bool applyLRMode = true;
    bool applyOversampling = false;
    const AudioChannelSet sidechainInput;
    //==============================================================================
    bool applyHighPass = false;
    bool applyLowPass = false;
    //==============================================================================
    bool hasSidechainInput = true;
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

    std::unique_ptr<juce::AlertWindow> alertWindow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DualChannelAudioProcessor)
};
