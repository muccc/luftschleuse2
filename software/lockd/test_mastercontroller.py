import random
import unittest
import mastercontroller
import Queue
import mock
import serialinterface
import packet
from doorlogic import DoorLogic
import time
from mock import patch

class MasterControllerTest(unittest.TestCase):
    def setUp(self):
        self.interface = mock.create_autospec(serialinterface.SerialInterface)
        self.interface.writeMessage = mock.MagicMock()
        self.input_queue = Queue.Queue()
        self.buttons = {1: 'Button0', 2: 'Button1', 4: 'Button2'}
        self.leds = {'LED0': 0, 'LED1': 1}
        self.master = mastercontroller.MasterController(self.interface,
                    self.input_queue, self.buttons, self.leds)
        self.t0 = time.time()

    def test_constructor(self):
        self.assertIsInstance(self.master, mastercontroller.MasterController)
    
    def test_button_press(self):
        p = packet.Packet(0, ord('S'), '\x01\x00\x00\x00\x00', False)
        self.master.update(p.toMessage())
        input = self.input_queue.get(block = False)
        self.assertEquals(input, {
            'origin_name': 'master',
            'origin_type': DoorLogic.Origin.CONTROL_PANNEL,
            'input_name': 'Button0',
            'input_type': DoorLogic.Input.BUTTON,
            'input_value': True})

        p = packet.Packet(0, ord('S'), '\x05\x00\x00\x00\x00', False)
        self.master.update(p.toMessage())
        input = self.input_queue.get(block = False)
        self.assertEquals(input, {
            'origin_name': 'master',
            'origin_type': DoorLogic.Origin.CONTROL_PANNEL,
            'input_name': 'Button2',
            'input_type': DoorLogic.Input.BUTTON,
            'input_value': True})

        p = packet.Packet(0, ord('S'), '\x04\x00\x00\x00\x00', False)
        self.master.update(p.toMessage())
        input = self.input_queue.get(block = False)
        self.assertEquals(input, {
            'origin_name': 'master',
            'origin_type': DoorLogic.Origin.CONTROL_PANNEL,
            'input_name': 'Button0',
            'input_type': DoorLogic.Input.BUTTON,
            'input_value': False})

    def test_leds(self):
        self.master.set_led('LED0', self.master.LedState.OFF)
        p = packet.Packet(0, ord('L'), '\x00\x01\x00\x00\x00', False)
        self.interface.writeMessage.assert_called_with('0', p.toMessage())

        self.master.set_led('LED1', self.master.LedState.OFF)
        p = packet.Packet(0, ord('L'), '\x01\x01\x00\x00\x00', False)
        self.interface.writeMessage.assert_called_with('0', p.toMessage())

        self.master.set_led('LED1', self.master.LedState.ON)
        p = packet.Packet(0, ord('L'), '\x01\x00\x00\x00\x00', False)
        self.interface.writeMessage.assert_called_with('0', p.toMessage())
    
    @patch('time.time')
    def test_tick(self, time_mock):
        query = packet.Packet(0, ord('S'), '\x00\x00\x00\x00\x00', False)
        time_mock.return_value = self.t0
        self.master.tick()
        time_mock.return_value = self.t0 + 0.3
        self.master.tick()
        self.interface.writeMessage.assert_not_called()
        time_mock.return_value = self.t0 + .6
        self.master.tick()
        self.interface.writeMessage.assert_called_once_with('0', query.toMessage())
        time_mock.return_value = self.t0 + .9
        self.master.tick()
        self.interface.writeMessage.assert_called_once_with('0', query.toMessage())
        time_mock.return_value = self.t0 + 1.3
        self.master.tick()
        self.assertEqual(self.interface.writeMessage.call_count, 2)


if __name__ == '__main__':
    unittest.main()
