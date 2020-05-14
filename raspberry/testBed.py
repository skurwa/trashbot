from time import sleep
import pigpio

A_limitMaxPin = 15

# connect to PiGPIO daemon
pi = pigpio.pi()

pi.set_mode(A_limitMaxPin, pigpio.INPUT)
pi.set_pull_up_down(A_limitMaxPin, pigpio.PUD_UP)




    

