import robot
import kinematics as km
import time
import sequencing

# setup
stepper1 = robot.TicStepper(ticSerial = '00307466')
stepper2 = robot.TicStepper(ticSerial = '00307491')
relay	 = robot.FourRelayModule(None, None, None, None)

sparrow = robot.Robot(stepper1, stepper2, relay)

# task timing
currentTime   	= 0
last5000msTime	= 0
last100msTime	= 0

runSeq      = sequencing.Sequence()
step20010   = sequencing.Step(runSeq, 20010, 20020)
step20020   = sequencing.Step(runSeq, 20020, 20030)


sparrow.startUp()

# loop running
try:
	while True:
		# continuous task
		currentTime = time.time()

		# 100ms task
		if currentTime - last100msTime > .1:
			last100msTime = currentTime
			sparrow.getChildStatus()
            
                if (runSeq.currentStep == 0):
       
            print("started running seq")
            runSeq.currentStep = step20010.stepNum

            if runSeq.currentStep == step20010.stepNum:
                # one-shot tasks at step start
                if not step20010.stepActive:
                    step20010.startStep()
                    print("started step20010")
                    sparrow.cmdMove(200,700)

                # command actions in active step
                if step20010.stepActive and not step20010.faulted:
                    print("executed step20010")

                # conditions for faulted status
                if (False) and not step20010.faulted:
                    step20010.faulted = True
                    print("faulted step20010")
                
                # conditions for taskDone
                if sparrow. and not step20010.faulted:
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

		# 5000ms task
		if currentTime - last5000msTime > 5:
			last5000msTime = currentTime

			# report stepper level errors
			if stepper1.errors:
				print('Stepper 1 errors: ', str(stepper1.errors))
			
			if stepper2.errors:
				print('Stepper 2 errors: ', str(stepper2.errors))

# cleanup on exit
except KeyboardInterrupt:
	stepper1.deenergize()
	stepper2.deenergize()
