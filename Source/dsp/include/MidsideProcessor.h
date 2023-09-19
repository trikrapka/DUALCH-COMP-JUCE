#include "Compressor.h"
#include "juce_dsp/juce_dsp.h"

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class MidSideProcessor
{
public:
    MidSideProcessor();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void processLR(AudioBuffer<float>& buffer);
    void processMS(AudioBuffer<float>& buffer);
    void reset();

private:
    AudioBuffer<float> midSideBuffer;
    Compressor lrCompressor;
    Compressor msCompressor;
};
