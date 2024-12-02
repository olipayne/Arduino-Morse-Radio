#ifndef WAVEBANDMANAGER_H
#define WAVEBANDMANAGER_H

#include "Config.h"

class WaveBandManager
{
public:
    static WaveBandManager &getInstance()
    {
        static WaveBandManager instance;
        return instance;
    }

    // Initialization
    void begin();

    // Core functionality
    void update();
    WaveBand getCurrentBand() const;

    // LED control
    void updateLEDs();
    void setLEDBrightness(uint8_t brightness);

private:
    WaveBandManager() = default;
    WaveBandManager(const WaveBandManager &) = delete;
    WaveBandManager &operator=(const WaveBandManager &) = delete;

    // Pin initialization
    void initializePins();
    void initializeLEDs();

    // LED state tracking
    void turnOffAllBandLEDs();
    void updateBandLED(WaveBand band);

    // Internal state
    uint8_t ledBrightness = LEDConfig::MAX_BRIGHTNESS;
    bool ledsInitialized = false;

    // LED to Band mapping
    struct BandLED
    {
        WaveBand band;
        uint8_t pin;
        uint8_t pwmChannel;
    };

    static const BandLED BAND_LEDS[];
    static constexpr size_t NUM_BANDS = 3;
};

#endif
