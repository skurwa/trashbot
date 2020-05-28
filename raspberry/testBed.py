import kinematics as km

stepModes = ['Full step', '1/2 step', '1/4 step', '1/8 step']

ang = km.getMotorAngles(250, 600)
print('ang: {}'.format(ang))

for stepMode in stepModes:
    
    step = km.convMotorAngToStepPose(ang, stepMode, stepMode)
    print('step: {}, stepMode: {}'.format(step, stepMode))
