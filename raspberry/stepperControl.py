# helper functions
from time import sleep
import pigpio

class StepperMotor:
    def __init__(self, daemon, enabPin, stepPin, dirPin):
        self.enabPin     = enabPin
        self.stepPin     = stepPin
        self.dirPin      = dirPin
        self.daemon      = daemon

        daemon.set_mode(enabPin, pigpio.OUTPUT)
        daemon.set_mode(dm0Pin, pigpio.OUTPUT)
        daemon.set_mode(dm1Pin, pigpio.OUTPUT)
        daemon.set_mode(dm2Pin, pigpio.OUTPUT)
        daemon.set_mode(dirPin, pigpio.OUTPUT)
        daemon.set_mode(stepPin, pigpio.OUTPUT)
        daemon.set_mode(limMaxPin, pigpio.INPUT)
        daemon.set_mode(limMinPin, pigpio.INPUT)
        daemon.set_mode(fltPin, pigpio.INPUT)
        daemon.set_pull_up_down(A_limMaxPin, pigpio.PUD_UP)
        daemon.set_pull_up_down(A_limMinPin, pigpio.PUD_UP)
    



# helpers

def generate_ramp(pi, ramp):
    """Generate ramp wave forms.
    ramp:  List of [Frequency, Steps]
    """
    pi.wave_clear()     # clear existing waves
    length = len(ramp)  # number of ramp levels
    wid = [-1] * length

    # Generate a wave per ramp level
    for i in range(length):
        frequency = ramp[i][0]
        micros = int(500000 / frequency)
        wf = []
        wf.append(pigpio.pulse(1 << STEP, 0, micros))  # pulse on
        wf.append(pigpio.pulse(0, 1 << STEP, micros))  # pulse off
        pi.wave_add_generic(wf)
        wid[i] = pi.wave_create()

    # Generate a chain of waves
    chain = []
    for i in range(length):
        steps = ramp[i][1]
        x = steps & 255
        y = steps >> 8
        chain += [255, 0, wid[i], 255, 1, x, y]

    pi.wave_chain(chain)  # Transmit chain.