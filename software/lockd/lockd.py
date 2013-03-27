import ConfigParser
import serialinterface
import sys
import time
import packet
from door import Door
from command import UDPCommand
import Queue
import logging
import logging.handlers
import traceback

'''
def add_custom_print_exception():
    old_print_exception = traceback.print_exception
    def custom_print_exception(etype, value, tb, limit=None, file=None):
        print 'foobar'
        tb_output = StringIO.StringIO()
        traceback.print_tb(tb, limit, tb_output)
        logger = logging.getLogger('logger')
        logger.error(tb_output.getvalue())
        tb_output.close()
        old_print_exception(etype, value, tb, limit=None, file=None)
    traceback.print_exception = custom_print_exception
'''

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
                logger.debug('Adding door "%s"'%section)
                door = Door(name, address, txseq, rxseq, key, ser)
                doors[address] = door
            else:
                logger.warning('Unknown entry type "%s"', t)
    seq = 0

    while True:
        address, message = ser.readMessage()
        if address in doors:
            logger.debug('Received data from address %d :'%address, str(list(message)))
            doors[address].update(message)

        if not command_queue.empty():
            command = command_queue.get()
            if command == 'unlock':
                for door in doors.values():
                    door.unlock(relock_timeout=50)
            elif command == 'lock':
                for door in doors.values():
                    door.lock()
        for d in doors:
            doors[d].tick()
#try:
#    pass
except Exception, e:
    #logging.exception(e)
    logger.exception(e)
    import traceback
    traceback.print_exc()
    sys.exit(1)

sys.exit(0)



