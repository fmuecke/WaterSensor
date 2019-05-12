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

template <int PIN>
struct Relais
{
  void Init()
  { 
    pinMode(PIN, OUTPUT);
    _toggleTime = millis();
  }

  void Toggle()
  {
    if (IsOn()) SwitchOff();
    else SwitchOn();
  }
  void SwitchOn() { digitalWrite(PIN, HIGH); _isOn = true; _toggleTime = millis(); }
  void SwitchOff() { digitalWrite(PIN, LOW); _isOn = false; _toggleTime = millis(); }
  bool IsOn() const { return _isOn; }
  
  unsigned long GetStateDuration() const 
  {
    return (millis() - _toggleTime) / 1000;
  }
  
private:  
  bool _isOn { false };
  unsigned long _toggleTime { 0 };
};

template<int PIN>
struct FlowSensor 
{
  void Init() 
  { 
    pinMode(PIN, INPUT);
  }

  double Measure(int milisecs)
  {
    int pulses = 0;
    const auto startVal = millis();
    auto now = startVal;
    for (; now - startVal < milisecs; now = millis())
    {
      unsigned long duration = pulseIn(PIN, HIGH);
      if (duration > 0)
      {
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

private:  
  double _maxFlow { 0 };
};

template <int PIN>
struct PressureSensor
{
  void Init() 
  { 
  }

  float GetMaxValue() const { return _maxVal; }

  float GetValue()
  { 
    auto outputVoltage = analogRead(PIN) * 5.00 / 1024;  // Sensor output voltage
    _lowestVoltage = min(outputVoltage, _lowestVoltage);
    auto result = (outputVoltage - _offset) * 4;  // Calculate water pressure in mBar
    _maxVal = max(result,  _maxVal);
    return result < 0 ? 0 : result;
  }


private:
  const float _offset = 0.405273;  // calibrate: use LOWEST voltage value determined in a dry run
  float _lowestVoltage = 10;
  float _maxVal = 0;
};

// global vars
Lcd lcd;
Relais<10> relais;
FlowSensor<2> flowSensor;
PressureSensor<0> pressureSensor;

unsigned long startTime = 0;

String DurationString(unsigned long totalSecs)
{
  String s = String(totalSecs / 60);
  s += "m ";
  s += totalSecs % 60;
  s += "s";
  
  return s;
}

void setup()
{
  lcd.Init();
  relais.Init();
  flowSensor.Init();
  //pressureSensor.Init(0);
  startTime = millis() / 1000;
}

void loop()
{
  auto flowRate = flowSensor.Measure(500);
  lcd.Clear();

  auto pressure = pressureSensor.GetValue();
  String s = String(pressure, 1);
  s += " (";
  s += String(pressureSensor.GetMaxValue(), 1);
  s += ") Bar";
  lcd.DrawLine(0, s.c_str());

  s = String(flowRate, 1);
  s += " (";
  s += String(flowSensor.GetMaxFlow(), 1);
  s += ") L/min";
  lcd.DrawLine(1, s.c_str());
  
#ifdef _DEBUG
  s = " ";
  s = String(flowSensor.GetMinPulse());
  s += "ms..";
  s += String(flowSensor.GetAveragePulse());
  s += "ms..";
  s += String(flowSensor.GetMaxPulse());
  s += "ms";
  lcd.DrawLine(2, s.c_str());
  
  s = String(lowestPressureVoltage,6);
  s += " - ";
  s += loopCount;
  lcd.DrawLine(4, s.c_str());
#endif

  //relais.Toggle();
  if (!relais.IsOn() && pressureSensor.GetValue() > 1.0f)  { relais.SwitchOn(); }
  if (relais.IsOn() && pressureSensor.GetValue() <= 0.25f) { relais.SwitchOff(); }
  
  s = relais.IsOn() ? "ON since " : "OFF since ";
  s += DurationString(relais.GetStateDuration());
  lcd.DrawLine(3, s.c_str());

  s = "total: ";
  s += DurationString(millis()/1000 - startTime);
  lcd.DrawLine(4, s.c_str());
  
  lcd.Flush(); 
}
