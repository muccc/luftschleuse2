import ConfigParser
import serialinterface
import sys
import time
from Crypto.Cipher import AES
import packet
from door import Door

config = ConfigParser.RawConfigParser()
config.read(sys.argv[1])

serialdevice = config.get('Master Controller', 'serialdevice')
baudrate = config.get('Master Controller', 'baudrate')
masterkey = config.get('Master Controller', 'key')
masterkey =''.join([chr(int(x)) for x in masterkey.split()])

ser = serialinterface.SerialInterface(serialdevice, baudrate, timeout=.1)
cipher = AES.new(masterkey, AES.MODE_ECB)

doors = {}

for section in config.sections():
    if config.has_option(section, 'type'):
        t = config.get(section, 'type')
        if t == 'door':
            name = section
            txseq = int(config.get(section, 'txsequence'))
            rxseq = int(config.get(section, 'rxsequence'))
            address = config.get(section, 'address')
            key = config.get(section, 'key')
            print 'Adding door "%s"'%section
            door = Door(name, address, txseq, rxseq, key, ser)
            doors[address] = door
        else:
            print 'Unknown entry type "%s"', t
seq = 0

while True:
    m = ser.readMessage()
    if m[0] in doors:
        doors[m[0]].update(m[1])
    seq+=1
    if seq == 80:
        doors['A'].unlock(permanent=True)
        pass
    if seq == 160:
        doors['A'].lock()
        seq = 0

