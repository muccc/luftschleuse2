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
from doorlogic import DoorLogic
from interfacelogic import InterfaceLogic
from announce import Announcer

config = ConfigParser.RawConfigParser()
config_file = sys.argv[1]
config.read(config_file)

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

    ser = serialinterface.SerialInterface(serialdevice, baudrate, timeout=.1)
    input_queue = Queue.Queue()

    udpcommand = UDPCommand('127.0.0.1', 2323, input_queue)

    doors = {}

    master = None

    logic = DoorLogic()

    announcer = Announcer()

    logic.add_state_listener(announcer.update_state)

    for section in config.sections():
        if config.has_option(section, 'type'):
            t = config.get(section, 'type')
            if t == 'door':
                name = section
                logger.debug('Adding door "%s"'%name)
                door = Door(name, config_file, config, ser, input_queue)
                address = config.get(name, 'address')
                doors[address] = door
                logic.add_door(door)
            else:
                logger.warning('Unknown entry type "%s"', t)
        elif section == 'Master Controller':
            #txseq = int(config.get(section, 'txsequence'))
            #rxseq = int(config.get(section, 'rxsequence'))
            #key = config.get(section, 'key')
            
            buttons_section = 'Master Controller Buttons'
            buttons = {}
            for button_name in config.options(buttons_section):
                button_pin = int(config.get(buttons_section, button_name))
                buttons[button_pin] = button_name

            leds_section = 'Master Controller LEDs'
            leds = {}
            for led_name in config.options(leds_section):
                led_pin = int(config.get(leds_section, led_name))
                leds[led_name] = led_pin


            master = MasterController('0', ser, input_queue, buttons, leds) 
    
    if master == None:
        logger.error('Please specify a master controller')
        sys.exit(1)

    interface_logic = InterfaceLogic(master)
    logic.add_state_listener(interface_logic.update_state)

    input_queue.put({'origin_name': 'init',
                     'origin_type': DoorLogic.Origin.INTERNAL,
                     'input_name': '',
                     'input_type': DoorLogic.Input.COMMAND,
                     'input_value': 'down'})
    f = open(sys.argv[1],'w');
    config.write(f);
    f.close()

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

        if not input_queue.empty():
            input = input_queue.get()
            logic.policy(input['origin_name'], input['origin_type'],
                         input['input_name'], input['input_type'], 
                         input['input_value'])
        for d in doors:
            doors[d].tick()
        master.tick()
        announcer.tick()
        interface_logic.tick()
        logic.tick()
        ''' 
        all_locked = True
        for d in doors:
            if not doors[d].locked:
                all_locked = False
        if all_locked:
            logger.debug("All doors locked")
        else:
            logger.debug("NOT all doors locked")
        '''
#try:
#    pass
except Exception, e:
    #logging.exception(e)
    logger.exception(e)
    import traceback
    traceback.print_exc()
    sys.exit(1)

sys.exit(0)



