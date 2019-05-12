#ifndef FLOW_SENSOR__HPP_INCLUDED
#define FLOW_SENSOR__HPP_INCLUDED
// see http://www.benripley.com/diy/arduino/three-ways-to-read-a-pwm-signal-with-arduino/
// and https://www.bc-robotics.com/tutorials/using-a-flow-sensor-with-arduino/

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
   
    double flowRate = (pulses * 2.25);       // Take counted pulses in the last second and multiply by 2.25mL (see specs)
    flowRate = flowRate * 60;                // Convert seconds to minutes, giving you mL / Minute
    flowRate = flowRate / (now - startVal);  // Convert mL to Liters, giving you Liters / Minute
    _maxFlow = max(flowRate, _maxFlow);

    return flowRate;
  }

  double GetMaxFlow() const { return _maxFlow; }

private:  
  double _maxFlow { 0 };
};

#endif