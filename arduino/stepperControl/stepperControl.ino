#define COUNTERCLOCKWISE  		0
#define CLOCKWISE         		1
#define MIN_LOW_TIME_MICROSEC	20

// pin definition
const int motorAEnablePin = 53;
const int motorADirPin    = 51;
const int motorAStepPin   = 49;

const int maxSpeed      	= (1000000 / MIN_LOW_TIME_MICROSEC);
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

class StepperMotor {
	private:
		int  _enablePin;
		int  _pulsePin;
		int  _dirPin;
		bool _alternator;
		unsigned long _lastMicros;

	public:
		// status bits
		bool stsOn;
		bool stsRunning;
		bool stsIdle;
		bool stsMoveComplete;
		bool stsMoveFailed;

		int	 currentSpeed;
		int  currentPulses;

    	// instance constructor
		StepperMotor(int enablePin, int pulsePin, int dirPin) {
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

		// enable the stepper 
		void cmdOn() {
			digitalWrite(this->_enablePin, HIGH);

			this->stsOn				= true;
			this->stsIdle 			= true;

			this->cmdReset();
		}

		// disable the stepper
		void cmdOff() {
			digitalWrite(_enablePin, LOW);

			this->stsOn = false;
			this->stsIdle = false;

			this->cmdReset();
		}

		// run the stepper for a single pulse
		bool cmdRun(int dir, int accel, int speed, int targetPulses) {
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
		bool cmdStop() {
			if (this->stsRunning) {
				this->stsRunning 	= false;
				this->stsMoveFailed = true;
			}
		}

		// reset motor values
		void cmdReset() {
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
		cmdAccel  = input[1];
		cmdSpeed  = input[2];
		cmdPulses = input[3];

		// adjust cmd speed if specified higher than allowable
		if (cmdSpeed > maxSpeed) {
			cmdSpeed = maxSpeed;
		}

		newInput 	= false;
		inputValue 	= "";
		newCmd   	= true;
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

// FOR DEBUGGING: get user input in format INT1,INT2,INT3,INT4 where INT1 = dir, INT2 = accel, INT3 = speed INT4 = pulses
void serialEvent() {
	while (Serial.available()) {
		// get the new byte:
		char inChar = (char) Serial.read();
		
		// store last value and trigger a flag to main program
		if (inChar == '\n') {
			input[inputIdx] = inputValue.toInt();
			inputValue = "";
			inputIdx = 0;
			newInput = true;
		}
		// store last value
		else if (inChar == ',') {
			input[inputIdx] = inputValue.toInt();
			inputValue = "";
			inputIdx++;
		}
		// build last value
		else {
			inputValue += inChar;
		}
	}
}