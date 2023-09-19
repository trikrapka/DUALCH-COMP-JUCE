
#pragma once
#include "LevelDetector.h"
#include "GainComputer.h"
#include "../JuceLibraryCode/JuceHeader.h"

class Compressor
{
public:

    Compressor() = default;
    ~Compressor();

    // Prepares compressor with a ProcessSpec-Object containing samplerate, blocksize and number of channels
    void prepare(const dsp::ProcessSpec& ps);

    // Sets compressor to bypassed/not bypassed
    void setPower(bool);

    // Sets input in dB
    void setInput(float);

    // Sets threshold in dB
    void setThreshold(float);

    // Sets ratio in dB
    void setRatio(float);

    // Sets knee-width in dB (> 0 = soft knee)
    void setKnee(float);

    // Sets make-up gain in dB
    void setMakeup(float);

    // Sets mix 0.0f - 1.0f
    void setMix(float);

    // Sets attack time in milliseconds
    void setAttack(float);

    // Sets release time in milliseconds
    void setRelease(float);

    // Gets current make-up gain value
    float getMakeup();

    // Return current sampleRate
    double getSampleRate();

    float getMaxGainReduction();

    // Processes input buffer
    void process(AudioBuffer<float>& buffer, int num_samples);

private:
    inline void applyInputGain(AudioBuffer<float>&, int);

    //Directly initialize process spec to avoid debugging problems
    juce::dsp::ProcessSpec procSpec{-1, 0, 0};

    AudioBuffer<float> originalSignal;
    std::vector<float> sidechainSignal;
    float* rawSidechainSignal{nullptr};

    LevelDetector ballistics;
    GainComputer gainComputer;

    float input{0.0f};
    float prevInput{0.0f};
    float makeup{0.0f};
    bool bypassed{false};
    float mix{1.0f};
    float maxGainReduction{0.0f};
};
