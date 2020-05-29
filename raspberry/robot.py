import RPi.GPIO as GPIO
import subprocess
import yaml
import kinematics as km

GPIO.setmode(GPIO.BCM)
stepModeDict = {'Full step': 1, '1/2 step': 2, '1/4 step': 4, '1/8 step': 8, '1/16 step': 16, '1/32 step': 32}


class TicStepper:
	def __init__(self, ticSerial=''):
		self.serial_id = ticSerial
		self.getStatusFull()

	def ticcmd(self, *args):
		if self.serial_id != '':
			return subprocess.check_output(['ticcmd', '-d', str(self.serial_id)] + list(args))
		else:
			return subprocess.check_output(['ticcmd'] + list(args))

	def getStatusFull(self):
		status = yaml.load(self.ticcmd('-s', '--full'), yaml.Loader)
		self.currentLimit = status['Current limit']
		self.stepMode = status['Step mode']
		self.errors = status['Errors currently stopping the motor']
		self.currentVelocity = status['Current velocity']
		self.currentPosition = status['Current position']
		self.positionUncertain = status['Position uncertain']
		if self.currentVelocity == 0:
			self.stsMoving = False
		else:
			self.stsMoving = True

		return status

	def getStatus(self):
		status = yaml.load(self.ticcmd('-s'), yaml.Loader)
		self.errors = status['Errors currently stopping the motor']
		self.currentVelocity = status['Current velocity']
		self.currentPosition = status['Current position']
		self.positionUncertain = status['Position uncertain']
		if self.currentVelocity == 0:
			self.stsMoving = False
		else:
			self.stsMoving = True

		return status

	def setTargetPosition(self, new_target):
		self.ticcmd('--exit-safe-start', '--position', str(new_target))

	def setTargetVelocity(self, new_target):
		self.ticcmd('--exit-safe-start', '--velocity', str(new_target))

	def haltAndSetPosition(self, position):
		self.ticcmd('--halt-and-set-position', str(position))

	def haltAndHold(self):
		self.ticcmd('--halt-and-hold')

	def home(self, dir):
		if dir == 0:
			self.ticcmd('--home rev')
		else:
			self.ticcmd('--home fwd')

	def resetCommandTimeout(self):
		self.ticcmd('--reset-command-timeout')

	def deenergize(self):
		self.ticcmd('--deenergize')

	def energize(self):
		self.ticcmd('--energize')

	def exitSafeStart(self):
		self.ticcmd('--exit-safe-start')

	def enterSafeStart(self):
		self.ticcmd('--enter-safe-start')

	def reset(self):
		self.ticcmd('--reset')

	def clearDriverError(self):
		self.ticcmd('--clear-driver-error')

	def setMaxSpeed(self, speed):
		self.ticcmd('--max-speed', str(speed))

	def setStartSpeed(self, speed):
		self.ticcmd('--starting-speed', str(speed))

	def setMaxAccel(self, accel):
		self.ticcmd('--max-accel', str(accel))

	def setMaxDecel(self, decel):
		self.ticcmd('--max-decel', str(decel))

	def setStepMode(self, mode):
		self.ticcmd('--step-mode', str(mode))

	def setCurrentLimit(self, current):
		self.ticcmd('--current', str(current))

	def setDecayMode(self, mode):
		self.ticcmd('--decay', str(mode))


class FourRelayModule:
	def __init__(self, switchPin1, switchPin2, switchPin3, switchPin4):
		GPIO.setup(switchPin1, GPIO.OUT)
		GPIO.setup(switchPin2, GPIO.OUT)
		GPIO.setup(switchPin3, GPIO.OUT)
		GPIO.setup(switchPin4, GPIO.OUT)

		self.switch1 = switchPin1
		self.switch2 = switchPin2
		self.switch3 = switchPin3
		self.switch4 = switchPin4

		GPIO.output(switchPin1, GPIO.LOW)
		GPIO.output(switchPin2, GPIO.LOW)
		GPIO.output(switchPin3, GPIO.LOW)
		GPIO.output(switchPin4, GPIO.LOW)

	def on1(self):
		GPIO.output(self.switch1, GPIO.HIGH)

	def off1(self):
		GPIO.output(self.switch1, GPIO.LOW)

	def on2(self):
		GPIO.output(self.switch2, GPIO.HIGH)

	def off2(self):
		GPIO.output(self.switch2, GPIO.LOW)

	def on3(self):
		GPIO.output(self.switch3, GPIO.HIGH)

	def off3(self):
		GPIO.output(self.switch3, GPIO.LOW)

	def on4(self):
		GPIO.output(self.switch4, GPIO.HIGH)

	def off4(self):
		GPIO.output(self.switch4, GPIO.LOW)


class Robot:
	def __init__(self, stepper1, stepper2, relay):
		self.stepper1 = stepper1
		self.stepper2 = stepper2
		self.relay = relay
		self.pathActive = False

    # initialize steppers
	def startUp(self):
		self.stepper1.energize()
		self.stepper2.energize()

		self.stepper1.getStatusFull()
		self.stepper2.getStatusFull()

	def getChildStatusFull(self):
		self.stepper1.getStatusFull()
		self.stepper2.getStatusFull()

	def getChildStatus(self):
		self.stepper1.getStatus()
		self.stepper2.getStatus()
	
	def cmdGrip(self):
		self.relay.on1()
 
	def cmdRelease(self):
		self.relay.off1()

	def cmdFlick(self):
		self.relay.on2()

	def cmdReturn(self):
		self.relay.off2()

	def cmdMove(self, xPose, yPose):
		# convert cartesian to step coordinates
		motorStepPose = km.convMotorAngToStepPose(km.getMotorAngles(xPose, yPose), self.stepper1.stepMode, self.stepper2.stepMode)

		# send desired pose to motor
		if motorStepPose[0] != None:
			self.stepper1.setTargetPosition(motorStepPose[0])
			self.stepper2.setTargetPosition(motorStepPose[1])
			return 1
		else:
			print('Invalid pose')
			return 0
	def cmdMovePath(self, movePath):
		# one-shot at beginning of path
		if not self.pathActive:
			i = 0
			pathLen = len(movePath)
			self.pathActive = True

		# execute moves while path active
		if self.pathActive:
			if not (self.stepper1.stsMoving or self.stepper2.stsMoving):
				self.cmdMove(movePath[i][0], movePath[i][1])
				if i < pathLen:
					i += 1

		if i == pathLen:
			self.pathActive = False
		
		
			