############################################################
# IOT Poject
# GUI for Light Bulb
# This program is the core that loads other dependent
# programs.
############################################################

import subprocess
import os
import signal
import Tkinter as tk
import ImageTk
import Image
import socket
import select
import httplib
import json
from threading import Thread
import pexpect
import sys
import time
import math

# MAC address (with modified firmware)
#20:73:7A:15:13:DE

# MAC address (Vera, Salar, and Wei's tag)
#00:10:18:01:0B:39


# Constants
PORT_LOCALINFO = 50012
PORT_TIME = 50011
PORT_INTENSITY = 50013
HOST = "127.0.0.1"
baseUrl = "api.parse.com"

# Initializing variables and basic set up for Bulb GUI. All images for bulb are pulled from ./Images/ folder
bulbRoot = tk.Tk()
img = ImageTk.PhotoImage(Image.open("../Images/Intensity_0.png").resize((640,500), Image.ANTIALIAS))
bulbRoot.geometry("640x800")
panel = tk.Label(bulbRoot, text="hello", image = img)
headingPanel = tk.Label(bulbRoot, padx=10, text="LightBulb Emulator", fg="green", bg="black", font="Verdana 10 bold", width=640)
weatherPanel = tk.Label(bulbRoot, padx=10, text="Weather,City, and Local Time information to be displayed here", fg="red", bg="yellow", font="Verdana 10 bold", width=640, height=200)
panel.pack(side="bottom")
headingPanel.pack(side="top")
weatherPanel.pack(side="top")
intensity, currentTime = "0", None
city, country = None, None
briefWeather, weatherDesc = None, None








tosigned = lambda n: float(n-0x10000) if n>0x7fff else float(n)
tosignedbyte = lambda n: float(n-0x100) if n>0x7f else float(n)


