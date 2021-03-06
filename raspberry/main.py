from time import sleep
import pigpio
import stepperControl

# pin assignments
A_enabPin   = 5
A_dm0Pin    = 21
A_dm1Pin    = 20
A_dm2Pin    = 16
A_stepPin   = 13
A_dirPin    = 6
A_fltPin    = 22
A_limMinPin = 23
A_limMaxPin = 24

B_limMinPin = 17
B_limMaxPin = 27

# connect to PiGPIO daemon
pi = pigpio.pi()

# set pin modes
pi.set_mode(A_enabPin, pigpio.OUTPUT)
pi.set_mode(A_dm0Pin, pigpio.OUTPUT)
pi.set_mode(A_dm1Pin, pigpio.OUTPUT)
pi.set_mode(A_dm2Pin, pigpio.OUTPUT)
pi.set_mode(A_dirPin, pigpio.OUTPUT)
pi.set_mode(A_stepPin, pigpio.OUTPUT)
pi.set_mode(A_limMaxPin, pigpio.INPUT)
pi.set_mode(A_limMinPin, pigpio.INPUT)
pi.set_mode(A_fltPin, pigpio.INPUT)

pi.set_pull_up_down(A_limMaxPin, pigpio.PUD_UP)
pi.set_pull_up_down(A_limMinPin, pigpio.PUD_UP)

try:
    while True:
        if pi.read(A_limMaxPin) == 0 or pi.read(A_limMinPin) == 0:
            pi.write(A_enabPin, 1)
            print("disabled")
        else:
            pi.write(A_enabPin, 0)
            print("enabled")
        if (pi.read(A_fltPin) == 0):
            pi.write(A_dm0Pin, 0)
            pi.write(A_dm1Pin, 0)
            pi.write(A_dm2Pin, 0)
            print("faulted")
        pi.write(A_stepPin, 0)
        pi.write(A_dirPin, 0)
        print("dm0: " + str(pi.read(A_dm0Pin)))
        print("dm1: " + str(pi.read(A_dm1Pin)))
        print("dm2: " + str(pi.read(A_dm2Pin)))
        if not pi.read(A_dm2Pin):
            pi.write(A_dm2Pin, 1)
        sleep(.5)
except KeyboardInterrupt:
    pi.write(A_dm0Pin, 0)
    pi.write(A_dm1Pin, 0)
    pi.write(A_dm2Pin, 0)
    pi.write(A_enabPin, 1)
    pi.stop()
    
