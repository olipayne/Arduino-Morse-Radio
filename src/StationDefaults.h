#ifndef STATION_DEFAULTS_H
#define STATION_DEFAULTS_H

#include "Config.h"

struct StationDefault {
  const char* name;
  int frequency;
  WaveBand band;
  const char* message;
};

// PROGMEM storage for station names and messages to save RAM
static const char station_athens[] PROGMEM = "Athens";
static const char msg_athens[] PROGMEM = "FIRST CLUE FOUND IN LOBBY";
static const char station_budapest[] PROGMEM = "Budapest";
static const char msg_budapest[] PROGMEM = "CHECK BEHIND BLUE PAINTING";
static const char station_warsaw[] PROGMEM = "Warsaw";
static const char msg_warsaw[] PROGMEM = "LOCKER 237 CODE 4521";
static const char station_prague[] PROGMEM = "Prague";
static const char msg_prague[] PROGMEM = "RED KEY OPENS NEXT DOOR";
static const char station_cairo[] PROGMEM = "Cairo";
static const char msg_cairo[] PROGMEM = "LOOK UNDER THE STAIRS";
static const char station_monaco[] PROGMEM = "Monaco";
static const char msg_monaco[] PROGMEM = "BASEMENT HOLDS SECRET";
static const char station_oslo[] PROGMEM = "Oslo";
static const char msg_oslo[] PROGMEM = "CHECK THE BOOKSHELF";
static const char station_ankara[] PROGMEM = "Ankara";
static const char msg_ankara[] PROGMEM = "THIRD FLOOR NEXT";
static const char station_tunis[] PROGMEM = "Tunis";
static const char msg_tunis[] PROGMEM = "FINAL CODE 1893";

static const char station_hilversum[] PROGMEM = "Hilversum";
static const char msg_hilversum[] PROGMEM = "START IN CONFERENCE ROOM";
static const char station_edinburgh[] PROGMEM = "Edinburgh";
static const char msg_edinburgh[] PROGMEM = "GREEN BOX HAS TOOLS";
static const char station_luxembourg[] PROGMEM = "Luxembourg";
static const char msg_luxembourg[] PROGMEM = "PASSWORD IS EAGLE";
static const char station_dublin[] PROGMEM = "Dublin";
static const char msg_dublin[] PROGMEM = "CHECK KITCHEN DRAWER";
static const char station_london[] PROGMEM = "London";
static const char msg_london[] PROGMEM = "FOLLOW THE MAP";
static const char station_vienna[] PROGMEM = "Vienna";
static const char msg_vienna[] PROGMEM = "NEXT CLUE IN VAULT";
static const char station_berlin[] PROGMEM = "Berlin";
static const char msg_berlin[] PROGMEM = "COMBINATION 8675";
static const char station_rome[] PROGMEM = "Rome";
static const char msg_rome[] PROGMEM = "LOOK BEHIND CLOCK";
static const char station_paris[] PROGMEM = "Paris";
static const char msg_paris[] PROGMEM = "FINAL ROOM KEY";

static const char station_canberra[] PROGMEM = "Canberra";
static const char msg_canberra[] PROGMEM = "BEGIN AT RECEPTION";
static const char station_jerusalem[] PROGMEM = "Jerusalem";
static const char msg_jerusalem[] PROGMEM = "CHECK THE SAFE";
static const char station_bangkok[] PROGMEM = "Bangkok";
static const char msg_bangkok[] PROGMEM = "WHITE CABINET TOP";
static const char station_havana[] PROGMEM = "Havana";
static const char msg_havana[] PROGMEM = "USB KEY IN DRAWER";
static const char station_beijing[] PROGMEM = "Beijing";
static const char msg_beijing[] PROGMEM = "FOLLOW RED MARKS";
static const char station_capetown[] PROGMEM = "Cape Town";
static const char msg_capetown[] PROGMEM = "NEXT CODE 9312";
static const char station_toronto[] PROGMEM = "Toronto";
static const char msg_toronto[] PROGMEM = "CHECK OLD FILES";
static const char station_washington[] PROGMEM = "Washington";
static const char msg_washington[] PROGMEM = "LAST DOOR LEFT";
static const char station_moscow[] PROGMEM = "Moscow";
static const char msg_moscow[] PROGMEM = "VICTORY ACHIEVED";

namespace StationDefaults {
// Station data stored in PROGMEM to save RAM
const StationDefault STATIONS[] PROGMEM = {
    // Long Wave
    {station_athens, 308, WaveBand::LONG_WAVE, msg_athens},
    {station_budapest, 1024, WaveBand::LONG_WAVE, msg_budapest},
    {station_warsaw, 1433, WaveBand::LONG_WAVE, msg_warsaw},
    {station_prague, 2048, WaveBand::LONG_WAVE, msg_prague},
    {station_cairo, 2252, WaveBand::LONG_WAVE, msg_cairo},
    {station_monaco, 2457, WaveBand::LONG_WAVE, msg_monaco},
    {station_oslo, 2970, WaveBand::LONG_WAVE, msg_oslo},
    {station_ankara, 3276, WaveBand::LONG_WAVE, msg_ankara},
    {station_tunis, 3993, WaveBand::LONG_WAVE, msg_tunis},

    // Medium Wave
    {station_hilversum, 308, WaveBand::MEDIUM_WAVE, msg_hilversum},
    {station_edinburgh, 922, WaveBand::MEDIUM_WAVE, msg_edinburgh},
    {station_luxembourg, 1024, WaveBand::MEDIUM_WAVE, msg_luxembourg},
    {station_dublin, 2048, WaveBand::MEDIUM_WAVE, msg_dublin},
    {station_london, 2150, WaveBand::MEDIUM_WAVE, msg_london},
    {station_vienna, 2457, WaveBand::MEDIUM_WAVE, msg_vienna},
    {station_berlin, 3072, WaveBand::MEDIUM_WAVE, msg_berlin},
    {station_rome, 3276, WaveBand::MEDIUM_WAVE, msg_rome},
    {station_paris, 3993, WaveBand::MEDIUM_WAVE, msg_paris},

    // Short Wave
    {station_canberra, 308, WaveBand::SHORT_WAVE, msg_canberra},
    {station_jerusalem, 922, WaveBand::SHORT_WAVE, msg_jerusalem},
    {station_bangkok, 1331, WaveBand::SHORT_WAVE, msg_bangkok},
    {station_havana, 2048, WaveBand::SHORT_WAVE, msg_havana},
    {station_beijing, 2355, WaveBand::SHORT_WAVE, msg_beijing},
    {station_capetown, 2560, WaveBand::SHORT_WAVE, msg_capetown},
    {station_toronto, 2765, WaveBand::SHORT_WAVE, msg_toronto},
    {station_washington, 3481, WaveBand::SHORT_WAVE, msg_washington},
    {station_moscow, 3993, WaveBand::SHORT_WAVE, msg_moscow}};

const size_t STATION_COUNT = sizeof(STATIONS) / sizeof(STATIONS[0]);
}  // namespace StationDefaults

#endif
