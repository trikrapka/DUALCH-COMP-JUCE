
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
    void setPower(bool);
    void setInput(float);
    void setThreshold(float);
    void setRatio(float);
    void setMakeup(float);
    void setMix(float);
    void setAttack(float);
    void setRelease(float);
    float getMakeup();
    
    double getSampleRate();

    float getMaxGainReduction();


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