# Wiced sense class
class wicedsense:

  accx = 0

  def pushToCloud(self, frames):
    print frames[0]
    connection = httplib.HTTPSConnection("api.parse.com", 443)
    connection.connect()
    connection.request('PUT', '/1/classes/Putt/12fz4AHTDK', json.dumps({
       "frames": frames
     }), {
       "X-Parse-Application-Id": "iAFEw9XwderX692l0DGIwoDDHcLTGMGtcBFbgMqb",
       "X-Parse-REST-API-Key": "I0xfoOS0nDqaHxfSBTgLNMuXGtsStl7zO0XZVDZX",
       "Content-Type": "application/json"
     }) 


  def processData(self):
    frames = []
    interval = 1.0 / 10.0
    t0 = self.time[0]
    t1 = self.time[1]
    i = 1
    vx = 0
    vy = 0
    vz = 0
    x = 0
    y = 0
    z = 0
    time = 0;
    print "processing data"
    print len(self.accel)
    while i < len(self.accel):
        t1 = self.time[i]
        if((t1 - t0) >= interval):
            time = interval
            interval = 1.0/10.0
        else:
            time = t1 - t0
            interval -= time

        x += vx*time + (0.5 * self.accel[i][0] * time**2)
        y += vy*time + (0.5 * self.accel[i][1] * time**2)
        z += vz*time + (0.5 * self.accel[i][2] * time**2)
        vx += self.accel[i][0] * time
        vy += self.accel[i][1] * time
        vz += self.accel[i][2] * time

        print "ax, ay, az: "+ str(self.accel[i][0]) + ", " + str(self.accel[i][1]) + ", " + str(self.accel[i][2])
        print "vx, vy, vz: "+ str(vx) + ", " + str(vy) + ", "+ str(vz)
        print "x, y, z: "+ str(x) + ", " + str(y) + ", " + str(z)
        print "time, interval: "+ str(time) + ", "+str(interval)
        if(interval == (1.0/10.0)):
            frames.append([x, y, z])
        else:
            i += 1
        t0 += time          

    return frames
            
        

  # Accelerometer conversion
  def convertData(self, rawX, rawY, rawZ, calibration):
    data = lambda v: tosigned(v) / calibration 
    xyz = [data(rawX), data(rawY), data(rawZ)]
    mag = (xyz[0]**2 + xyz[1]**2 + xyz[2]**2)**0.5
    return (xyz, mag)
    

  # Init function to connect to wiced sense using mac address
  # Blue tooth address is obtained from blescan.py 
  def __init__( self, bluetooth_adr ):
    self.con = pexpect.spawn('gatttool -b ' + bluetooth_adr + ' --interactive')
    self.con.logfile = sys.stdout
    self.con.expect('\[LE\]>', timeout=600)
    print "Preparing to connect. You might need to press the side button..."
    self.con.sendline('connect')
    # test for success of connect
    self.con.expect('Connection successful.*\[LE\]>')
    print "Connection successful"

    self.cb = {}

    # Data from sensors
    self.time = []
    self.accel = []
    self.gyro = []
    self.magnetometer=[]
    self.humidity = 0
    self.temperature = 0
    self.pressure = 0
    return

  # Function to write a value to a particular handle  
  def char_write_cmd( self, handle, value ):
    cmd = 'char-write-req 0x%02x 0%x' % (handle, value)
    #print cmd
    self.con.sendline( cmd )
    return

  # Funcrion to read from a handle
  def char_read_hnd(self, handle):
    self.con.sendline('char-read-hnd 0x%02x' % handle)
    self.con.expect('descriptor: .*? \r')
    after = self.con.after
    rval = after.split()[1:]
    #print rval
    # decode data obtained from the sensor tag
    #print "RVAL"
    #print rval
    rval = ''.join(rval)
    #print rval  
    self.dataCallback(rval)

  def processdata(self,data):

    # This is to decode and get sensor data from the packets we received
    # please refer to the broadcom data packet format document to understand the structure of the packet
    data_decode = ''.join(data)
    mask = int(data_decode[0:2],16)
    print "processing data"
    if(mask==0x0b):
      print "PROCESSING"
      self.accelerometer = [int((data_decode[4:6] + data_decode[2:4]),16)]
      #self.accelerometer.append(int((data_decode[4:6] + data_decode[2:4]),16))
      self.accelerometer.append(int((data_decode[8:10] + data_decode[6:8]),16))
      self.accelerometer.append(int((data_decode[12:14] + data_decode[10:12]),16))
      self.gyroscope = [int((data_decode[16:18] + data_decode[14:16]),16)]
      #self.gyroscope.append(int((data_decode[16:18] + data_decode[14:16]),16))
      self.gyroscope.append(int((data_decode[20:22] + data_decode[18:20]),16))
      self.gyroscope.append(int((data_decode[24:26] + data_decode[22:24]),16))
      self.magnetometer = [int((data_decode[28:30] + data_decode[26:28]),16)]
      #self.magnetometer.append(int((data_decode[28:30] + data_decode[26:28]),16))
      self.magnetometer.append(int((data_decode[32:34] + data_decode[30:32]),16))
    else:
      self.humidity = int((data_decode[4:6] + data_decode[2:4]),16)
      self.pressure = int((data_decode[4:6] + data_decode[2:4]),16)/10
      self.temperature = int((data_decode[4:6] + data_decode[2:4]),16)/10
    #print "ACCELEROMETER"
    #print self.accelerometer



    

  def register_cb( self, handle, fn ):
    self.cb[handle]=fn
    return

  def dataCallback(self, v):
    bytelen = len(v)
    # Make sure 18 (?) bytes are received
    if(v[1] == "b"):
      #vx = int( str(v[2]*256 + v[1]) )
      vx = (int(v[2],16))*256 + int(v[3],16)
      #print int(v[2],16)
      (Axyz, Amag) = self.convertData(vx,0,0,1.0)
      #wicedsense.accx = int(Axyz[0])
      wicedsense.accx = int(round(math.fabs(Axyz[0])))
      #print "vx: " + str(vx)
      #self.accel.append(Axyz)
      #self.gyro.append(Gxyz)
      #print "Axyz: " + str(Axyz)
      #print "Amag: " + str(Amag)
      #print "Gxyz: " + str(Gxyz)
      #print "Gmag: " + str(Gmag)
    return

