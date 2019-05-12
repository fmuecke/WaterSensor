#include "lcd.hpp"
#include "FlowSensor.hpp"
#include "PressureSensor.hpp"
#include "Relais.hpp"

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
  pressureSensor.Init();
  startTime = millis() / 1000;

  // turn off internal LED
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
}

void loop()
{
  auto flowRate = flowSensor.Measure(500);
  lcd.Clear();

  // e.g. "2.0 Bar (2.2)"
  auto pressure = pressureSensor.GetValue();
  String s = String(pressure, 1);
  s += " Bar (";
  s += String(pressureSensor.GetMaxValue(), 1);
  s += ")";
  lcd.DrawLine(0, s.c_str());

  // e.g. "10.3 L/min (11.4)"
  s = String(flowRate, 1);
  s += " L/min (";
  s += String(flowSensor.GetMaxFlow(), 1);
  s += ")";
  lcd.DrawLine(1, s.c_str());
  
  if (!relais.IsOn() && pressureSensor.GetValue() >= 1.2f)  { relais.SwitchOn(); }
  if (relais.IsOn() && pressureSensor.GetValue() <= 0.6f) { relais.SwitchOff(); }

  // e.g. "OPEN since 6m 12s"
  s = relais.IsOn() ? "OPEN " : "CLOSED ";
  s += DurationString(relais.GetStateDuration());
  s += " (";
  s += DurationString(relais.GetLastOpenDuration());
  s += ")";
  lcd.DrawLine(3, s.c_str());

  s = "elapsed: ";
  s += DurationString(millis()/1000 - startTime);
  lcd.DrawLine(4, s.c_str());
  
  lcd.Flush(); 
}
