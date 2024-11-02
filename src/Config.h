#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <Preferences.h>

enum MorseSpeed
{
  LOW_SPEED,
  MEDIUM_SPEED,
  HIGH_SPEED
};
enum WaveBand
{
  LONG_WAVE,
  MEDIUM_WAVE,
  SHORT_WAVE
};

extern bool wifiEnabled; // External declaration of Wi-Fi state

extern unsigned int speakerDutyCycle; // Declare speakerDutyCycle as extern
extern unsigned int morseFrequency;
extern String londonMessage;
extern String hilversumMessage;
extern String barcelonaMessage;
extern MorseSpeed morseSpeed;
extern WaveBand currentWave;
extern Preferences preferences;

extern bool morsePlaying;
extern const int SPEAKER_PIN;
extern const int SPEAKER_CHANNEL;
extern bool morseToneOn;
extern unsigned int dashDuration;
extern unsigned int dotDuration;
extern unsigned int partGap;
extern unsigned int characterGap;
const int LEEWAY = 100;

const int PWR_SW = 14;
const int LW_BAND_SW = 12;
const int MW_BAND_SW = 6;
const int SLOW_DECODE = 5;
const int MED_DECODE = 11;
const int AUDIO_ON_OFF = 10;
const int TUNING_ADC = 17;
const int VOLUME_ADC = 18;
const int BACKLIGHT_PIN = 33;
const int POWER_LED_PIN = 35;
const int LW_LED_PIN = 36;
const int MW_LED_PIN = 37;
const int SW_LED_PIN = 38;
const int LOCK_LED_PIN = 43;
const int CARRIER_PWM_PIN = 7;
const int DECODE_PWM_PIN = 3;
const int MORSE_LEDS_PIN = 44;

void saveConfigurations();
void loadConfigurations();

#endif
