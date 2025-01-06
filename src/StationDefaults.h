#ifndef STATION_DEFAULTS_H
#define STATION_DEFAULTS_H

#include "Config.h"

struct StationDefault {
  const char* name;
  int frequency;
  WaveBand band;
  const char* message;
};

namespace StationDefaults {
const StationDefault STATIONS[] = {
    // Long Wave
    {"Athens", 308, WaveBand::LONG_WAVE, "FIRST CLUE FOUND IN LOBBY"},
    {"Budapest", 1024, WaveBand::LONG_WAVE, "CHECK BEHIND BLUE PAINTING"},
    {"Warsaw", 1433, WaveBand::LONG_WAVE, "LOCKER 237 CODE 4521"},
    {"Prague", 2048, WaveBand::LONG_WAVE, "RED KEY OPENS NEXT DOOR"},
    {"Cairo", 2252, WaveBand::LONG_WAVE, "LOOK UNDER THE STAIRS"},
    {"Monaco", 2457, WaveBand::LONG_WAVE, "BASEMENT HOLDS SECRET"},
    {"Oslo", 2970, WaveBand::LONG_WAVE, "CHECK THE BOOKSHELF"},
    {"Ankara", 3276, WaveBand::LONG_WAVE, "THIRD FLOOR NEXT"},
    {"Tunis", 3993, WaveBand::LONG_WAVE, "FINAL CODE 1893"},

    // Medium Wave
    {"Hilversum", 308, WaveBand::MEDIUM_WAVE, "START IN CONFERENCE ROOM"},
    {"Edinburgh", 922, WaveBand::MEDIUM_WAVE, "GREEN BOX HAS TOOLS"},
    {"Luxembourg", 1024, WaveBand::MEDIUM_WAVE, "PASSWORD IS EAGLE"},
    {"Dublin", 2048, WaveBand::MEDIUM_WAVE, "CHECK KITCHEN DRAWER"},
    {"London", 2150, WaveBand::MEDIUM_WAVE, "FOLLOW THE MAP"},
    {"Vienna", 2457, WaveBand::MEDIUM_WAVE, "NEXT CLUE IN VAULT"},
    {"Berlin", 3072, WaveBand::MEDIUM_WAVE, "COMBINATION 8675"},
    {"Rome", 3276, WaveBand::MEDIUM_WAVE, "LOOK BEHIND CLOCK"},
    {"Paris", 3993, WaveBand::MEDIUM_WAVE, "FINAL ROOM KEY"},

    // Short Wave
    {"Canberra", 308, WaveBand::SHORT_WAVE, "BEGIN AT RECEPTION"},
    {"Jerusalem", 922, WaveBand::SHORT_WAVE, "CHECK THE SAFE"},
    {"Bangkok", 1331, WaveBand::SHORT_WAVE, "WHITE CABINET TOP"},
    {"Havana", 2048, WaveBand::SHORT_WAVE, "USB KEY IN DRAWER"},
    {"Beijing", 2355, WaveBand::SHORT_WAVE, "FOLLOW RED MARKS"},
    {"Cape Town", 2560, WaveBand::SHORT_WAVE, "NEXT CODE 9312"},
    {"Toronto", 2765, WaveBand::SHORT_WAVE, "CHECK OLD FILES"},
    {"Washington", 3481, WaveBand::SHORT_WAVE, "LAST DOOR LEFT"},
    {"Moscow", 3993, WaveBand::SHORT_WAVE, "VICTORY ACHIEVED"}};

const size_t STATION_COUNT = sizeof(STATIONS) / sizeof(STATIONS[0]);
}  // namespace StationDefaults

#endif
