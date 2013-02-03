import ConfigParser
import serialinterface
import sys
import time
from Crypto.Cipher import AES
import packet
from door import Door
from command import UDPCommand
import Queue

config = ConfigParser.RawConfigParser()
config.read(sys.argv[1])

serialdevice = config.get('Master Controller', 'serialdevice')
baudrate = config.get('Master Controller', 'baudrate')
masterkey = config.get('Master Controller', 'key')
masterkey =''.join([chr(int(x)) for x in masterkey.split()])

ser = serialinterface.SerialInterface(serialdevice, baudrate, timeout=.1)
cipher = AES.new(masterkey, AES.MODE_ECB)
command_queue = Queue.Queue()

udpcommand = UDPCommand('127.0.0.1', 2323, command_queue)

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
    if not command_queue.empty():
        command = command_queue.get()
        if command == 'permunlock':
            for door in doors:
                doors[door].unlock(permanent=True)
        elif command == 'lock':
            for door in doors:
                doors[door].lock()
    for d in self.doors:
        d.tick()
