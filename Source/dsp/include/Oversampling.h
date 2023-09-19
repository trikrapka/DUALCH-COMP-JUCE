#pragma once

#include <JuceHeader.h>

class Oversampling
{
public:
    Oversampling(int factor);
    ~Oversampling();

    void setOversamplingFactor(int factor, int blockSize);
    void processOversampledBlock(AudioBuffer<float>& buffer);
    void copyOversampledToBuffer(AudioBuffer<float>& buffer);
    void reset();

    int getOversamplingFactor() const;

private:
    int oversamplingFactor;
    AudioBuffer<float> oversampledBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Oversampling)
};
