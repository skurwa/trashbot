import subprocess
import yaml

class TicStepper:
	def __init__(self, ticSerial = ''):
		self.serial_id = ticSerial

	def ticcmd(self, *args):
		if self.serial_id != '':
			return subprocess.check_output(['ticcmd', '-d', str(self.serial_id)] + list(args))
		else:
			return subprocess.check_output(['ticcmd'] + list(args))
	def getStatus(self):
		return yaml.load(self.ticcmd('-s', '--full'), yaml.Loader)

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

