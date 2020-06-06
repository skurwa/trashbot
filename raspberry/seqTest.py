import sequencing

runSeq = sequencing.Sequence()

step20010 = sequencing.Step(runSeq, 20010, 20020, 0)
step20020 = sequencing.Step(runSeq, 20020, 20030, 0)

while True:
    if (runSeq.currentStep == 0):
        print("started running seq")
        runSeq.currentStep = step20010.stepNum

    if runSeq.currentStep == step20010.stepNum:
        # one-shot tasks at step start
        if not step20010.stepActive:
            step20010.startStep()
            print("started step20010")

        # command actions in active step
        if step20010.stepActive and not step20010.faulted:
            print("executed step20010")

        # conditions for faulted status
        if (False) and not step20010.faulted:
            step20010.faulted = True
            print("faulted step20010")
        
        # conditions for taskDone
        if (True) and not step20010.faulted:
            step20010.tasksDone = True
            print("finished step20010")

        # faulted recovery
        if step20010.faulted:
            print("recover faulted state")
            step20010.faulted = False

        # wrap-up tasks at end of step
        if step20010.tasksDone:
            step20010.finishStep()
            print("wrapped up step20010")
            print(step20010.stepActive)

    if runSeq.currentStep == step20020.stepNum:
        # one-shot tasks at step start
        if not step20020.stepActive:
            step20020.startStep()
            print("started step20020")
        
        # command actions in active step
        if step20020.stepActive and not step20020.faulted:
            print("executed step20020")

        # conditions for faulted status
        if (False) and not step20020.faulted:
            step20020.faulted = True
            print("faulted step20020")
        
        # conditions for taskDone
        if (True) and not step20020.faulted:
            step20020.tasksDone = True
            print("finished step20020")

        # wrap-up tasks at end of step
        if step20020.tasksDone:
            step20020.finishStep()
            print("wrapped up step20020")
