#!/usr/bin/env python
# Michael Saunby. April 2013
#
# Notes.
# pexpect uses regular expression so characters that have special meaning
# in regular expressions, e.g. [ and ] must be escaped with a backslash.
#
#   Copyright 2013 Michael Saunby
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.

import pexpect
import sys

# Wiced sense class

class wicedsense:
  # Init function to connect to wiced sense using mac address
  # Blue tooth address is obtained from blescan.py 
  def __init__( self, bluetooth_adr ):
    self.con = pexpect.spawn('gatttool -b ' + bluetooth_adr + ' --interactive')
    self.con.expect('\[LE\]>', timeout=600)
    print "Preparing to connect. You might need to press the side button..."
    self.con.sendline('connect')
    # test for success of connect
    self.con.expect('Connection successful.*\[LE\]>')
    print "Connection successful"

    self.cb = {}

    # Data from sensors
    self.accelerometer = []
    self.gyroscope = []
    self.magnetometer=[]
    self.humidity = 0
    self.temperature = 0
    self.pressure = 0
    return

  # Function to write a value to a particular handle  
  def char_write_cmd( self, handle, value ):
    cmd = 'char-write-req 0x%02x 0%x' % (handle, value)
    print cmd
    self.con.sendline( cmd )
    return

  # Funcrion to read from a handle
  def char_read_hnd(self, handle):
    self.con.sendline('char-read-hnd 0x%02x' % handle)
    self.con.expect('descriptor: .*? \r')
    after = self.con.after
    rval = after.split()[1:]
    print rval
    # decode data obtained from the sensor tag
    self.processdata(rval)

  def processdata(self,data):

    # This is to decode and get sensor data from the packets we received
    # please refer to the broadcom data packet format document to understand the structure of the packet
    data_decode = ''.join(data)
    mask = int(data_decode[0:2],16)
    if(mask==0x0b):
      self.accelerometer.append(int((data_decode[4:6] + data_decode[2:4]),16))
      self.accelerometer.append(int((data_decode[8:10] + data_decode[6:8]),16))
      self.accelerometer.append(int((data_decode[12:14] + data_decode[10:12]),16))
      self.gyroscope.append(int((data_decode[16:18] + data_decode[14:16]),16))
      self.gyroscope.append(int((data_decode[20:22] + data_decode[18:20]),16))
      self.gyroscope.append(int((data_decode[24:26] + data_decode[22:24]),16))
      self.magnetometer.append(int((data_decode[28:30] + data_decode[26:28]),16))
      self.magnetometer.append(int((data_decode[32:34] + data_decode[30:32]),16))
    else:
      self.humidity = int((data_decode[4:6] + data_decode[2:4]),16)
      self.pressure = int((data_decode[4:6] + data_decode[2:4]),16)/10
      self.temperature = int((data_decode[4:6] + data_decode[2:4]),16)/10

    # Notification handle = 0x002b 
  def notification_loop( self ):
    while True:
      try:
        pnum = self.con.expect('Notification handle = .*? \r', timeout=4)
        print "Printing in notification loop"
      except pexpect.TIMEOUT:
        print "TIMEOUT exception!"
        break
      if pnum==0:
        after = self.con.after
        hxstr = after.split()[3:]
        handle = long(float.fromhex(hxstr[0]))
        if True:
          self.cb[handle]([long(float.fromhex(n)) for n in hxstr[2:]])
          pass
        else:
          print "TIMEOUT!!"
          pass

  def register_cb( self, handle, fn ):
    self.cb[handle]=fn;
    return


class SensorCallbacks:

  data = {}

  def __init__(self,addr):
    self.data['addr'] = addr

# main function USAGE python sense.py <mac address>
def main():

  bluetooth_adr = sys.argv[1]
  #data['addr'] = bluetooth_adr
  if len(sys.argv) > 2:
      datalog = open(sys.argv[2], 'w+')

 # while True:
  try:   
    print "[re]starting.."

    tag = wicedsense(bluetooth_adr)
    cbs = SensorCallbacks(bluetooth_adr)

    print cbs.data['addr']
    
    
    #tag.register_cb(0x2b,cbs.processdata)
    tag.char_write_cmd(0x2b,0x01)
    # Read from handle 
    tag.char_read_hnd(0x2a)
    #tag.notification_loop()

    # Print the data
    print tag.accelerometer 
    print tag.gyroscope
    print tag.magnetometer

    # @avi: Add final REST API code to push data to Parse Cloud


  except:
    pass

if __name__ == "__main__":
  main()
