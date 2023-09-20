# Dual Channel Compressor 

![my badge](https://badgen.net/badge/Language/C++/blue)
![my badge](https://badgen.net/badge/Format/VST3/green)
![my badge](https://badgen.net/badge/Framework/JUCE/orange)
![my badge](https://badgen.net/badge/Version/1.0.0.0/pink)

<img src="https://github.com/trikrapka/DUALCH-COMP-JUCE/blob/main/Additions/plugin.png" width="800">

## Features
- Input/Output Gain
- Threshold/Ratio
- Attack/Release
- Mix
- Metering (Gain Reduction)
- Sidechain
- Linking
- MidSide mode
- High/Low Pass Filters
- Oversampling

## Manual
#### General
- **Input:** The Input knob adjusts the level of the signal before any processing happens. Driving the input signal harder into the compressor can be used as an alternative to lowering the threshold.
- **Output:** The Output knob adjusts the level of the signal after processing happens.
- **Mix:** The Mix knob enables you to mix between the dry and processed signal. This can be used for parallel processing and enables use-cases like NY-Style drum compression. 
  
#### Gain Computer
- **Threshold:** The Threshold knob adjusts the level above which the compressor starts attenuating the input signal. The Input knob alternatively can be used to drive the signal harder into the compressor, resulting in a less low threshold being needed. Be careful to not drive the signal too hard or it'll start clipping.
- **Ratio:** The Ratio knob determines how much the signal is attenuated above the chosen threshold. For a ratio of 4:1, one dB remains for every 4dB of input signal above the threshold. At a ratio of 1:1 no compression is happening, as the input is exactly the output. At a ratio of inifinity:1 (knob all the way to the right) the compressor is acting as a limiter, meaning that everything above the treshold is completely compressed away.

#### Ballistics
- **Attack:** The Attack knob sets the time that determines how fast the compression will set in once the signal exceeds the threshold. Generally speaking you want a rather fast attack time for transient-rich signals like drums to minimize overshoot. 
- **Release:** The Release knob sets the time that determines how fast the compressor will recover from the gain reduction once the signal falls under the threshold. Depending on the input signal, short release times may introduce a "pumping" effect and/or distortion.

#### Metering
- **Gainreduction:** This is a simple gainreduction meter.

#### Additional
- **Sidechain** - Sidechain button enables sidechain. You can control the dB using knobs
- **MidSide** - Button to enable MidSide mode
- **Linking** - Button to link two channels controls
- **Oversampling** - To sample a signal at a sampling frequency significantly higher than the Nyquist rate. Button to enable/disable. 3 options: 2x, 4x, 8x.
- **High and Low Pass filters** - The low and high pass buttons makes it possible to cutoff frequencies. You can change the dB/oct value.

### Scheme of the compressor:

![Scheme of the compressor:](https://github.com/trikrapka/DUALCH-COMP-JUCE/blob/main/Additions/Base-Diagram.png)
