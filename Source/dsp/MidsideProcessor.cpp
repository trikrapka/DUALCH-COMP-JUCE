#include "include/MidSideProcessor.h"

MidSideProcessor::MidSideProcessor()
{
}

void MidSideProcessor::prepare(const juce::dsp::ProcessSpec& spec)
{
    midSideBuffer.setSize(2, spec.maximumBlockSize);
    lrCompressor.prepare(spec);  // Prepare the compressor for LR channels
    msCompressor.prepare(spec);  // Prepare the compressor for MS channels
}

void MidSideProcessor::processLR
(AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();

    // Process LR channels using the compressor
    for (int channel = 0; channel < 2; ++channel)
    {
        AudioBuffer<float> tempBuffer(1, numSamples); // Temporary buffer for a single channel
        tempBuffer.copyFrom(0, 0, buffer, channel, 0, numSamples); // Copy channel data to temporary buffer
        lrCompressor.process(tempBuffer, numSamples); // Process the single channel
        buffer.copyFrom(channel, 0, tempBuffer, 0, 0, numSamples); // Copy processed data back to original buffer
    }
}

void MidSideProcessor::processMS(AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    // Split LR channels to MS channels
    for (int channel = 0; channel < numChannels; ++channel)
    {
        midSideBuffer.copyFrom(channel, 0, buffer, channel, 0, numSamples);
    }

    // Process MS channels using the compressor
    msCompressor.process(midSideBuffer, numSamples); // Pass the appropriate number of arguments

    // Convert MS channels back to LR channels
    for (int channel = 0; channel < numChannels; ++channel)
    {
        buffer.copyFrom(channel, 0, midSideBuffer, channel, 0, numSamples);
    }
}


void MidSideProcessor::reset()
{
    midSideBuffer.clear();
}

