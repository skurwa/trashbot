#include "Arduino.h"
#include "Stepper.h"


void Stepper::Stepper(int enablePin, int pulsePin, int dirPin) {
	pinMode(enablePin, OUTPUT);
	pinMode(pulsePin, OUTPUT);
	pinMode(dirPin, OUTPUT);
		
	_enablePin 		= enablePin;
	_pulsePin		= pulsePin;
	_dirPin 		= dirPin;
	currentPulses	= 0;
	currentSpeed 	= 0;
	currentAccel	= 0;
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
	_pulses = 0;
	_currentSpeed = 0;
	_stop = 0;

	this->_pcmdRun(accel, speed, pulses);

}

void Stepper::_pcmdRun(int accel, int speed, long pulses) {
	int currentSpeed = 0;
	long pulsesToTargetSpeed = speed / accel;
	while (currentPulses < pulses) {
		// triangular motion profile; cannot reach target speed
		if (pulsesToTargetSpeed > pulses / 2) {
			if (currentPulses <= pulses / 2) {
				currentSpeed = currentSpeed + accel*(pulses/2 - currentPulses);
			}
			else if (currentPulses > pulses / 2) {
				currentSpeed = currentSpeed - accel*(pulses/2 - currentPulses);
			}
		}
		// trapezoidal motion profile or triangular motion profile with momentary target speed
		else if (pulsesToTargetSpeed <= pulses / 2) {
			if (currentPulses < pulsesToTargetSpeed) {
				currentSpeed = currentSpeed + (currentPulses * accel);
			}
			else if (currentPulses > (pulses - pulsesToTargetSpeed)) {
				currentSpeed = currentSpeed - accel*(currentPulses - (pulses - pulsesToTargetSpeed));
			}

		}

		digitalWrite(_pulsePin, HIGH);
		delayMicroseconds(50);
		digitalWrite(_pulsePin, LOW);
		delayMicroseconds((1/speed) - 50)
		currentPulses++;
	}

}