#include "include/Oversampling.h"

Oversampling::Oversampling(int factor)
    : oversamplingFactor(factor), oversampledBuffer(nullptr, 0, 0)
{
}

Oversampling::~Oversampling()
{
}

void Oversampling::setOversamplingFactor(int factor, int blockSize)
{
    oversamplingFactor = factor;
    oversampledBuffer.setSize(1, blockSize * oversamplingFactor);
}

int Oversampling::getOversamplingFactor() const
{
    return oversamplingFactor;
}

void Oversampling::processOversampledBlock(AudioBuffer<float>& buffer)
{
    auto numChannels = buffer.getNumChannels();
    auto numSamples = buffer.getNumSamples();

    oversampledBuffer.clear();

    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getReadPointer(channel);
        auto* oversampledData = oversampledBuffer.getWritePointer(0);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            for (int i = 0; i < oversamplingFactor; ++i)
            {
                oversampledData[sample * oversamplingFactor + i] = channelData[sample];
            }
        }
    }
}

void Oversampling::copyOversampledToBuffer(AudioBuffer<float>& buffer)
{
    auto numChannels = buffer.getNumChannels();
    auto numSamples = buffer.getNumSamples();

    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        auto* oversampledData = oversampledBuffer.getReadPointer(0);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            channelData[sample] = oversampledData[sample * oversamplingFactor];
        }
    }
}

void Oversampling::reset()
{
    oversampledBuffer.clear();
}