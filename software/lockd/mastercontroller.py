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
from packet import Packet
#import apptime as time
import time
import logging
from doorlogic import DoorLogic
import queue

class MasterController:
    class LedState:
        ON = 0
        OFF = 1
        BLINK_FAST = 2
        BLINK_SLOW = 3
        FLASH = 4

    def __init__(self, interface, input_queue, buttons, leds):
        self.address = b'0'

        self.interface = interface

        self.supply_voltage = 0
        self.logger = logging.getLogger('logger')
        self.pressed_buttons = 0

        self.buttons = buttons
        self.leds = leds

        self.input_queue = input_queue

        self.timestamp = time.time()

        self.priority = 50

        self.tx_msg_queue = queue.Queue()

    def update(self, message):
        if len(message) != 16:
            self.logger.warning("The received message is not 16 bytes long")
            return

        #self.logger.debug("Decoded message: %s"%str(list(message)))

        p = Packet.fromMessage(message)
        if p.cmd == ord('S'):
            self.supply_voltage = p.data[3]*0.1

            pressed_buttons = p.data[0]
            self.logger.debug('master: pressed_buttons = %d', pressed_buttons)
            for pin in self.buttons:
                if pressed_buttons & pin and not self.pressed_buttons & pin:
                    self.pressed_buttons |= pin
                    self.input_queue.put({'origin_name': 'master',
                                          'origin_type': DoorLogic.Origin.CONTROL_PANNEL,
                                          'input_name': self.buttons[pin],
                                          'input_type': DoorLogic.Input.BUTTON,
                                          'input_value': True})
                elif not pressed_buttons & pin and self.pressed_buttons & pin:
                    self.input_queue.put({'origin_name': 'master',
                                          'origin_type': DoorLogic.Origin.CONTROL_PANNEL,
                                          'input_name': self.buttons[pin],
                                          'input_type': DoorLogic.Input.BUTTON,
                                          'input_value': False})
                    self.pressed_buttons &= ~pin

            self.logger.info('Master state: %s'%self.get_state())

    def get_state(self):
        state = ''
        state = state + ' Voltage=%.1f V'%self.supply_voltage
        state = state.strip()
        return state

    def tick(self):
        #self.logger.debug('master: tick')
        if time.time() - self.timestamp > .5:
            self._send_command(ord('S'), b'')
            self.timestamp = time.time()

    def set_led(self, led_name, state):
        led = self.leds[led_name]
        self._send_command(ord('L'), b'%c%c'%(led, state))

    def _send_command(self, command, data):
        p = Packet(seq=0, cmd=command, data=data, seq_sync=False)
        msg = p.toMessage()

        self.logger.debug('Msg to mastercontroller: %s'%list(msg))
        self.interface.writeMessage(self.priority, self.address, msg, self.tx_msg_queue)

