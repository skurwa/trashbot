import ticAPI
import kinematics as km
import time

# setup
stepper1 = ticAPI.TicStepper(ticSerial = '00307501')
stepper2 = ticAPI.TicStepper(ticSerial = '')

# task timing
currentTime   	= 0
last1000msTime	= 0
last100msTime	= 0

stepper1.reset()
stepper2.reset()
stepper1.energize()
stepper2.energize()

# loop running
try:
	while True:
		# continuous task
		currentTime = time.time()

		# 100ms task
		if currentTime - last100msTime > .1:
			stepper1.getStatus()
			stepper2.getStatus()
			lastTime = currentTime
		
			# only start move sequence if robot not moving and not errored
			if stepper1.currentVelocity == 0 and stepper2.currentVelocity == 0 and not (stepper1.errors or stepper2.errors):
				# get desired coordinates from user
				target_x, target_y = list(map(int,input("Enter desired pose: ").strip().split(',')))

				# convert to step frame
				motorAng 		= km.getMotorAngles(target_x, target_y) 
				motorStepPose 	= km.convMotorAngToStepPose(motorAng, 4)

				# send desired pose to motor
				if motorStepPose[0] != None:
					stepper1.setTargetPosition(motorStepPose[0])
					stepper2.setTargetPosition(motorStepPose[1])

		# 1000ms task
		if currentTime - last1000msTime > 1:
			
			# report stepper level errors
			if stepper1.errors:
				print('Stepper 1 errors: ', str(stepper1.errors))
			
			if stepper2.errors:
				print('Stepper 2 errors: ', str(stepper2.errors))
			
			last1000msTime = currentTime



# cleanup on exit
except KeyboardInterrupt:
	stepper1.deenergize()
	stepper2.deenergize()
