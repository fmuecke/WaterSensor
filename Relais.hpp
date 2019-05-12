#ifndef RELAIS__HPP_INCLUDED
#define RELAIS__HPP_INCLUDED

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
  void SwitchOff() 
  { 
	digitalWrite(PIN, LOW); 
	_isOn = false; 
	_lastOpenDuration = GetStateDuration();
	_toggleTime = millis(); 
  }
  
  bool IsOn() const { return _isOn; }
  unsigned long GetStateDuration() const { return (millis() - _toggleTime) / 1000; }
  unsigned long GetLastOpenDuration() const { return _lastOpenDuration; }
  
private:  
  bool _isOn { false };
  unsigned long _toggleTime { 0 };
  unsigned long _lastOpenDuration { 0 };
};

#endif