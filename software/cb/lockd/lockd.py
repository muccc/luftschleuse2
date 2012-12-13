import ConfigParser
import serialinterface
import sys
import time

config = ConfigParser.RawConfigParser()
config.read(sys.argv[1])

serialdevice = config.get('MasterController', 'serialdevice')
baudrate = config.get('MasterController', 'baudrate')

ser = serialinterface.SerialInterface(serialdevice, baudrate, timeout=.1)

ser.writeMessage('0', '')

while True:
    for i in range(5):
        print ser.readMessage()
    print time.time()

