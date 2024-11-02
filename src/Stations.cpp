#include "Stations.h"

// Define the stations with their frequencies, bands, and default Morse code messages
Station stations[] = {
    // Long Wave
    {"Athens", 300, LONG_WAVE, "A"},
    {"Budapest", 1000, LONG_WAVE, "B"},
    {"Warsaw", 1400, LONG_WAVE, "W"},
    {"Prague", 2000, LONG_WAVE, "P"},
    {"Cairo", 2200, LONG_WAVE, "C"},
    {"Monaco", 2400, LONG_WAVE, "M"},
    {"Oslo", 2900, LONG_WAVE, "O"},
    {"Ankara", 3200, LONG_WAVE, "AN"},
    {"Kiev", 3900, LONG_WAVE, "K"},

    // Medium Wave
    {"Hilversum", 300, MEDIUM_WAVE, "H"},
    {"Edinburgh", 900, MEDIUM_WAVE, "ED"},
    {"Luxembourg", 1000, MEDIUM_WAVE, "L"},
    {"Dublin", 2000, MEDIUM_WAVE, "D"},
    {"London", 2100, MEDIUM_WAVE, "LO"},
    {"Vienna", 2400, MEDIUM_WAVE, "V"},
    {"Berlin", 3000, MEDIUM_WAVE, "BE"},
    {"Rome", 3200, MEDIUM_WAVE, "R"},
    {"Paris", 3900, MEDIUM_WAVE, "P"},

    // Short Wave
    {"Canberra", 300, SHORT_WAVE, "CA"},
    {"Jerusalem", 900, SHORT_WAVE, "JE"},
    {"Bangkok", 1300, SHORT_WAVE, "B"},
    {"Havana", 2000, SHORT_WAVE, "H"},
    {"Beijing", 2300, SHORT_WAVE, "BJ"},
    {"Cape Town", 2500, SHORT_WAVE, "CT"},
    {"Toronto", 2700, SHORT_WAVE, "T"},
    {"Washington", 3400, SHORT_WAVE, "W"},
    {"Moscow", 3900, SHORT_WAVE, "M"}};

const int numStations = sizeof(stations) / sizeof(stations[0]);
