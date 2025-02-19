#ifndef LIGHTSMANAGER_H
#define LIGHTSMANAGER_H

#include <Preferences.h>

class LightsManager
{
public:
    LightsManager();

    void begin();
    void changeState(uint8_t state);
    uint8_t getState();

private:
    uint8_t state;
    Preferences pref;
};

#endif // LIGHTSMANAGER_H