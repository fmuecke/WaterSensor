#ifndef PRESSURE_SENSOR__HPP_INCLUDED
#define PRESSURE_SENSOR__HPP_INCLUDED

// see: https://wiki.dfrobot.com/Gravity__Water_Pressure_Sensor_SKU__SEN0257
/************************************************************
  Water Pressure Sensor Calibration

  The output voltage offset of the sensor is 0.5V (norminal).
  However, due to the zero-drifting of the internal circuit, the
  no-load output voltage is not exactly 0.5V. Calibration needs to
  be carried out as follow.

  Calibration: connect the 3 pin wire to the Arduio UNO (VCC, GND and Signal)
  without connecting the sensor to the water pipe and run the program
  for once. Mark down the LOWEST voltage value through the serial
  monitor and revise the "OffSet" value to complete the calibration.

  After the calibration the sensor is ready for measuring!
**************************************************************/

// OFFSET: calibrate: use LOWEST voltage value determined in a dry run
#define VOLTAGE_OFFSET 0.405273f

template <int PIN>
struct PressureSensor
{
  void Init() 
  {}

  float GetMaxValue() const { return _maxVal; }

  float GetValue()
  { 
    auto outputVoltage = analogRead(PIN) * 5.00 / 1024;  // Sensor output voltage
    _lowestVoltage = min(outputVoltage, _lowestVoltage);
    auto result = (outputVoltage - VOLTAGE_OFFSET) * 4;  // Calculate water pressure in mBar
    _maxVal = max(result,  _maxVal);
    return result < 0 ? 0 : result;
  }

private:
  float _lowestVoltage = 10;
  float _maxVal = 0;
};

#endif