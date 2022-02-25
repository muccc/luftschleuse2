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
import configparser
import serialinterface
import sys
from door import Door
from mastercontroller import MasterController
from command import UDPCommand
import queue
import logging
import logging.handlers
import traceback
from doorlogic import DoorLogic
from userinterfacelogic import UserInterfaceLogic
from announce import Announcer
from display_controller import DisplayController
from displaylogic import DisplayLogic

class Lockd:
    def __init__(self, config):
        self.logger = logging.getLogger('logger')
        self.logger.info('Starting lockd')

        #add_custom_print_exception()

        serialdevice = config.get('Master Controller', 'serialdevice')
        baudrate = config.get('Master Controller', 'baudrate')

        self.serial_interface = serialinterface.SerialInterface(serialdevice, baudrate, timeout=.1)

        self.input_queue = queue.Queue()

        udpcommand = UDPCommand('127.0.0.1', 2323, self.input_queue)

        self.doors = {}

        self.master = None

        display = None
        
        self.display_controller = None

        self.logic = DoorLogic()

        for section in config.sections():
            if config.has_option(section, 'type'):
                t = config.get(section, 'type')
                if t == 'door':
                    door_name = section
                    self.logger.debug('Adding door "%s"'%door_name)
                    buttons = {1: 'manual_control', 2: 'bell_code'}
                    door = Door(door_name, config, self.serial_interface, self.input_queue, buttons)
                    door_address = bytes(config.get(door_name, 'address'), encoding="ascii")
                    self.doors[door_address] = door
                    self.logic.add_door(door)
                else:
                    self.logger.warning('Unknown entry type "%s"', t)
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
                
                self.master = MasterController(self.serial_interface, self.input_queue, buttons, leds) 
            
            elif section == 'Display':
                display_type = config.get(section, 'display_type') 
                max_update_rate = float(config.get(section, 'max_update_rate'))
                if display_type == "Nokia_1600":
                    from display import Display
                    display = Display(self.serial_interface)
                elif display_type == 'simulation':
                    from display_pygame import Display
                    display = Display()
                elif display_type == 'network':
                    from display_network import Display
                    display = Display()
                elif display_type == 'None':
                    display = None
                else:
                    self.logger.warning('Unknown display type "%s"', display_type)
            elif section == 'Status Receiver':
                host = config.get(section, 'host')
                port = int(config.get(section, 'port'))
                self.announcer = Announcer(host, port)
                self.logic.add_state_listener(self.announcer.update_state)
        
        if self.master == None:
            self.logger.error('Please specify a self.master controller')
            sys.exit(1)

        self.interface_logic = UserInterfaceLogic(self.master)
        self.logic.add_state_listener(self.interface_logic.update_state)
        
        if display != None:
            self.display_controller = DisplayController(display, max_update_rate)
            self.display_logic = DisplayLogic(self.display_controller)
            self.logic.add_state_listener(self.display_logic.update_state)
            for door in self.doors.values():
                self.display_logic.add_door(door)

        else:
            self.logger.warning('No display specified.')

        self.input_queue.put({'origin_name': 'init',
                        'origin_type': DoorLogic.Origin.INTERNAL,
                        'input_name': '',
                        'input_type': DoorLogic.Input.COMMAND,
                        'input_value': 'down'})

    def process(self):
        timeout = False
        while not timeout:
            address, message = self.serial_interface.readMessage()
            if address != False:
                self.logger.debug('Received data from address %s :%s' %
                        (address, str(list(message))))
            else:
                timeout = True

            if address in self.doors:
                self.doors[address].update(message)
            elif address == self.master.address:
                self.master.update(message)

        if not self.input_queue.empty():
            input = self.input_queue.get()
            self.logic.policy(input['origin_name'], input['origin_type'],
                        input['input_name'], input['input_type'], 
                        input['input_value'])
        for d in self.doors:
            self.doors[d].tick()
        self.master.tick()
        self.announcer.tick()
        self.interface_logic.tick()
        self.logic.tick()

        if self.display_controller != None:
            self.display_logic.tick()
            self.display_controller.tick()

if __name__ == '__main__':
    config = configparser.RawConfigParser()
    config_file = sys.argv[1]
    config.read(config_file)

    logger = logging.getLogger('logger')

    logger_level = config.get('Logging', 'level')
    if logger_level == 'debug':
        logger.setLevel(logging.DEBUG)
    elif logger_level == 'info':
        logger.setLevel(logging.INFO)
    elif logger_level == 'warning':
        logger.setLevel(logging.WARNING)
    elif logger_level == 'error':
        logger.setLevel(logging.ERROR)
    elif logger_level == 'fatal':
        logger.setLevel(logging.FATAL)
    else:
        logger.setLevel(logging.ERROR)

    logger_host = config.get('Logging', 'host')
    logger_port = int(config.get('Logging', 'port'))

    handler = logging.handlers.SysLogHandler(address = (logger_host, logger_port), facility=19)

    logger.addHandler(handler)

    try:
        lockd = Lockd(config)

        while True:
            lockd.process();
    #try:
    #    pass
    except Exception as e:
        #logging.exception(e)
        logger.exception(e)
        import traceback
        traceback.print_exc()
        sys.exit(1)

    sys.exit(0)


