import time 

class Sequence:
    def __init__(self):
        self.currentStep = 0

class Step:
    def __init__(self, seq, stepNum, nextStepNum, stepTime = 0):
        self.seq        = seq
        self.stepNum    = stepNum
        self.nextStep   = nextStepNum
        self.stepActive = False
        self.stepTime   = stepTime
        self.tasksDone  = False
        self.faulted    = False

    def startStep(self):
        self.stepActive = True
        self.stepTime   = time.time()

    def finishStep(self):
        self.tasksDone = False
        self.stepActive = False
        self.seq.currentStep = self.nextStep
