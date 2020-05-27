import kinematics as km

stepMode = 'Full step'
ang = km.getMotorAngles(250, 600)
print('ang: {}'.format(ang))

step = km.convMotorAngToStepPose(ang, stepMode)
print('step: {}, stepMode: {}'.format(step, stepMode))

stepMode = '1/2 step'
step = km.convMotorAngToStepPose(ang, stepMode)
print('step: {}, stepMode: {}'.format(step, stepMode))

stepMode = '1/4 step'
step = km.convMotorAngToStepPose(ang, stepMode)
print('step: {}, stepMode: {}'.format(step, stepMode))

stepMode = '1/8 step'
step = km.convMotorAngToStepPose(ang, stepMode)
print('step: {}, stepMode: {}'.format(step, stepMode))
