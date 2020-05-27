import ticAPI
import kinematics as km
import time

# setup
stepper1 = ticAPI.TicStepper(ticSerial = '00307501')
stepper2 = ticAPI.TicStepper(ticSerial = '')

# task timing
currentTime   	= 0
last5000msTime	= 0
last100msTime	= 0

stepper1.energize()
stepper2.energize()

stepper1.getStatusFull()
stepper2.getStatusFull()

# loop running
try:
	while True:
		# continuous task
		currentTime = time.time()

		# 100ms task
		if currentTime - last100msTime > .1:
			lastTime = currentTime

			stepper1.getStatus()
			stepper2.getStatus()
			
		
			# only start move sequence if robot not moving and not errored
			if stepper1.currentVelocity == 0 and stepper2.currentVelocity == 0 and not (stepper1.errors or stepper2.errors):
				# get desired coordinates from user
				target_x, target_y = list(map(int,input("Enter desired pose: ").strip().split(',')))

				# convert to step frame
				motorStepPose = km.convMotorAngToStepPose(km.getMotorAngles(target_x, target_y), stepper1.stepMode, stepper2.stepMode)

				# send desired pose to motor
				if motorStepPose[0] != None:
					stepper1.setTargetPosition(motorStepPose[0])
					stepper2.setTargetPosition(motorStepPose[1])

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
