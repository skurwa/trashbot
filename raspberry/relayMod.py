from time import sleep
import pigpio
from stepperControl import StepperMotor

# pin assignments
pi = pigpio.pi()

motor = StepperMotor(pi,17,27,22)

try:
    while True:
        print(motor.switchEnabState())
        sleep(3)
except KeyboardInterrupt:
    pi.stop()
    