class SensorCallbacks:

  data = {}

  def __init__(self,addr):
    self.data['addr'] = addr









# This function is a callback after periodic interval. Called by bulbRoot.after()
# We also update the GUI of bulb here
def checkCondition(accx,intensity):

	print
	print "accx"
	print accx
	print
    
	if accx > 5:
		intensity = str(int(intensity)+1)
	else:
		intensity = str(int(intensity)-1)

	if int(intensity) > 10:
		intensity = "10"
	if int(intensity) < 0:
		intensity = "0"

	print "Intensity is", intensity , type(intensity)
	if intensity == "0":
		new_path = "Intensity_0.png"
	if intensity == "1":
		new_path = "Intensity_1.png"
	if intensity == "2":
		new_path = "Intensity_2.png"
	if intensity == "3":
		new_path = "Intensity_3.png"
	if intensity == "4":
		new_path = "Intensity_4.png"
	if intensity == "5":
		new_path = "Intensity_5.png"
	if intensity == "6":
		new_path = "Intensity_6.png"
	if intensity == "7":
		new_path = "Intensity_7.png"
	if intensity == "8":
		new_path = "Intensity_8.png"
	if intensity == "9":
		new_path = "Intensity_9.png"
	if intensity == "10":
		new_path = "Intensity_10.png"

	if intensity != None:
		# Update the new image and text when needed in GUI
		newImg = ImageTk.PhotoImage(Image.open("../Images/" + new_path).resize((640,500), Image.ANTIALIAS))
		panel.configure(image = newImg)
		panel.image = newImg
	
		connection = httplib.HTTPSConnection(baseUrl, 443)
		connection.connect()
		connection.request('PUT', '/1/classes/Bulb/7SPLF6KHR6', json.dumps({
       "City": city,
       "Weather": briefWeather,
		"Intensity": int(intensity)
     }), {
       "X-Parse-Application-Id": "LLXKP3xsmyHpEsZiYo6b8i9kHhsHDKyrlkW5lNrP",
       "X-Parse-REST-API-Key": "zbXsj30R5Tgn4SfSpWOsAyyQh477RbzvdNd89gVi",
       "Content-Type": "application/json"
     }) 
	#print(json.loads(connection.getresponse().read()))
	return intensity
	#bulbRoot.after(2000, checkCondition, intensity)





def updateloop(tag,intensity):
  while True:
    try:
      a = tag.char_read_hnd(0x2a)
      #wicedsense.dataCallback()
      print "accx"
      print wicedsense.accx
      print ""

      # Tkinter graphics - potentially create a loop without .after (?)
      intensity = checkCondition(wicedsense.accx,intensity)
      
      #bulbRoot.mainloop()
      #bulbRoot.tk.dooneevent(tkinter._tkinter.DONT_WAIT)
      bulbRoot.after(100, updateloop, tag, intensity)
      bulbRoot.mainloop()
    except Exception, e:
      print str(e)
      pass


# main()
if __name__=="__main__":
  displacement = 0

  bluetooth_adr = sys.argv[1]
  #data['addr'] = bluetooth_adr
  if len(sys.argv) > 2:
      datalog = open(sys.argv[2], 'w+')

 
  try:   
      print "[re]starting.."

      tag = wicedsense(bluetooth_adr)
      cbs = SensorCallbacks(bluetooth_adr)

      i=1

      tag.char_write_cmd(0x2b,0x01)

      intensity = "0"
     

  except Exception, e:
      print str(e)
      pass

  updateloop(tag,intensity)




	# Once we close the GUI, we need to clean up and close the processes p1 and p2
	#p1.terminate()
	#os.killpg(p2.pid, signal.SIGTERM)


