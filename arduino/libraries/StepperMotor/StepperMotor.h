#ifndef StepperMotor_h
#define StepperMotor_h

#include "Arduino.h"

class StepperMotor {
	private:
		int  _enablePin;
		int  _pulsePin;
		int  _dirPin;
		bool _alternator;
		unsigned long _lastMicros;

	public:
		// status bits
		bool stsOn;
		bool stsRunning;
		bool stsIdle;
		bool stsMoveComplete;
		bool stsMoveFailed;

		int	 currentSpeed;
		int  currentPulses;

		StepperMotor(int enablePin, int pulsePin, int dirPin);
		void cmdOn();
		void cmdOff();
		bool cmdRun(int dir, int accel, int speed, int targetPulses);
		bool cmdStop();
		void cmdReset();
		void updateMotion(int accel, int speed, int targetPulses);
};

#endif