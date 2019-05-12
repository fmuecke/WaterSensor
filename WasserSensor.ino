// see http://www.benripley.com/diy/arduino/three-ways-to-read-a-pwm-signal-with-arduino/
// and https://www.bc-robotics.com/tutorials/using-a-flow-sensor-with-arduino/
// pressure sensor: https://wiki.dfrobot.com/Gravity__Water_Pressure_Sensor_SKU__SEN0257
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

#include "lcd.hpp"

struct Relais
{
  void Init(int pin) { 
    _pin = pin;
    pinMode(_pin, OUTPUT);
  }

  void On() { digitalWrite(_pin, HIGH); }
  void Off() { digitalWrite(_pin, LOW); }
  
  int _pin {10};  // default pin
};

struct FlowSensor
{
  void Init(int pin) { 
    _pin = pin;
    pinMode(_pin, INPUT);
  }

  double Measure(int milisecs) {
    int pulses = 0;
    const auto startVal = millis();
    auto now = startVal;
    for (; now - startVal < milisecs; now = millis())
    {
      auto duration = pulseIn(_pin, HIGH);
      _minPulse = min(duration, _minPulse)/1000;
      _maxPulse = max(duration, _maxPulse)/1000;
  
      if (duration > 0) 
      {
        _totalDuration += duration;
        ++_totalPulses;
        ++pulses;
      }
    }
   
    double flowRate = (pulses * 2.25);        // Take counted pulses in the last second and multiply by 2.25mL (see specs)
    flowRate = flowRate * 60;                // Convert seconds to minutes, giving you mL / Minute
    flowRate = flowRate / (now - startVal);  // Convert mL to Liters, giving you Liters / Minute
    return flowRate;
  }

  // values are in milliseconds
  unsigned long GetAveragePulse() const { return _totalDuration / _totalPulses; }
  unsigned long GetMaxPulse() const { return _maxPulse; }
  unsigned long GetMinPulse() const { return _minPulse; }

private:  
  int _pin { 2 };
  unsigned int _totalPulses { 0 };
  unsigned long _totalDuration { 0 };
  unsigned long _avgPulse { 0 };
  unsigned long _maxPulse { 0 };
  unsigned long _minPulse { 100000 };
};

// global vars
Lcd lcd;
Relais relais;
FlowSensor flowSensor;

//const int sensor_pin = 2; 
const int pressure_pin = 0;
//unsigned int pulse = 0; // volatile?
unsigned int loopCount = 0;
//double minVal = 100000;
//double maxVal = 0;
//double sumVal = 0;
//unsigned int allPulses = 0;
const float pressure_offset = 0.42;  // calibrate: use LOWEST voltage value determined in a dry run
float lowestPressureVoltage = 10;

float getPressure() { 
  auto outputVoltage = analogRead(pressure_pin) * 5.00 / 1024;  // Sensor output voltage
  lowestPressureVoltage = min(outputVoltage, lowestPressureVoltage);
  auto result = (outputVoltage - pressure_offset) * 400;  // Calculate water pressure in mBar
  return result < 0 ? 0 : result;
}

void setup() {
  lcd.Init();
  relais.Init(10);
  flowSensor.Init(2); 
}

void loop() {
  //int pulse = 0;
  loopCount++;

  auto flowRate = flowSensor.Measure(1000);
  lcd.Clear();

  String s = String(getPressure(),1);
  s += " Bar ";
  s += String(flowRate, 1);
  s += " L/min  ";
  lcd.DrawLine(0, s.c_str());

  s = "min: "; 
  s += String(flowSensor.GetMinPulse());
  s += "ms";
  lcd.DrawLine(1, s.c_str());
  
  s = "max: ";
  s += String(flowSensor.GetMaxPulse());
  s += "ms";
  lcd.DrawLine(2, s.c_str());
  
  s = "avg: ";
  s += String(flowSensor.GetAveragePulse());
  s += "ms";
  lcd.DrawLine(3, s.c_str());
  
  s = String(lowestPressureVoltage,6);
  s += " - ";
  s += loopCount;
  lcd.DrawLine(4, s.c_str());
  
  lcd.Flush(); 
}
