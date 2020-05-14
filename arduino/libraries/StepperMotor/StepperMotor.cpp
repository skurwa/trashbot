#include "Arduino.h"
#include "StepperMotor.h"

// constructor
StepperMotor::StepperMotor(int enablePin, int stepPin, int dirPin, int dm0Pin, int dm1Pin, int dm2Pin, int faultPin) {
	// assign pin settings
	pinMode(enablePin, OUTPUT);
	pinMode(stepPin, OUTPUT);
	pinMode(dirPin, OUTPUT);
	pinMode(dm0Pin, OUTPUT);
	pinMode(dm1Pin, OUTPUT);
	pinMode(dm2Pin, OUTPUT);
	pinMode(resetPin, OUTPUT);
	pinMode(faultPin, INPUT);

	// initialize private values
	this->_enablePin	= enablePin;
	this->_stepPin		= stepPin;
	this->_dirPin 		= dirPin;
	this->_alternator 	= true;
	this->_lastMicros 	= 0;
	this->_curMoveSteps	= 0;

	// initialize public values
	this->curSpeed 	= 0;
	this->curPose = 0;

	// initialize status bits
	this->stsOn 		= false;
	this->stsRunning 	= false;
	this->stsIdle 		= false;

	digitalWrite(enablePin, LOW);
	digitalWrite(stepPin, HIGH);

}

// enable the motor
void StepperMotor::cmdOn() {
	digitalWrite(this->_enablePin, HIGH);

	this->stsOn				= true;
	this->stsIdle 			= true;

	this->cmdReset();
}

// disable the stepper
void StepperMotor::cmdOff() {
	digitalWrite(_enablePin, LOW);

	this->stsOn = false;
	this->stsIdle = false;

	this->cmdReset();
}

bool StepperMotor::cmdRun(int dir, int accel, int speed, int targetSteps) {
	if (!this->stsRunning) {
		// reset before running new rutargetStepsnning command
		this->cmdReset();
		// assign motor direction
		digitalWrite(_dirPin, dir);
	}

	unsigned long lastMicros = 0;
	bool alternator = true;
	
	// only run if there are steps left to run
	if (this->_curMoveSteps < targetSteps) {
	
		// send a step
		if (this->_alternator) {
			// if 10 microseconds have elapsed
			if (((unsigned long) (micros() - this->_lastMicros)) > 10) {
				// send high step
				digitalWrite(this->_stepPin, HIGH);

				// update last microsecond count
				this->_lastMicros = micros();

				// alternate
				this->_alternator = !this->_alternator;
			}
		}
		else {
			// if variable time (depending on speed) has elapsed
			if (((unsigned long) (micros() - this->_lastMicros)) > (1000000 / this->curSpeed)) {
				// send step step
				digitalWrite(this->_stepPin, LOW);

				// update last microsecond count
				this->_lastMicros = micros();

				// update motion
				updateMotion(accel, speed, targetSteps);

				// alternate
				this->_alternator = !this->_alternator;
				
				// increment step count
				this->_curMoveSteps++;
			}
		}

		// update status flags
		this->stsIdle    		= false;
		this->stsRunning 		= true;
		this->stsMoveComplete 	= false;

		// exit function while running
		return true;
	}

	// update status flags
	this->stsRunning 		= false;
	this->stsIdle 			= true;
	this->stsMoveComplete 	= true;

	// exit function after finishing run
	return false;
}

// stop the motor if running
bool StepperMotor::cmdStop() {
	if (this->stsRunning) {
		this->stsRunning 	= false;
		this->stsMoveFailed = true;
		return true;
	}
	return false;
}

// reset motor values
void StepperMotor::cmdReset() {
	// reset accumulators
	this->_curMoveSteps = 0;
	this->curSpeed  = 0;
	this->_lastMicros   = 0;

	// reset status bits
	this->stsMoveComplete	= false;
	this->stsMoveFailed	  	= false;
	this->stsRunning	  	= false;
}

void StepperMotor::cmdHome(int dir, int home) {
	
}

void StepperMotor::cmdMove(int accel, int speed, int targetPosition) {
	int move = curPose - targetPosition;
	if 
}

// helper function to recalculate motion during move
void StepperMotor::updateMotion(int accel, int speed, int targetSteps) {
	// quick calcs
	int stepsToTargetSpeed = speed / accel;
	int halfPath            = targetSteps / 2;

	// triangular motion profile; cannot reach target speed
	if (stepsToTargetSpeed > halfPath) {
		// accelerate
		if (this->_curMoveSteps <= halfPath) {
			this->curSpeed = this->curSpeed + accel;
		}
		// decelerate
		else if (this->_curMoveSteps > halfPath) {
			this->curSpeed = this->curSpeed - accel;
		}
	}
	// trapezoidal motion profile or triangular motion profile with momentary target speed
	else if (stepsToTargetSpeed <= halfPath) {
		// accelerate
		if (this->_curMoveSteps < stepsToTargetSpeed) {
			this->curSpeed = this->curSpeed + accel;
		}
		// decelerate
		else if (this->_curMoveSteps > (targetSteps - stepsToTargetSpeed)) {
			this->curSpeed = this->curSpeed - accel;
		}
	}
}