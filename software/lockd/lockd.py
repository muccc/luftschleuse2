#    This file is part of lockd, the daemon of the luftschleuse2 project.
#
#    See https://github.com/muccc/luftschleuse2 for more information.
#
#    Copyright (C) 2013 Tobias Schneider <schneider@muc.ccc.de> 
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
from display_controller import DisplayController
from displaylogic import DisplayLogic

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

    serial_interface = serialinterface.SerialInterface(serialdevice, baudrate, timeout=.1)

    input_queue = Queue.Queue()

    udpcommand = UDPCommand('127.0.0.1', 2323, input_queue)

    doors = {}

    master = None

    display = None
    
    display_controller = None

    logic = DoorLogic()

    for section in config.sections():
        if config.has_option(section, 'type'):
            t = config.get(section, 'type')
            if t == 'door':
                door_name = section
                logger.debug('Adding door "%s"'%door_name)
                door = Door(door_name, config_file, config, serial_interface, input_queue)
                door_address = config.get(door_name, 'address')
                doors[door_address] = door
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
            
            master = MasterController(serial_interface, input_queue, buttons, leds) 
        
        elif section == 'Display':
            display_type = config.get(section, 'display_type') 
            if display_type == "Nokia_1600":
                from display import Display
                display = Display(serial_interface)
            elif display_type == 'simulation':
                from display_pygame import Display
                display = Display()
            elif display_type == 'network':
                from display_network import Display
                display = Display()
            elif display_type == 'None':
                display = None
            else:
                logger.warning('Unknown display type "%s"', display_type)
        elif section == 'Status Receiver':
            host = config.get(section, 'host')
            port = int(config.get(section, 'port'))
            announcer = Announcer(host, port)
            logic.add_state_listener(announcer.update_state)
    
    if master == None:
        logger.error('Please specify a master controller')
        sys.exit(1)

    interface_logic = InterfaceLogic(master)
    logic.add_state_listener(interface_logic.update_state)
    
    if display != None:
        display_controller = DisplayController(display)
        display_logic = DisplayLogic(display_controller)
        logic.add_state_listener(display_logic.update_state)
        for door in doors.values():
            display_logic.add_door(door)

    else:
        logger.warning('No display specified.')

    input_queue.put({'origin_name': 'init',
                     'origin_type': DoorLogic.Origin.INTERNAL,
                     'input_name': '',
                     'input_type': DoorLogic.Input.COMMAND,
                     'input_value': 'down'})
    while True:
        timeout = False
        while not timeout:
            address, message = serial_interface.readMessage()
            if address != False:
                logger.debug('Received data from address %s :%s'%(address, str(list(message))))
            else:
                timeout = True

            if address in doors:
                doors[address].update(message)
            elif address == master.address:
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
        if display_controller != None:
            display_controller.tick()
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



