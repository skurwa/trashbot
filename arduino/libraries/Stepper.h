#ifndef Stepper_h
#define Stepper_h

#include "Arduino.h"

class Stepper {
  public:
    Stepper(int enablePin, int pulsePin, int dirPin);
    void pcmdOn();
	void pcmdOff();
	void pcmdStart(int dir, int accel, int speed, long pulses);

	long currentPulses;
	int currentSpeed;
  
  private:
    int  _enablePin;
	int  _pulsePin;
	int  _dirPin;

	void _pcmdRun();

};

#endif