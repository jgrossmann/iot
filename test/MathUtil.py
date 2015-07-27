# Math utility functions
import math

PI = 3.1415926536

# Global functions/hex and byte conversions
tosigned = lambda n: float(n-0x10000) if n>0x7fff else float(n)
tosignedbyte = lambda n: float(n-0x100) if n>0x7f else float(n)

def RtoGround(roll, pitch, yaw):
  return [[math.cos(pitch) * math.cos(yaw), 
        math.sin(roll) * math.sin(pitch) * math.cos(yaw) - math.cos(roll) * math.sin(yaw),
        math.cos(roll) * math.sin(pitch) * math.cos(yaw) + math.sin(roll) * math.sin(yaw)],
       [math.cos(pitch) * math.sin(yaw),
        math.sin(roll) * math.sin(pitch) * math.sin(yaw) + math.cos(roll) * math.cos(yaw),
        math.cos(roll) * math.sin(pitch) * math.sin(yaw) - math.sin(roll) * math.cos(yaw)],
       [-math.sin(pitch),
        math.sin(roll) * math.cos(pitch),
        math.cos(roll) * math.cos(pitch)]]

def RtoPutter(roll, pitch, yaw):
  return zip(*RtoGround(roll, pitch, yaw))

def roll(accelX, accelY, accelZ):
  if accelX**2 + accelZ**2 == 0:
    print "would be divide by 0"
    return (math.atan(4.0 * 9.80665) * 180.0) / PI
  return (math.atan(accelY / math.sqrt(accelX**2 + accelZ**2)) * 180.0) / PI
  #return (math.atan2(-accelX, accelZ) * 180.0) / PI


def pitch(accelX, accelY, accelZ):
  #return (math.atan(accelY / (accelY**2 + accelZ**2)) * 180.0) / PI
  return (math.atan2(-accelX, accelZ) * 180.0) / PI


def yaw(roll, pitch, magX, magY, magZ):
  roll = math.radians(roll)
  pitch = math.radians(pitch)

  x = magX * math.cos(pitch) + magZ * math.sin(pitch)
  y = magX * math.sin(roll) * math.sin(pitch) + magY * math.cos(roll) - magZ * math.sin(roll) * math.cos(pitch)

  #x = magX * math.cos(pitch) + magY * math.sin(pitch) * math.sin(roll)
  #y = magY * math.cos(roll) + magZ * math.sin(roll)
  return (math.atan2(y, x) * 180.0) / PI


def displacement(d, v, a, dt):
  # displacement
	d1 = v*dt
	d2 = 0.5 * a * dt**2
	d = d1 + d2

	# velocity
	v1 = v
	v2 = a*dt
	v = v1 + v2

	return d,v


# Accelerometer conversion
def convertData(rawX, rawY, rawZ, calibration):
  data = lambda v: tosigned(v) / calibration 
  xyz = [data(rawX), data(rawY), data(rawZ)]
  mag = (xyz[0]**2 + xyz[1]**2 + xyz[2]**2)**0.5
  return (xyz, mag)


def getAngle(gyroData, dt):
    return gyroData * dt


def rotateAcceleration(accel, angles):
  #angles: 0:roll, 1:pitch, 2:yaw
  #accel: 0:x, 1:y, 2:z
  gravity = rotateGravity(angles)
  accel[0] -= gravity[0]
  accel[1] -= gravity[1]
  accel[2] -= gravity[2]
  rotAccel = [0.0, 0.0, 0.0]
  roll = math.radians(angles[0])
  pitch = math.radians(angles[1])
  yaw = math.radians(angles[2])
  R = RtoGround(roll, pitch, yaw)
  for i in range(3):
    for j in range(3):
      rotAccel[i] += R[i][j] * accel[j]

  return rotAccel


def rotateGravity(angles):
  #gravityOffset = 0.00063
  gravity = [0.0,0.0,9.80665 - gravityOffset]
  rotGravity = [0, 0, 0]
  roll = math.radians(angles[0])
  pitch = math.radians(angles[1])
  yaw = math.radians(angles[2])
  R = RtoPutter(roll, pitch, yaw)
  for i in range(3):
    for j in range(3):
      rotGravity[i] += R[i][j] * gravity[j]
  return rotGravity




