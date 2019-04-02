// Settings.h

#ifndef _SETTINGS_h
#define _SETTINGS_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <EEPROM.h>

struct Config
{
    unsigned int WasherTimeout = 47;
    unsigned int WaterTimeout = 7;
    unsigned int FeederTimeout = 4;
    char Monitor[15] = "09234875443";
    bool asdf = true;
    unsigned int checksum;
};

class SettingsClass
{
private:


public:
    void SaveConfig();
    void ResetConfig();
    void LoadConfig();

    Config Current;
};

extern SettingsClass Settings;
#endif