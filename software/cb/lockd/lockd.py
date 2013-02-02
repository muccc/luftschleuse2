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

    raw_input()
    p = packet.Packet(seq=seq, cmd=ord('D'), data='\x00')
    msg = cipher.encrypt(p.toMessage())
    ser.writeMessage('A', msg)
    for i in range(5):
        print list(cipher.decrypt(ser.readMessage()[1]))

    raw_input()
    p = packet.Packet(seq=seq, cmd=ord('D'), data='\x01')
    msg = cipher.encrypt(p.toMessage())
    ser.writeMessage('A', msg)

    for i in range(5):
        print list(cipher.decrypt(ser.readMessage()[1]))
    #time.sleep(5.002)
    #print time.time()
    
    raw_input()
    p = packet.Packet(seq=seq, cmd=ord('D'), data='\x02')
    msg = cipher.encrypt(p.toMessage())
    ser.writeMessage('A', msg)
    for i in range(5):
        print list(cipher.decrypt(ser.readMessage()[1]))

    #time.sleep(5.002)


