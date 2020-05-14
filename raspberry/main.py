from time import sleep
import pigpio

# pin assignments
dirPin  = 20
stepPin = 21

# connect to PiGPIO daemon
pi = pigpio.pi()

# set pin modes
pi.set_mode(dirPin, pigpio.OUTPUT)
pi.set_mode(stepPin, pigpio.OUTPUT)

try:
	while True:


except KeyboardInterrupt:
	
