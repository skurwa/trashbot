import ticAPI
import math

stepper1 = ticAPI.TicStepper(ticSerial = '00307501')
stepper2 = ticAPI.TicStepper(ticSerial = '')

# robot geometry
a = 420
b = 424
c = 628
d = 65
e = 99

def A(x_EE, d):
	return x_EE - d

def B(y_EE, e):
	return y_EE - e

def C(b, c, A, B):
	return ((b*b) - (c*c) + (A*A) + (B*B)) / (2* b)

def alpha(A, B, C):
	return math.degrees(2 * math.atan((B + math.sqrt((A*A) + (B*B) - (C*C)))/(A + C))) + 1.8

def beta(A, B, C):
	return math.degrees(2 * math.atan((B - math.sqrt((A*A) + (B*B) - (C*C)))/(A + C)))

def getMotorAngles(target_x, target_y):
	curA = A(target_x, d)
	curB = B(target_y, e)
	curC = C(b, c, curA, curB)
	try:
		return alpha(curA, curB, curC), beta(curA, curB, curC)
	except:
		return None, None

def convMotorAngToStepPose(motorAngles, stepMode):
	if motorAngles[0] != None:
		return int(motorAngles[0] * stepMode), int(motorAngles[1]*stepMode)
	return None, None

stepper1.reset()
stepper1.energize()

try:
	while True:
		if stepper1.getStatus()['Current velocity'] == 0:
			# get desired coordinates from user
			target_x, target_y = list(map(int,input("Enter desired pose: ").strip().split(',')))

			# convert to step frame
			motorAng = getMotorAngles(target_x, target_y) 
			print(motorAng)
			
			motorStepPose = convMotorAngToStepPose(motorAng, 4)
			print(motorStepPose)

			# send desired pose to motor
			if motorStepPose[0] != None:
				stepper1.setTargetPosition(motorStepPose[0])
				stepper2.setTargetPosition(motorStepPose[1])
				
except KeyboardInterrupt:
	stepper1.deenergize()
	stepper2.deenergize()
