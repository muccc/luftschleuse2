import ConfigParser
import serialinterface
import sys
import time
from Crypto.Cipher import AES
import packet


config = ConfigParser.RawConfigParser()
config.read(sys.argv[1])

serialdevice = config.get('MasterController', 'serialdevice')
baudrate = config.get('MasterController', 'baudrate')
masterkey = config.get('MasterController', 'key')
masterkey =''.join([chr(int(x)) for x in masterkey.split()])

ser = serialinterface.SerialInterface(serialdevice, baudrate, timeout=.1)
cipher = AES.new(masterkey, AES.MODE_ECB)

seq = 0

while True:
    seq+=1
    p = packet.Packet(seq=seq, cmd=ord('L'), data='')
    msg = cipher.encrypt(p.toMessage())
    ser.writeMessage('0', msg)
    #for i in range(5):
    #    print ser.readMessage()
    time.sleep(.002)
    print time.time()
    ser.writeMessage('A', msg)
    time.sleep(.002)

