import math

# robot geometry
a = 420
b = 424
c = 628
d = 65
e = 99

def alphaA(x_EE, d):
	return x_EE - d

def alphaB(y_EE, e):
	return y_EE - e

def alphaC(b, c, A, B):
	return ((b*b) - (c*c) + (A*A) + (B*B)) / (2* b)

def betaSimX(x_EE, d):
	return x_EE + d

def betaA(x_EE, a, d):
	return x_EE - a + d

def betaB(y_EE, e):
	return y_EE - e

def betaC(a, b, c, xSim, B):
	return ((b*b) + (a - xSim)*(a - xSim) - (c*c) + (B*B)) / (2*b)

def alpha(A, B, C):
	return math.degrees(2 * math.atan((B + math.sqrt((A*A) + (B*B) - (C*C)))/(A + C))) + 1.08

def beta(A, B, C):
	return math.degrees(2 * math.atan((B - math.sqrt((A*A) + (B*B) - (C*C)))/(A + C))) - 1.08

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

for xVal in range(0, 500, 250):
	for yVal in range(400,800,100):
		curAlpha, curBeta = getMotorAngles(xVal, yVal)
		print('xPose: {}\tyPose: {}'.format(xVal, yVal))
		print('\talpha: {}\tbeta: {}'.format(curAlpha, curBeta))

# xVal = 230
# yVal = 550

# curAlpha, curBeta = getMotorAngles(xVal, yVal)
# print('xPose: {}\tyPose: {}'.format(xVal, yVal))
# print('\talpha: {}\tbeta: {}'.format(curAlpha, curBeta))