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
    uint8_t ledBrightness = 255;
    bool ledsInitialized = false;

    // Constants for LED control
    static constexpr uint8_t LED_MIN_BRIGHTNESS = 10;
    static constexpr uint8_t LED_MAX_BRIGHTNESS = 255;
    static constexpr uint8_t LED_PWM_CHANNEL = 2; // Use different channel from audio
    static constexpr uint8_t LED_PWM_RESOLUTION = 8;
    static constexpr uint32_t LED_PWM_FREQ = 5000;

    // LED to Band mapping
    struct BandLED
    {
        WaveBand band;
        uint8_t pin;
    };

    static const BandLED BAND_LEDS[];
    static constexpr size_t NUM_BANDS = 3;
};

#endif