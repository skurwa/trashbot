#include <Wire.h>
#include <Stepper.h>

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
const int maxFreq         =  (1000000 / HIGH_TIME_MICROSEC);
long heartBeatTime        = 0;
int freqInput           = 0;
int lastFreqInput       = 0;
String freqInputString  = "";
bool stringComplete       = false;

Stepper motorA(motorAEnablePin, motorAStepPin, motorADirPin);

void setup() {
  Serial.begin(9600);
//  // start I2C comm
//  Wire.begin(ADDRESS);
//  Wire.onRequest(requestEvent);
//  Wire.onReceive(receiveEvent);

}

void loop() {  
  // heartbeat
  if (millis() - heartBeatTime > 3000) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    heartBeatTime = millis();
    Serial.print("freqInput (Hz): ");
    Serial.println(freqInput);
  }

  if (freqInput != 0 && freqInput < maxFreq && freqInput != lastFreqInput) {
    motorA.pcmdOn();
    motorA.pcmdStart(1, 20, freqInput, 5000);
    lastFreqInput = freqInput;
  }
  else if (freqInput == 0) {
    motorA.pcmdOff();
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
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
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
