import random
import unittest
import door
import Queue
import mock
import serialinterface
import packet
from doorlogic import DoorLogic
import time
from mock import patch
import tempfile
import os
import ConfigParser

class DoorTest(unittest.TestCase):
    def setUp(self):
        self.interface = mock.create_autospec(serialinterface.SerialInterface)
        self.interface.writeMessage = mock.MagicMock()
        self.input_queue = Queue.Queue()
        self.t0 = time.time()
        (self.config_file, self.config_file_path) = tempfile.mkstemp()

        self.config_file = os.fdopen(self.config_file, 'w')
        self.config_file.write("[Door1]\r\nrx_sequence = 128\r\n")
        self.config_file.close()

        config = ConfigParser.RawConfigParser()
        config.add_section("Door1")
        config.set("Door1", "rx_sequence", 128)
        config.set("Door1", "inital_unlock", True)
        config.set("Door1", "address", 'A')
        config.set("Door1", "key", '0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0')
        self.key = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]

        self.door = door.Door('Door1', self.config_file_path,
                                config, self.interface,

                                self.input_queue)

    def tearDown(self):
        os.remove(self.config_file_path)

    def test_constructor(self):
        self.assertIsInstance(self.door, door.Door)
    
    def test_button_press(self):
        p = packet.Packet(129, ord('S'), '\x01\x00\x00\x00\x00', False)
        self.door.update(p.toMessage(self.key))
        input = self.input_queue.get(block = False)
        self.assertEquals(input, {
            'origin_name': 'Door1',
            'origin_type': DoorLogic.Origin.DOOR,
            'input_name': 'manual_control',
            #'input_name': 'Button0',
            'input_type': DoorLogic.Input.BUTTON,
            'input_value': ''})
            #'input_value': True})

    def test_out_of_sync_packet(self):
        p = packet.Packet(10, ord('S'), '\x01\x00\x00\x00\x00', False)
        self.door.update(p.toMessage(self.key))

        p = packet.Packet(128, 0, '\x00\x00\x00\x00\x00', True)
        self.interface.writeMessage.assert_called_once_with('A', p.toMessage(self.key))
    
    @patch('time.time')
    def test_update_sync(self, time_mock):
        p = packet.Packet(123, 0, '\x00\x00\x00\x00\x00', True)
        self.door.update(p.toMessage(self.key))
        time_mock.return_value = self.t0 + 3
        self.door.tick()
        query = packet.Packet(124, ord('D'), '\x02\x00\x00\x00\x00', False)
        self.interface.writeMessage.assert_called_once_with('A', query.toMessage(self.key))
        
    def test_leds(self):
        #self.door.set_led('LED0', self.door.LedState.OFF)
        #p = packet.Packet(0, ord('L'), '\x00\x01\x00\x00\x00', False)
        #self.interface.writeMessage.assert_called_with('0', p.toMessage())
        pass

    @patch('time.time')
    def test_tick(self, time_mock):
        query = packet.Packet(1, ord('D'), '\x02\x00\x00\x00\x00', False)
        time_mock.return_value = self.t0
        self.door.tick()
        time_mock.return_value = self.t0 + 0.3
        self.door.tick()
        self.interface.writeMessage.assert_not_called()
        time_mock.return_value = self.t0 + 1.6
        self.door.tick()
        self.interface.writeMessage.assert_called_once_with('A', query.toMessage(self.key))
        time_mock.return_value = self.t0 + 1.8
        self.door.tick()
        self.interface.writeMessage.assert_called_once_with('A', query.toMessage(self.key))
        time_mock.return_value = self.t0 + 2
        self.door.tick()
        #query = packet.Packet(2, ord('D'), '\x02\x00\x00\x00\x00', False)
        self.assertEqual(self.interface.writeMessage.call_count, 2)


if __name__ == '__main__':
    unittest.main()
