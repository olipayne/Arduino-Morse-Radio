#include "WaveBandManager.h"

void updateWaveBand()
{
    if (digitalRead(LW_BAND_SW) == LOW)
    {
        currentWave = LONG_WAVE;
    }
    else if (digitalRead(MW_BAND_SW) == LOW)
    {
        currentWave = MEDIUM_WAVE;
    }
    else
    {
        currentWave = SHORT_WAVE;
    }
}
