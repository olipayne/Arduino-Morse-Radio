#include "WaveBandManager.h"

// Define the LED mapping
const WaveBandManager::BandLED WaveBandManager::BAND_LEDS[] = {
    {WaveBand::LONG_WAVE, Pins::LW_LED},
    {WaveBand::MEDIUM_WAVE, Pins::MW_LED},
    {WaveBand::SHORT_WAVE, Pins::SW_LED}};

void WaveBandManager::begin()
{
    initializePins();
    initializeLEDs();
    update(); // Initial update
}

void WaveBandManager::initializePins()
{
    // Initialize switch pins with pullups
    pinMode(Pins::LW_BAND_SWITCH, INPUT_PULLUP);
    pinMode(Pins::MW_BAND_SWITCH, INPUT_PULLUP);

    // Initialize LED pins
    for (size_t i = 0; i < NUM_BANDS; i++)
    {
        pinMode(BAND_LEDS[i].pin, OUTPUT);
        digitalWrite(BAND_LEDS[i].pin, LOW);
    }
}

void WaveBandManager::initializeLEDs()
{
    if (!ledsInitialized)
    {
        // Set up PWM for LED control
        for (size_t i = 0; i < NUM_BANDS; i++)
        {
            ledcSetup(LED_PWM_CHANNEL + i, LED_PWM_FREQ, LED_PWM_RESOLUTION);
            ledcAttachPin(BAND_LEDS[i].pin, LED_PWM_CHANNEL + i);
        }
        ledsInitialized = true;
    }
}

void WaveBandManager::update()
{
    auto &config = ConfigManager::getInstance();
    WaveBand newBand;

    // Read switch states
    if (digitalRead(Pins::LW_BAND_SWITCH) == LOW)
    {
        newBand = WaveBand::LONG_WAVE;
    }
    else if (digitalRead(Pins::MW_BAND_SWITCH) == LOW)
    {
        newBand = WaveBand::MEDIUM_WAVE;
    }
    else
    {
        newBand = WaveBand::SHORT_WAVE; // Default to short wave
    }

    // Only update if band has changed
    if (newBand != config.getWaveBand())
    {
        config.setWaveBand(newBand);
        updateLEDs();
        Log::println("Wave band changed to: ", toString(newBand));
    }
}

WaveBand WaveBandManager::getCurrentBand() const
{
    return ConfigManager::getInstance().getWaveBand();
}

void WaveBandManager::updateLEDs()
{
    turnOffAllBandLEDs();
    updateBandLED(getCurrentBand());
}

void WaveBandManager::turnOffAllBandLEDs()
{
    for (size_t i = 0; i < NUM_BANDS; i++)
    {
        ledcWrite(LED_PWM_CHANNEL + i, 0);
    }
}

void WaveBandManager::updateBandLED(WaveBand band)
{
    for (size_t i = 0; i < NUM_BANDS; i++)
    {
        if (BAND_LEDS[i].band == band)
        {
            ledcWrite(LED_PWM_CHANNEL + i, ledBrightness);
            break;
        }
    }
}

void WaveBandManager::setLEDBrightness(uint8_t brightness)
{
    // Constrain brightness to valid range
    ledBrightness = constrain(brightness, LED_MIN_BRIGHTNESS, LED_MAX_BRIGHTNESS);
    updateLEDs(); // Update LEDs with new brightness
}
