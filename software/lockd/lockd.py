import ConfigParser
import serialinterface
import sys
import time
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
    address, message = ser.readMessage()
    if address in doors:
        doors[address].update(message)

    if not command_queue.empty():
        command = command_queue.get()
        if command == 'permunlock':
            for door in doors.values():
                door.unlock(permanent=True)
        elif command == 'unlock':
            for door in doors.values():
                door.unlock(permanent=False)
        elif command == 'lock':
            for door in doors.values():
                door.lock()

    for d in doors:
        doors[d].tick()