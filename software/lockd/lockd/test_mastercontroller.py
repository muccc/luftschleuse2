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
import random
import unittest
import queue
import mock
import time
from mock import patch

from . import mastercontroller
from . import serialinterface
from . import packet
from .doorlogic import DoorLogic

class MasterControllerTest(unittest.TestCase):
    def setUp(self):
        self.interface = mock.create_autospec(serialinterface.SerialInterface)
        self.interface.writeMessage = mock.MagicMock()
        self.input_queue = queue.Queue()
        self.buttons = {1: 'Button0', 2: 'Button1', 4: 'Button2'}
        self.leds = {'LED0': 0, 'LED1': 1}
        self.master = mastercontroller.MasterController(self.interface,
                    self.input_queue, self.buttons, self.leds)
        self.t0 = time.time()

    def test_constructor(self):
        self.assertIsInstance(self.master, mastercontroller.MasterController)
    
    def test_button_press(self):
        p = packet.Packet(0, ord('S'), b'\x01\x00\x00\x00\x00', False)
        self.master.update(p.toMessage())
        input = self.input_queue.get(block = False)
        self.assertEqual(input, {
            'origin_name': 'master',
            'origin_type': DoorLogic.Origin.CONTROL_PANNEL,
            'input_name': 'Button0',
            'input_type': DoorLogic.Input.BUTTON,
            'input_value': True})

        p = packet.Packet(0, ord('S'), b'\x05\x00\x00\x00\x00', False)
        self.master.update(p.toMessage())
        input = self.input_queue.get(block = False)
        self.assertEqual(input, {
            'origin_name': 'master',
            'origin_type': DoorLogic.Origin.CONTROL_PANNEL,
            'input_name': 'Button2',
            'input_type': DoorLogic.Input.BUTTON,
            'input_value': True})

        p = packet.Packet(0, ord('S'), b'\x04\x00\x00\x00\x00', False)
        self.master.update(p.toMessage())
        input = self.input_queue.get(block = False)
        self.assertEqual(input, {
            'origin_name': 'master',
            'origin_type': DoorLogic.Origin.CONTROL_PANNEL,
            'input_name': 'Button0',
            'input_type': DoorLogic.Input.BUTTON,
            'input_value': False})

    def test_leds(self):
        self.master.set_led('LED0', self.master.LedState.OFF)
        p = packet.Packet(0, ord('L'), b'\x00\x01\x00\x00\x00', False)
        self.interface.writeMessage.assert_called_with(self.master.priority, b'0', p.toMessage(), self.master.tx_msg_queue)

        self.master.set_led('LED1', self.master.LedState.OFF)
        p = packet.Packet(0, ord('L'), b'\x01\x01\x00\x00\x00', False)
        self.interface.writeMessage.assert_called_with(self.master.priority, b'0', p.toMessage(), self.master.tx_msg_queue)

        self.master.set_led('LED1', self.master.LedState.ON)
        p = packet.Packet(0, ord('L'), b'\x01\x00\x00\x00\x00', False)
        self.interface.writeMessage.assert_called_with(self.master.priority, b'0', p.toMessage(), self.master.tx_msg_queue)
    
    @patch('time.time')
    def test_tick(self, time_mock):
        query = packet.Packet(0, ord('S'), b'\x00\x00\x00\x00\x00', False)
        time_mock.return_value = self.t0
        self.master.tick()
        time_mock.return_value = self.t0 + 0.3
        self.master.tick()
        self.interface.writeMessage.assert_not_called()
        time_mock.return_value = self.t0 + .6
        self.master.tick()
        self.interface.writeMessage.assert_called_once_with(self.master.priority, b'0', query.toMessage(), self.master.tx_msg_queue)
        time_mock.return_value = self.t0 + .9
        self.master.tick()
        self.interface.writeMessage.assert_called_once_with(self.master.priority, b'0', query.toMessage(), self.master.tx_msg_queue)
        time_mock.return_value = self.t0 + 1.3
        self.master.tick()
        self.assertEqual(self.interface.writeMessage.call_count, 2)


if __name__ == '__main__':
    unittest.main()
