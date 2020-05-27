import math

# robot geometry
a = 420
b = 424
c = 628
d = 65
e = 99

stepModeDict = {'Full step': 1, '1/2 step': 2, '1/4 step': 4, '1/8 step': 8, '1/16 step': 16, '1/32 step': 32}

def alphaA(x_EE, d):
	return x_EE - d

def alphaB(y_EE, e):
	return y_EE - e

def alphaC(b, c, A, B):
	return ((b * b) - (c * c) + (A * A) + (B * B)) / (2 * b)

def betaSimX(x_EE, d):
	return x_EE + d

def betaA(x_EE, a, d):
	return x_EE - a + d

def betaB(y_EE, e):
	return y_EE - e

def betaC(a, b, c, xSim, B):
	return ((b * b) + (a - xSim) * (a - xSim) - (c * c) + (B * B)) / (2 * b)

def alpha(A, B, C):
	return math.degrees(2 * math.atan((B + math.sqrt((A * A) + (B * B) - (C * C))) / (A + C))) + 1.08

def beta(A, B, C):
	return math.degrees(2 * math.atan((B - math.sqrt((A * A) + (B * B) - (C * C))) / (A + C))) - 1.08

def getMotorAngles(target_x, target_y):
	curAlphaA = alphaA(target_x, d)
	curAlphaB = alphaB(target_y, e)
	curAlphaC = alphaC(b, c, curAlphaA, curAlphaB)

	curBetaX = betaSimX(target_x, d)
	curBetaA = betaA(target_x,a, d)
	curBetaB = betaB(target_y, e)
	curBetaC = betaC(a, b, c, curBetaX, curBetaB)

	try:
		return alpha(curAlphaA, curAlphaB, curAlphaC), beta(curBetaA, curBetaB, curBetaC)
	except:
		return None, None

def convMotorAngToStepPose(motorAngles, stepMode1, stepMode2):
	try:
		stepSize1 = stepModeDict[stepMode]
	except:
		# assume full steps if step mode not in dict
		stepSize1 = 1

	try:
		stepSize2 = stepModeDict[stepMode]
	except:
		stepSize2 = 1
	
	if motorAngles[0] != None:
		return int(motorAngles[0] * stepSize1), int(motorAngles[1] * stepSize2)
	return None, None