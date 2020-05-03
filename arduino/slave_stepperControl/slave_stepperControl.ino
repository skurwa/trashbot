#include <Wire.h>

#define OUT_PAYLOAD_SIZE  5
#define ADDRESS           2
#define COUNTERCLOCKWISE  0
#define CLOCKWISE         1
#define HIGH_TIME_MICROSEC  15

// pin definition
const int motorAEnablePin = 53;
const int motorAStepPin   = 49;
const int motorADirPin    = 51;
const int motorBEnablePin = 29;
const int motorBStepPin   = 4;
const int motorBDirPin    = 5;

// global variables
const int maxFreq       =  (1000000 / HIGH_TIME_MICROSEC);
long heartBeatTime      = 0;

int input[4]  = {0,0,0,0};
bool newInput = false;
bool newCmd   = false;

int cmdDir    = 0;
int cmdAccel  = 0;
int cmdSpeed  = 0;
int cmdPulses = 0;


unsigned long lastMicros = 0;
bool alternator = true;


int inputIdx      = 0;
String inputValue = "";


class StepperMotor {
  // class member vars
  private:
    int  _enablePin;
	  int  _pulsePin;
	  int  _dirPin;
    bool _alternator;
    unsigned long _lastMicros;

  public:
    bool stsOn;
    bool stsRunning;
    bool stsIdle;
	  int	 currentSpeed;
	  int  currentPulses;

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
      this->stsOn = false;
      this->stsRunning = false;
      this->stsIdle = false;

      digitalWrite(enablePin, LOW);
    }

    // enable the stepper 
    void cmdOn() {
	    digitalWrite(this->_enablePin, HIGH);
      this->stsOn = true;
      this->stsIdle = true;
      
      this->cmdReset();
    }

    // disable the stepper
    void cmdOff() {
	    digitalWrite(_enablePin, LOW);
      this->stsOn = false;
    }

    // run the stepper for a single pulse
    bool cmdRun(int dir, int accel, int speed, int targetPulses) {
      if (!this->stsRunning) {
        // reset before running new running command
        cmdReset();
        // assign motor direction
        digitalWrite(_dirPin, dir);
      }

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
          // if 
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
        this->stsIdle    = false;
        this->stsRunning = true;
        // exit function
        return true;
      }
      // update status flags
      this->stsRunning = false;
      this->stsIdle = true;
      // exit function
      return false;
    }

    // reset motor values
    void cmdReset() {
      this->currentPulses = 0;
      this->currentSpeed  = 0;
      this->_lastMicros   = 0;
    }

    void updateMotion(int accel, int speed, int targetPulses) {
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
};  

StepperMotor motorA(motorAEnablePin, motorAStepPin, motorADirPin);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {  
  // turn motor on if not on
  if (!motorA.stsOn) {
    motorA.cmdOn();
  }
  
  // update cmd values for motor if ready to receive new input
  if (motorA.stsIdle && newInput) {
    motorA.cmdReset();

    cmdDir    = input[0];
    Serial.print("cmdDir: ");
    Serial.println(cmdDir);

    cmdAccel  = input[1];
    Serial.print("cmdAccel: ");
    Serial.println(cmdAccel);

    cmdSpeed  = input[2];
    Serial.print("cmdSpeed: ");
    Serial.println(cmdSpeed);

    cmdPulses = input[3];
    Serial.print("cmdPulses: ");
    Serial.println(cmdPulses);

    newInput = false;
    inputValue = "";
    newCmd   = true;

  }

  // run motor if working on previous input or received a new input
  if (motorA.stsRunning || newCmd) {
    motorA.cmdRun(cmdDir, cmdAccel, cmdSpeed, cmdPulses);
    newCmd = false;

    // Serial.print("Motor A current pulses: ");
    // Serial.println(motorA.currentPulses);

    // Serial.print("Motor A current speed: ");
    // Serial.println(motorA.currentSpeed);
  }

  // heartbeat
  if (millis() - heartBeatTime > 1000) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    heartBeatTime = millis();
  }

  // Serial.print("                                 micros: ");
  // Serial.println(micros());

  // DIRECT DEBUG ONLY
  // digitalWrite(motorAStepPin,HIGH);
  // delayMicroseconds(10);
  // digitalWrite(motorAStepPin,LOW);
  // delayMicroseconds(1000000/1200);

  // if (alternator) {
  //   if ((unsigned long) (micros() - lastMicros) > 10) {
  //     digitalWrite(motorAStepPin, HIGH);
  //     lastMicros = micros();

  //     // switch alternator
  //     alternator = !alternator;
  //   }
  // }
  // else {
  //   if ((unsigned long) (micros() - lastMicros) > (1000000 / 1200)) {
  //     digitalWrite(motorAStepPin, LOW);
  //     lastMicros = micros();

  //     // switch alternator
  //     alternator = !alternator;

  //   }
  // }
}

// FOR DEBUGGING: get user input in format INT1,INT2,INT3,INT4 where INT1 = dir, INT2 = accel, INT3 = speed INT4 = pulses
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char) Serial.read();
    Serial.print(inChar);
    // store last value and trigger a flag to main program
    if (inChar == '\n') {
      input[inputIdx] = inputValue.toInt();
      Serial.print("input[");
      Serial.print(inputIdx);
      Serial.print("]: ");
      Serial.println(input[inputIdx]);
      inputValue = "";
      inputIdx = 0;
      newInput = true;
    }
    // store last value
    else if (inChar == ',') {
      input[inputIdx] = inputValue.toInt();
      Serial.print("input[");
      Serial.print(inputIdx);
      Serial.print("]: ");
      Serial.println(input[inputIdx]);
      inputValue = "";
      inputIdx++;
    }
    // build last value
    else {
      inputValue += inChar;
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
