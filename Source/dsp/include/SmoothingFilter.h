
#pragma once

class SmoothingFilter
{
public:

    SmoothingFilter() = default;
    
    void prepare(const double& fs);
    void process(const double& sample);
    void setAlpha(double a);
    void setAlphaWithTime(float timeInSeconds);
    double getState();

private:
    double a1{1.0}, b1{0.0};
    double state{0.0};
    double sampleRate{0.0};
    bool first{true};
};
