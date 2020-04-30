#include <Wire.h>

#define OUT_PAYLOAD_SIZE  5
#define ADDRESS           2
#define COUNTERCLOCKWISE  0
#define CLOCKWISE         1
#define HIGH_TIME_MICROSEC  15

// pin definition
const int motorAEnablePin = 28;
const int motorAStepPin   = 2;
const int motorADirPin    = 3;
const int motorBEnablePin = 29;
const int motorBStepPin   = 4;
const int motorBDirPin    = 5;

// global variables
const int maxFreq       =  (1000000 / HIGH_TIME_MICROSEC);
long heartBeatTime      = 0;
int freqInput           = 0;
int lastFreqInput       = 0;
String freqInputString  = "";
bool stringComplete     = false;


class StepperMotor {
  // class member vars
  private:
    int  _enablePin;
	  int  _pulsePin;
	  int  _dirPin;
    
    bool _alternator;
    unsigned long _lastMicros;

  public:
	  int	 currentSpeed;
	  long currentPulses;

    // constructor
    StepperMotor(int enablePin, int pulsePin, int dirPin) {
      pinMode(enablePin, OUTPUT);
      pinMode(pulsePin, OUTPUT);
      pinMode(dirPin, OUTPUT);
  
      this->_enablePin	= enablePin;
      this->_pulsePin		= pulsePin;
      this->_dirPin 		= dirPin;
      this->_alternator = true;
      this->currentPulses	= 0;
      this->currentSpeed 	= 0;
      this->_lastMicros = 0;
    }

    // enable the stepper 
    void pcmdOn() {
	    digitalWrite(this->_enablePin, HIGH);
    }

    // disable the stepper
    void pcmdOn() {
	    digitalWrite(_enablePin, LOW);
    }

    // run the stepper for a pulse
    bool cmdRun(int accel, int speed, long targetPulses) {
    	long pulsesToTargetSpeed = speed / accel;
      // only run if there are pulses left to run
      if (this->currentPulses < targetPulses) {
        // triangular motion profile; cannot reach target speed
        if (pulsesToTargetSpeed > targetPulses / 2) {
          // accelerate
          if (this->currentPulses <= targetPulses / 2) {
            this->currentSpeed = this->currentSpeed + accel*(targetPulses/2 - this->currentPulses);
          }
          // decelerate
          else if (this->currentPulses > targetPulses / 2) {
            this->currentSpeed = this->currentSpeed - accel*(targetPulses/2 - this->currentPulses);
          }
        }
        // trapezoidal motion profile or triangular motion profile with momentary target speed
        else if (pulsesToTargetSpeed <= targetPulses / 2) {
          // accelerate
          if (this->currentPulses < pulsesToTargetSpeed) {
            this->currentSpeed = this->currentSpeed + (this->currentPulses * accel);
          }
          // decelerate
          else if (this->currentPulses > (targetPulses - pulsesToTargetSpeed)) {
            this->currentSpeed = this->currentSpeed - accel*(this->currentPulses - (targetPulses - pulsesToTargetSpeed));
          }
        }
        
        // send a pulse
        if this->_alternator {
          if ((unsigned long) (micros() - this->_lastMicros) > 10) {
            digitalWrite(this->_pulsePin, HIGH);
            this->_lastMicros = micros();
          }
        }
        else {
          if ((unsigned long) (micros() - this->_lastMicros) > (1 / this->currentSpeed)) {
            digitalWrite(this->_pulsePin, LOW);
            this->_lastMicros = micros();
          }
        }

        // switch alternator
        this->alternator = !this->alternator;

        // increment current pulses
        this->currentPulses++;
      }
    }
    
    // reset currentSpeed and currentPulses;
    void cmdReset() {
      this->currentSpeed  = 0;
      this->currentPulses = 0;
    }
};

void setup() {
  Serial.begin(9600);
}

void loop() {  


  if (freqInput != 0 && freqInput < maxFreq && freqInput != lastFreqInput) {
    motorA.pcmdOn();
    motorA.pcmdStart(1, 20, freqInput, 5000);
    lastFreqInput = freqInput;
  }
  else if (freqInput == 0) {
    motorA.pcmdOff();
  }

  // heartbeat
  if (millis() - heartBeatTime > 3000) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    heartBeatTime = millis();
    Serial.print("freqInput (Hz): ");
    Serial.println(freqInput);
  }
}

// FOR DEBUGGING: get user frequency input from serial port
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char) Serial.read();
    // add it to the periodInputString:
    if (isDigit(inChar)) {
      freqInputString += inChar;
    }
    // if incoming character is a newline, set a flag so the main loop can do something about it:
    if (inChar == '\n') {
      freqInput = freqInputString.toInt();
      freqInputString = "";
    }
  }
}








//// when master asks for data
//void requestEvent() {
//  // payload container
//  byte OutPayload[OUT_PAYLOAD_SIZE];
//
//  Wire.write(OutPayload, OUT_PAYLOAD_SIZE);
//}

//// when master wants to give data
//void receiveEvent(int byteCount) {
//  dir      =  Wire.read(); // first byte is direction
//  pwmWheel =  Wire.read(); // second byte is PWM
//}
