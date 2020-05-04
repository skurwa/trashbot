#include "Arduino.h"
#include "StepperMotor.h"

// constructor
StepperMotor::StepperMotor(int enablePin, int pulsePin, int dirPin) {
	// assign pin settings
	pinMode(enablePin, OUTPUT);
	pinMode(pulsePin, OUTPUT);
	pinMode(dirPin, OUTPUT);

	// initialize private values
	this->_enablePin	= enablePin;
	this->_pulsePin		= pulsePin;
	this->_dirPin 		= dirPin;
	this->_alternator 	= true;
	this->_lastMicros 	= 0;

	// initializ public values
	this->currentPulses	= 0;
	this->currentSpeed 	= 0;

	// initialize status bits
	this->stsOn 		= false;
	this->stsRunning 	= false;
	this->stsIdle 		= false;

	digitalWrite(enablePin, LOW);
	digitalWrite(pulsePin, HIGH);

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

bool StepperMotor::cmdRun(int dir, int accel, int speed, int targetPulses) {
	if (!this->stsRunning) {
		// reset before running new running command
		this->cmdReset();
		// assign motor direction
		digitalWrite(_dirPin, dir);
	}

	unsigned long lastMicros = 0;
	bool alternator = true;
	
	// only run if there are pulses left to run
	if (this->currentPulses < targetPulses) {
	
		// send a pulse
		if (this->_alternator) {
			// if 10 microseconds have elapsed
			if (((unsigned long) (micros() - this->_lastMicros)) > 10) {
				// send high pulse
				digitalWrite(this->_pulsePin, HIGH);

				// update last microsecond count
				this->_lastMicros = micros();

				// alternate
				this->_alternator = !this->_alternator;
			}
		}
		else {
			// if variable time (depending on speed) has elapsed
			if (((unsigned long) (micros() - this->_lastMicros)) > (1000000 / this->currentSpeed)) {
				// send step pulse
				digitalWrite(this->_pulsePin, LOW);

				// update last microsecond count
				this->_lastMicros = micros();

				// update motion
				updateMotion(accel, speed, targetPulses);

				// alternate
				this->_alternator = !this->_alternator;
				
				// increment pulse count
				this->currentPulses++;
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
	this->currentPulses = 0;
	this->currentSpeed  = 0;
	this->_lastMicros   = 0;

	// reset status bits
	this->stsMoveComplete	= false;
	this->stsMoveFailed	  	= false;
	this->stsRunning	  	= false;
}

// helper function to recalculate motion during move
void StepperMotor::updateMotion(int accel, int speed, int targetPulses) {
	// quick calcs
	int pulsesToTargetSpeed = speed / accel;
	int halfPath            = targetPulses / 2;

	// triangular motion profile; cannot reach target speed
	if (pulsesToTargetSpeed > halfPath) {
		// accelerate
		if (this->currentPulses <= halfPath) {
			this->currentSpeed = this->currentSpeed + accel;
		}
		// decelerate
		else if (this->currentPulses > halfPath) {
			this->currentSpeed = this->currentSpeed - accel;
		}
	}
	// trapezoidal motion profile or triangular motion profile with momentary target speed
	else if (pulsesToTargetSpeed <= halfPath) {
		// accelerate
		if (this->currentPulses < pulsesToTargetSpeed) {
			this->currentSpeed = this->currentSpeed + accel;
		}
		// decelerate
		else if (this->currentPulses > (targetPulses - pulsesToTargetSpeed)) {
			this->currentSpeed = this->currentSpeed - accel;
		}
	}
}