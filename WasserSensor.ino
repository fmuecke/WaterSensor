#define DIAG

#ifdef DIAG
  #include <SPI.h>
  #include <SD.h>
#else
  #include "OledDisplay.hpp"
#endif

#include "FlowSensor.hpp"
#include "PressureSensor.hpp"
#include "Relais.hpp"

#ifdef DIAG
  File logFile;
  bool sdLoaded = false;
#else 
  OledDisplay oled;
#endif

Relais<10> relais;
FlowSensor<2> flowSensor;
PressureSensor<0> pressureSensor;

unsigned long startTick = 0;
unsigned long lastWriteTick = 0;

String DurationString(unsigned long totalSecs)
{
  String s = String(totalSecs / 60);
  s += F("m ");
  s += totalSecs % 60;
  s += F("s");
  return s;
}

void setup()
{
#ifdef DIAG
  sdLoaded = SD.begin(4);
  if (sdLoaded)
  {
    // turn off internal LED
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);
  }
#else
  // turn off internal LED
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  oled.Init();
#endif

  relais.Init();
  flowSensor.Init();
  pressureSensor.Init();
  startTick = millis() / 1000;
}

void loop()
{
  auto currentTick = millis()/1000;
  auto flowRate = flowSensor.Measure(500);

  // e.g. "2.0 Bar (2.2)"
  auto pressure = pressureSensor.GetValue();
  String line0 = String(pressure, 1);
  line0 += F(" Bar (");
  line0 += String(pressureSensor.GetMaxValue(), 1);
  line0 += F(")");

  // e.g. "10.3 L/min (11.4)"
  String line1 = String(flowRate, 1);
  line1 += F(" L/min (");
  line1 += String(flowSensor.GetMaxFlow(), 1);
  line1 += F(")");
  
  if (!relais.IsOn() && (int)pressureSensor.GetValue() > 1)  { relais.SwitchOn(); }
  if (relais.IsOn() && pressureSensor.GetValue() <= 0.6f) { relais.SwitchOff(); }

  // e.g. "OPEN since 6m 12s"
  String line3 = relais.IsOn() ? F("OPEN ") : F("CLOSED ");
  line3 += DurationString(relais.GetStateDuration());
  line3 += F(" (");
  line3 += DurationString(relais.GetLastOpenDuration());
  line3 += F(")");

  String line4 = F("elapsed: ");
  line4 += DurationString(currentTick - startTick);
  

#ifdef DIAG
  if (sdLoaded && currentTick - lastWriteTick > 60)
  {
    lastWriteTick = currentTick;
    logFile = SD.open(F("pumpe.log"), FILE_WRITE);
    logFile.print(currentTick);
    logFile.print(F(";"));
    logFile.print(pressure);
    logFile.print(F(";"));
    logFile.print(flowRate);
    logFile.print(F(";"));
    logFile.println(relais.IsOn()?F("on"):F("off"));
    logFile.close();
  }
#else
  oled.Clear();
  oled.DrawLine(0, line0.c_str());
  oled.DrawLine(1, line1.c_str());
  //oled.DrawLine(2, line2.c_str());
  oled.DrawLine(3, line3.c_str());
  oled.DrawLine(4, line4.c_str());
  oled.Flush(); 
#endif
  
}
