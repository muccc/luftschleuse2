import ConfigParser
import serialinterface
import sys
import time
import packet
from door import Door
from mastercontroller import MasterController
from command import UDPCommand
import Queue
import logging
import logging.handlers
import traceback
from announce import Announcer

config = ConfigParser.RawConfigParser()
config.read(sys.argv[1])

logger = logging.getLogger('logger')
logger.setLevel(logging.DEBUG)

logger_host = config.get('Logging', 'host')
logger_port =int( config.get('Logging', 'port'))

handler = logging.handlers.SysLogHandler(address = (logger_host, logger_port), facility=19)

logger.addHandler(handler)

logger.info('Starting lockd')

#add_custom_print_exception()

try:
#if 1:
    serialdevice = config.get('Master Controller', 'serialdevice')
    baudrate = config.get('Master Controller', 'baudrate')

    announcer = Announcer(False)

    ser = serialinterface.SerialInterface(serialdevice, baudrate, timeout=.1)
    command_queue = Queue.Queue()

    udpcommand = UDPCommand('127.0.0.1', 2323, command_queue)

    doors = {}

    master = None

    for section in config.sections():
        if config.has_option(section, 'type'):
            t = config.get(section, 'type')
            if t == 'door':
                name = section
                txseq = int(config.get(section, 'txsequence'))
                rxseq = int(config.get(section, 'rxsequence'))
                address = config.get(section, 'address')
                initial_unlock = config.get(section, 'inital_unlock')
                if initial_unlock == 'True':
                    initial_unlock = True
                else:
                    initial_unlock = False

                key = config.get(section, 'key')
                logger.debug('Adding door "%s"'%section)
                door = Door(name, address, txseq, rxseq, key, ser, initial_unlock)
                doors[address] = door
            else:
                logger.warning('Unknown entry type "%s"', t)
        elif section == 'Master Controller':
            txseq = int(config.get(section, 'txsequence'))
            rxseq = int(config.get(section, 'rxsequence'))
            key = config.get(section, 'key')
            master = MasterController('0', txseq, rxseq, key, ser, command_queue) 
    
    if master == None:
        logger.error('Please specify a master controller')
        sys.exit(1)

    command_queue.put('announce_closed')
    
    while True:
        timeout = False
        while not timeout:
            address, message = ser.readMessage()
            if address != False:
                logger.debug('Received data from address %s :%s'%(address, str(list(message))))
            else:
                timeout = True

            if address in doors:
                doors[address].update(message)
            elif address == '0':
                master.update(message)

        if not command_queue.empty():
            command = command_queue.get()
            logger.info("-------------Received new command: %s----------------"%command)
            if command == 'unlock':
                for door in [door for door in doors.values() if door.initial_unlock == True]:
                    logger.info("Unlocking %s"%door.name)
                    door.unlock(relock_timeout=5)
            elif command == 'lock':
                for door in doors.values():
                    door.lock()
                command_queue.put('announce_closed')
            elif command == 'announce_open':
                announcer.announce_open()
                master.announce_open()
            elif command == 'announce_closed':
                announcer.announce_closed()
                master.announce_closed()
            elif command == 'toggle_announce':
                if announcer.open:
                    command_queue.put('announce_closed')
                else:
                    command_queue.put('announce_open')
        for d in doors:
            doors[d].tick()
        master.tick()
        announcer.tick()
        
        all_locked = True
        for d in doors:
            if not doors[d].locked:
                all_locked = False
        if all_locked:
            logger.debug("All doors locked")
        else:
            logger.debug("NOT all doors locked")
        master.set_global_state(all_locked)

#try:
#    pass
except Exception, e:
    #logging.exception(e)
    logger.exception(e)
    import traceback
    traceback.print_exc()
    sys.exit(1)

sys.exit(0)



