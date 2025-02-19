#include "SensorUtils.h"

float calculateGasPercentage(uint16_t sensorValue)
{
    
  // Constants based on assumed experimental data
  const int cleanAirValue = 200;   // Sensor value in clean air
  const int maxValue = 3500;       // Sensor value at 100% gas concentration

  // Ensure the sensor value is within the valid range
  if (sensorValue < cleanAirValue) {
    sensorValue = cleanAirValue;  // Clamp to minimum value
  } else if (sensorValue > maxValue) {
    sensorValue = maxValue;       // Clamp to maximum value
  }

  // Calculate gas percentage
  float gasPercentage = ((float)(sensorValue - cleanAirValue) / (maxValue - cleanAirValue)) * 100.0;

  return gasPercentage;
}

float calculateLightPercentage(int ldrValue) {
  float percentage = (ldrValue / 4095.0) * 100.0;
  return percentage;
}
