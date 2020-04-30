#include "Arduino.h"
#include "Stepper.h"


Stepper::Stepper(int enablePin, int pulsePin, int dirPin) {
	pinMode(enablePin, OUTPUT);
	pinMode(pulsePin, OUTPUT);
	pinMode(dirPin, OUTPUT);
		
	this->_enablePin	= enablePin;
	this->_pulsePin		= pulsePin;
	this->_dirPin 		= dirPin;
	this->currentPulses	= 0;
	this->currentSpeed 	= 0;
}

void Stepper::pcmdOn() {
	digitalWrite(_enablePin, HIGH);
}

void Stepper::pcmdOff() {
	digitalWrite(_enablePin, LOW);
}

void Stepper::pcmdStart(int dir, int accel, int speed, long pulses) {
	digitalWrite(_dirPin, dir);
	digitalWrite(_enablePin, HIGH);
	
	this->_pulses = 0;
	this->_currentSpeed = 0;

	this->_pcmdRun(accel, speed, pulses);

}

void Stepper::_pcmdRun(int accel, int speed, long pulses) {
	int currentSpeed = 0;
	long pulsesToTargetSpeed = speed / accel;
	while (this->currentPulses < pulses) {
		// triangular motion profile; cannot reach target speed
		if (pulsesToTargetSpeed > pulses / 2) {
			if (this->currentPulses <= this->pulses / 2) {
				this->currentSpeed = this->currentSpeed + accel*(pulses/2 - this->currentPulses);
			}
			else if (this->currentPulses > pulses / 2) {
				this->currentSpeed = this->currentSpeed - accel*(pulses/2 - this->currentPulses);
			}
		}
		// trapezoidal motion profile or triangular motion profile with momentary target speed
		else if (pulsesToTargetSpeed <= pulses / 2) {
			if (this->currentPulses < pulsesToTargetSpeed) {
				this->currentSpeed = this->currentSpeed + (this->currentPulses * accel);
			}
			else if (this->currentPulses > (pulses - pulsesToTargetSpeed)) {
				this->currentSpeed = this->currentSpeed - accel*(this->currentPulses - (pulses - pulsesToTargetSpeed));
			}

		}

		digitalWrite(this->_pulsePin, HIGH);
		delayMicroseconds(10);
		digitalWrite(this->_pulsePin, LOW);
		delayMicroseconds((1/currentSpeed) - 10);
		currentPulses++;
	}

}