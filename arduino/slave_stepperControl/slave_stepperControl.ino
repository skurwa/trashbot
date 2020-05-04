#include <Wire.h>
#include <StepperMotor.h>

#define OUT_PAYLOAD_SIZE      5
#define ADDRESS               2
#define COUNTERCLOCKWISE  		0
#define CLOCKWISE         		1
#define MIN_LOW_TIME_MICROSEC	20

// pin definition
const int motorAEnablePin = 53;
const int motorADirPin    = 51;
const int motorAStepPin   = 49;

const int maxSpeed      	  = (1000000 / MIN_LOW_TIME_MICROSEC);
unsigned long heartBeatTime = 0;

int input[4]  = {0,0,0,0};
bool newInput = false;
bool newCmd   = false;

int cmdDir    = 0;
int cmdAccel  = 0;
int cmdSpeed  = 0;
int cmdPulses = 0;

int inputIdx      = 0;
String inputValue = "";

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
		cmdAccel  = input[1];
		cmdSpeed  = input[2];
		cmdPulses = input[3];

		// adjust cmd speed if specified higher than allowable
		if (cmdSpeed > maxSpeed) {
			cmdSpeed = maxSpeed;
		}

		newInput = false;
		inputValue = "";
		newCmd   = true;
	}

  // run motor if working on previous input or received a new input
  if (motorA.stsRunning || newCmd) {
    motorA.cmdRun(cmdDir, cmdAccel, cmdSpeed, cmdPulses);
    newCmd = false;
  }

  // heartbeat
  if (millis() - heartBeatTime > 1000) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    heartBeatTime = millis();
  }
}

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
