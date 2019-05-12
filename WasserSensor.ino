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
  void Init(int pin)
  { 
    _pin = pin;
    pinMode(_pin, OUTPUT);
  }

  void Toggle()
  {
    if (IsOn()) Off();
    else On();
  }
  void On() { digitalWrite(_pin, HIGH); _isOn = true; }
  void Off() { digitalWrite(_pin, LOW); _isOn = false; }
  bool IsOn() const { return _isOn; }
  
private:  
  int _pin {10};  // default pin
  bool _isOn { false };
};

struct FlowSensor 
{
  void Init(int pin) 
  { 
    _pin = pin;
    pinMode(_pin, INPUT);
  }

  double Measure(int milisecs)
  {
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
    _maxFlow = max(flowRate, _maxFlow);

    return flowRate;
  }

  double GetMaxFlow() const { return _maxFlow; }

  // values are in milliseconds
  unsigned long GetAveragePulse() const { return _totalDuration / _totalPulses / 1000; }
  unsigned long GetMaxPulse() const { return _maxPulse; }
  unsigned long GetMinPulse() const { return _minPulse; }

private:  
  int _pin { 2 };
  unsigned int _totalPulses { 0 };
  unsigned long _totalDuration { 0 };
  unsigned long _avgPulse { 0 };
  unsigned long _maxPulse { 0 };
  unsigned long _minPulse { 100000 };
  double _maxFlow { 0 };
};

// global vars
Lcd lcd;
Relais relais;
FlowSensor flowSensor;

const int pressure_pin = 0;
unsigned int loopCount = 0;
const float pressure_offset = 0.42;  // calibrate: use LOWEST voltage value determined in a dry run
float lowestPressureVoltage = 10;
float maxPressure = 0;

float getPressure()
{ 
  auto outputVoltage = analogRead(pressure_pin) * 5.00 / 1024;  // Sensor output voltage
  lowestPressureVoltage = min(outputVoltage, lowestPressureVoltage);
  auto result = (outputVoltage - pressure_offset) * 4;  // Calculate water pressure in mBar
  return result < 0 ? 0 : result;
}

void setup()
{
  lcd.Init();
  relais.Init(10);
  flowSensor.Init(2); 
}

void loop()
{
  loopCount++;

  auto flowRate = flowSensor.Measure(1000);
  lcd.Clear();

  auto pressure = getPressure();
  maxPressure = max(pressure, maxPressure);
  String s = String(pressure, 1);
  s += " Bar (";
  s += String(maxPressure, 1);
  s += ")";
  lcd.DrawLine(0, s.c_str());

  s = String(flowRate, 1);
  s += " L/min (";
  s += String(flowSensor.GetMaxFlow(), 1);
  s += ")";
  lcd.DrawLine(1, s.c_str());
  
  s = " ";
  s += String(flowSensor.GetMinPulse());
  s += "ms..";
  s += String(flowSensor.GetMaxPulse());
  s += "ms";
  lcd.DrawLine(2, s.c_str());
  
  s = " avg: ";
  s += String(flowSensor.GetAveragePulse());
  s += "ms";
  lcd.DrawLine(3, s.c_str());
  
  s = String(lowestPressureVoltage,6);
  s += " - ";
  s += loopCount;
  lcd.DrawLine(4, s.c_str());

  //relais.Toggle();
  
  s = "relais is: ";
  s += relais.IsOn();
  lcd.DrawLine(5, s.c_str());
  
  lcd.Flush(); 
}
