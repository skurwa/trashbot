int enabPin = 7;
int dm0Pin = 6;
int dm1Pin = 5;
int dm2Pin = 4;
int faultPin = 3;
int stepPin = 10;

void setup() {
  Serial.begin(9600);

  // initialize pins
  pinMode(faultPin, INPUT);
  pinMode(enabPin, OUTPUT);
  pinMode(dm0Pin, OUTPUT);
  pinMode(dm1Pin, OUTPUT);
  pinMode(dm2Pin, OUTPUT);
  pinMode(stepPin, OUTPUT);

  // set inital states
  digitalWrite(enabPin, LOW);
  digitalWrite(dm0Pin, HIGH);
  digitalWrite(dm1Pin, HIGH);
  digitalWrite(dm2Pin, HIGH);
}

void loop() {
  // if faulted, report fault and reset 
  if (digitalRead(faultPin) == LOW) {
    Serial.println("I faulted");
    digitalWrite(dm0Pin, LOW);
    digitalWrite(dm1Pin, LOW);
    digitalWrite(dm2Pin, LOW);
  }
  // if not faulted, step motor
  else {
    Serial.println("I'm running");
    digitalWrite(dm0Pin, LOW);
    digitalWrite(dm1Pin, LOW);
    digitalWrite(dm2Pin, HIGH);
    if (digitalRead(stepPin) == LOW) {
      digitalWrite(stepPin, HIGH);
    }
    else {
      digitalWrite(stepPin, LOW);
    }
  }

  // wait 2.5 seconds
  delay(500);
}
