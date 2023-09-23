
#pragma once

class LevelEnvelopeFollower
{
public:
    LevelEnvelopeFollower() = default;
    
    void prepare(const double& fs);

    void setPeakDecay(float dc);
    void setRmsDecay(float dc);
    void updatePeak(const float* const* channelData, int numChannels, int numSamples);
    void updateRMS(const float* const* channelData, int numChannels, int numSamples);
    float getPeak();
    
    float getRMS();

private:
    float currMaxPeak{0.0f};
    float currMaxRMS{0.0f};
    float peakDecay{0.99992f};
    float rmsDecay{0.95f};
    float peakDecayInSeconds{0.5f};
    float rmsDecayInSeconds{0.0f};
    int peakDecayInSamples{0};
    int rmsDecayInSamples{0};
    double sampleRate{0.0f};
};
