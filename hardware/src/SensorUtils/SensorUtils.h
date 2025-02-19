#ifndef SENSORUTILS_H
#define SENSORUTILS_H

#include <Arduino.h>

float calculateGasPercentage(uint16_t sensorValue);
float calculateLightPercentage(int ldrValue);

#endif