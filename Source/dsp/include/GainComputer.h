
#pragma once


class GainComputer
{
public:

    GainComputer();
    
    void setThreshold(float);
    void setRatio(float);
    
    float applyCompression(float&);

    void applyCompressionToBuffer(float*, int);

private:
    float threshold{-20.0f};
    float ratio{2.0f};
    float knee{6.0f}, kneeHalf{3.0f};
    float slope{-0.5f};
};
