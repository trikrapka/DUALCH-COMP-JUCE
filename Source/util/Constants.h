
#pragma once
#include <cstdint>

namespace Constants
{
    namespace Colors
    {
        //Backgrounds
        constexpr uint32_t bg_App{0xff424242};
        constexpr uint32_t bg_DarkGrey{0xff212121};
        constexpr uint32_t bg_MidGrey{0xff616161};
        constexpr uint32_t bg_LightGrey{0xff9e9e9e};

        //Gradients
        constexpr uint32_t gd_LightGreyStart{0xffeeeee};
        constexpr uint32_t gd_LightGreyEnd{0xffc4c4c4};
        constexpr uint32_t gd_DarktGreyStart{0xff646464};
        constexpr uint32_t gd_DarkGreyEnd{0xff464646};

        //Misc
        constexpr uint32_t statusOutline{0xfffb8c00};
        constexpr uint32_t knobShadow{0xff424242};
    }

    namespace Margins
    {
        constexpr float small = 2.0f;
        constexpr float medium = 4.0f;
        constexpr float big = 8.0f;
    }

    namespace Parameter
    {
        constexpr float inputStart = -24.0f;
        constexpr float inputEnd = 24.0f;
        constexpr float inputInterval = 0.1f;

        constexpr float attackStart = 1.0f;
        constexpr float attackEnd = 640.0f;
        constexpr float attackInterval = 0.01f;

        constexpr float releaseStart = 5.0f;
        constexpr float releaseEnd = 5000.0f;
        constexpr float releaseInterval = 0.01f;

        constexpr float thresholdStart = -32.0f;
        constexpr float thresholdEnd = 0.0f;
        constexpr float thresholdInterval = 0.1f;

        constexpr float ratioStart = 1.0f;
        constexpr float ratioEnd = 10.0f;
        constexpr float ratioInterval = 1.0f;

        constexpr float highPassStart = 6.0f;
        constexpr float highPassEnd = 24.0f;
        constexpr float highPassInterval = 6.0f;
        
        constexpr float highPassFreqStart = 20.0f;
        constexpr float highPassFreqEnd = 20000.0f;
        constexpr float highPassFreqInterval = 1.0f;

        constexpr float lowPassStart = 6.0f;
        constexpr float lowPassEnd = 24.0f;
        constexpr float lowPassInterval = 6.0f;

        constexpr float lowPassFreqStart = 20.0f;
        constexpr float lowPassFreqEnd = 20000.0f;
        constexpr float lowPassFreqInterval = 1.0f;

        constexpr float sidechainStart = 0.0f;
        constexpr float sidechainEnd = 80.0f;
        constexpr float sidechainInterval = 0.001f;

        constexpr float mixStart = 0.0f;
        constexpr float mixEnd = 1.0f;
        constexpr float mixInterval = 0.001f;
    }
}
