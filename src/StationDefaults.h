#ifndef STATION_DEFAULTS_H
#define STATION_DEFAULTS_H

#include "Config.h"

struct StationDefault
{
    const char *name;
    int frequency;
    WaveBand band;
    const char *message;
};

namespace StationDefaults
{
    const StationDefault STATIONS[] = {
        // Long Wave
        {"Athens", 300, WaveBand::LONG_WAVE, "FIRST CLUE FOUND IN LOBBY"},
        {"Budapest", 1000, WaveBand::LONG_WAVE, "CHECK BEHIND BLUE PAINTING"},
        {"Warsaw", 1400, WaveBand::LONG_WAVE, "LOCKER 237 CODE 4521"},
        {"Prague", 2000, WaveBand::LONG_WAVE, "RED KEY OPENS NEXT DOOR"},
        {"Cairo", 2200, WaveBand::LONG_WAVE, "LOOK UNDER THE STAIRS"},
        {"Monaco", 2400, WaveBand::LONG_WAVE, "BASEMENT HOLDS SECRET"},
        {"Oslo", 2900, WaveBand::LONG_WAVE, "CHECK THE BOOKSHELF"},
        {"Ankara", 3200, WaveBand::LONG_WAVE, "THIRD FLOOR NEXT"},
        {"Tunis", 3900, WaveBand::LONG_WAVE, "FINAL CODE 1893"},

        // Medium Wave
        {"Hilversum", 300, WaveBand::MEDIUM_WAVE, "START IN CONFERENCE ROOM"},
        {"Edinburgh", 900, WaveBand::MEDIUM_WAVE, "GREEN BOX HAS TOOLS"},
        {"Luxembourg", 1000, WaveBand::MEDIUM_WAVE, "PASSWORD IS EAGLE"},
        {"Dublin", 2000, WaveBand::MEDIUM_WAVE, "CHECK KITCHEN DRAWER"},
        {"London", 2100, WaveBand::MEDIUM_WAVE, "FOLLOW THE MAP"},
        {"Vienna", 2400, WaveBand::MEDIUM_WAVE, "NEXT CLUE IN VAULT"},
        {"Berlin", 3000, WaveBand::MEDIUM_WAVE, "COMBINATION 8675"},
        {"Rome", 3200, WaveBand::MEDIUM_WAVE, "LOOK BEHIND CLOCK"},
        {"Paris", 3900, WaveBand::MEDIUM_WAVE, "FINAL ROOM KEY"},

        // Short Wave
        {"Canberra", 300, WaveBand::SHORT_WAVE, "BEGIN AT RECEPTION"},
        {"Jerusalem", 900, WaveBand::SHORT_WAVE, "CHECK THE SAFE"},
        {"Bangkok", 1300, WaveBand::SHORT_WAVE, "WHITE CABINET TOP"},
        {"Havana", 2000, WaveBand::SHORT_WAVE, "USB KEY IN DRAWER"},
        {"Beijing", 2300, WaveBand::SHORT_WAVE, "FOLLOW RED MARKS"},
        {"Cape Town", 2500, WaveBand::SHORT_WAVE, "NEXT CODE 9312"},
        {"Toronto", 2700, WaveBand::SHORT_WAVE, "CHECK OLD FILES"},
        {"Washington", 3400, WaveBand::SHORT_WAVE, "LAST DOOR LEFT"},
        {"Moscow", 3900, WaveBand::SHORT_WAVE, "VICTORY ACHIEVED"}};

    const size_t STATION_COUNT = sizeof(STATIONS) / sizeof(STATIONS[0]);
}

#endif
