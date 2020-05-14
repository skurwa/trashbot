import RPi.GPIO as GPIO
from time import sleep

GPIO.setmode(GPIO.BCM)

testPinMin = 14
testPinMax = 15

GPIO.setup(testPinMin, GPIO.IN, pull_up_down = GPIO.PUD_UP)
GPIO.setup(testPinMax, GPIO.IN, pull_up_down = GPIO.PUD_UP)
try:
    while True:
        if GPIO.input(testPinMin):
            print("min all good")
        else:
            print("min switch hit")
            
        if GPIO.input(testPinMax):
            print("max all good")
        else:
            print("max switch hit")
        sleep(1)        
except KeyboardInterrupt:
    GPIO.cleanup()