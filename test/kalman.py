'''
Kalman filter implementation
Updates a 1 dimensional angle of rotation based on stream of data:
    newAngle: pitch, roll, yaw based on accelerometer data
    newRate: angular velocity data from gyroscope (1-d deg/s)
    dt: timestep between two consecutive data polls
'''


class Kalman:
    #static bias and error variables
    Q_angle = 0.004
    Q_bias = 0.003
    R_measure = 0.03

    angle = 0.0
    bias = 0.0

    P = [ [0 for j in range(2)] for i in range(2) ]  #initialize size of matrix 2x2
    P[0][0] = 0.0 #may have to change the matrix, assuming bias is 0 and that we know the initial starting angle
    P[0][1] = 0.0
    P[1][0] = 0.0
    P[1][1] = 0.0

    def __init__(self, startAngle):
        self.angle = startAngle

# The angle should be in degrees and the rate should be in degrees per second and the delta time in seconds
    def updateAngle(self, newAngle, newRate, dt):

        #predict new angle based on prior data
        rate = newRate - self.bias  #angular velocity with bias offset
        self.angle += dt * rate    #integrated angle based on angular velocity and timestep

        #Update estimation error covariance - Project the error covariance ahead
        self.P[0][0] += dt * (dt*self.P[1][1] - self.P[0][1] - self.P[1][0] + self.Q_angle)
        self.P[0][1] -= dt * self.P[1][1]
        self.P[1][0] -= dt * self.P[1][1]
        self.P[1][1] += self.Q_bias * dt

        #Calculate Kalman gain
        S = self.P[0][0] + self.R_measure  #Estimate error
        K = [0 for i in range(2)] #kalman gain 2x1 vector
        K[0] = self.P[0][0] / S
        K[1] = self.P[1][0] / S

        # Calculate angle and bias - Update estimate with measurement zk (newAngle)
        y = newAngle - self.angle

        self.angle += K[0] * y
        self.bias += K[1] * y

        #Calculate estimation error covariance - Update the error covariance
        temp00 = P[0][0]
        temp01 = P[0][1]

        P[0][0] -= K[0] * temp00
        P[0][1] -= K[0] * temp01
        P[1][0] -= K[1] * temp00
        P[1][1] -= K[1] * temp01

        return self.angle
