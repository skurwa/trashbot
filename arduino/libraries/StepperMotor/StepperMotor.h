#ifndef StepperMotor_h
#define StepperMotor_h

#include "Arduino.h"

class StepperMotor {
	private:
		int  _enablePin;
		int  _stepPin;
		int  _dirPin;
		bool _alternator;
		int  _curMoveSteps;
		unsigned long _lastMicros;

	public:
		// status bits
		bool stsOn;
		bool stsRunning;
		bool stsIdle;
		bool stsMoveComplete;
		bool stsMoveFailed;
		bool stsHomeRefOK;

		int	curSpeed;
		int	curPose;

		StepperMotor(int enablePin, int stepPin, int dirPin);
		void cmdOn();
		void cmdOff();
		bool cmdRun(int dir, int accel, int speed, int targetSteps);
		bool cmdStop();
		void cmdReset();
		void cmdHome();
		void cmdMove(int accel, int speed, int targetPosition)
		void updateMotion(int accel, int speed, int targetSteps);
		void angToPul(int steps);
		void pulToAng(int ang);
};

#endif