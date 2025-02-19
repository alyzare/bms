#include "LightsManager.h"

LightsManager::LightsManager() {}

void LightsManager::begin()
{
    pref.begin("lights", false);
    state = pref.getUChar("state", 0);
}

void LightsManager::changeState(uint8_t state)
{
    this->state = state;
    pref.putUChar("state", state);
}

uint8_t LightsManager::getState()
{
    return state;
}