#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include "Config.h"

void setVolume(int volume);
void handleAudioPlayback();
void stopAudio();
void pulseDecodePWM();

#endif
