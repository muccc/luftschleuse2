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

class MockLogger:
    def __init__(self):
        pass

    def debug(self, message):
        print("Debug: %s"%message)

    def info(self, message):
        print("Info: %s"%message)

    def warning(self, message):
        print("Warning: %s"%message)


class DoorTest(unittest.TestCase):
    def setUp(self):
        self.interface = mock.create_autospec(serialinterface.SerialInterface)
        self.interface.writeMessage = mock.MagicMock()
        self.input_queue = Queue.Queue()
        self.t0 = time.time()
        self.door_name = 'Door1'
        self.persisted_min_rx_seq_leap = 2**15

        (self.config_file, self.config_file_path) = tempfile.mkstemp()

        config_file = open(self.config_file_path, 'w')
        config_file.write("[Door1]\r\nrx_sequence = 128\r\n")
        config_file.close()

        config = ConfigParser.RawConfigParser()
        config.add_section(self.door_name)
        self.persisted_min_rx_seq = 128
        config.set("Door1", "rx_sequence", self.persisted_min_rx_seq)
        config.set("Door1", "rx_sequence_leap", self.persisted_min_rx_seq_leap)
        config.set("Door1", "inital_unlock", True)
        config.set("Door1", "address", 'A')
        config.set("Door1", "key", '0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0')
        self.key = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]


        self.buttons = {1: 'Button0', 2: 'Button1'}

        self.packet_press   = packet.Packet(0, ord('S'), '\x01\x00\x00\x00\x00', False)
        self.packet_release = packet.Packet(0, ord('S'), '\x00\x00\x00\x00\x00', False)
        self.packet_unlocking = packet.Packet(0, ord('S'), '\x00\x10\x00\x00\x00', False)
        self.packet_perm_unlocked = packet.Packet(0, ord('S'), '\x00\x40\x00\x00\x00', False)
        self.packet_locked = packet.Packet(0, ord('S'), '\x00\x02\x00\x00\x00', False)


        self.door = door.Door('Door1', self.config_file_path,
                                config, self.interface, self.input_queue,
                                self.buttons)
        #self.door.logger = MockLogger()
    def tearDown(self):
        os.remove(self.config_file_path)

    def test_constructor(self):
        self.assertIsInstance(self.door, door.Door)

    def do_accept(self, packet, seq):
        packet.seq = seq
        self.door.update(packet.toMessage(self.key))
        return self.input_queue.get(block = False)

    def do_accept_test(self, packet, seq):
        packet.seq = seq
        self.assertTrue(self.door.update(packet.toMessage(self.key)))
        self.assertTrue(not self.input_queue.empty(), "Message with seq nr %d was not accepted" % seq)
        return self.input_queue.get(block = False)

    def do_not_accept(self, packet, seq):
        packet.seq = seq
        self.door.update(packet.toMessage(self.key))
    
    def do_not_accept_test(self, packet, seq):
        packet.seq = seq
        self.assertFalse(self.door.update(packet.toMessage(self.key)))
        self.assertTrue(self.input_queue.empty(), "Message with seq nr %d was accepted" % seq)
 
    def test_rx_sequence_number_normal(self):
        self.do_accept_test(self.packet_press, self.persisted_min_rx_seq)
        self.do_accept_test(self.packet_release, self.persisted_min_rx_seq + 1)

    def test_rx_sequence_number_too_small(self):
        self.do_not_accept_test(self.packet_press, self.persisted_min_rx_seq - 1)
        self.do_not_accept_test(self.packet_release, self.persisted_min_rx_seq - 2)

    def test_rx_sequence_number_must_increase(self):
        self.do_accept(self.packet_press, self.persisted_min_rx_seq + 128)
        self.do_not_accept_test(self.packet_release, self.persisted_min_rx_seq + 128)
        self.do_not_accept_test(self.packet_release, self.persisted_min_rx_seq + 50)

    def test_rx_sequence_number_can_jump(self):
        self.do_accept_test(self.packet_press, self.persisted_min_rx_seq + 50)
        self.do_accept_test(self.packet_release, self.persisted_min_rx_seq + 128)
        self.do_accept_test(self.packet_press, self.persisted_min_rx_seq + 2**15)
        self.do_accept_test(self.packet_release, self.persisted_min_rx_seq + 2**16)
        self.do_accept_test(self.packet_press, self.persisted_min_rx_seq + 2**16+23)

    def test_same_packet_increases_seq_number(self):
        self.do_accept(self.packet_press, self.persisted_min_rx_seq + 50)
        self.do_not_accept(self.packet_press, self.persisted_min_rx_seq + 128)
        self.do_not_accept_test(self.packet_release, self.persisted_min_rx_seq + 128)

    def test_button_press(self):
        input = self.do_accept(self.packet_press, self.persisted_min_rx_seq)
        self.assertEquals(input, {
            'origin_name': 'Door1',
            'origin_type': DoorLogic.Origin.DOOR,
            'input_name': 'Button0',
            #'input_name': 'Button0',
            'input_type': DoorLogic.Input.BUTTON,
            'input_value': True})

    def test_button_release(self):
        self.do_accept(self.packet_press, self.persisted_min_rx_seq)
        input = self.do_accept(self.packet_release, self.persisted_min_rx_seq + 1)
        self.assertEquals(input, {
            'origin_name': 'Door1',
            'origin_type': DoorLogic.Origin.DOOR,
            'input_name': 'Button0',
            #'input_name': 'Button0',
            'input_type': DoorLogic.Input.BUTTON,
            'input_value': False})


    def test_seq_sync_message_tx(self):
        self.do_accept(self.packet_press, self.persisted_min_rx_seq + 50)
        self.do_not_accept(self.packet_release, self.persisted_min_rx_seq + 30)

        p = packet.Packet(self.persisted_min_rx_seq + 51, 0, '\x00\x00\x00\x00\x00', True)
        self.interface.writeMessage.assert_called_with('A', p.toMessage(self.key))
 
    @patch('time.time')
    def test_seq_sync_message_rx(self, time_mock):
        p = packet.Packet(123, 0, '\x00\x00\x00\x00\x00', True)
        self.door.update(p.toMessage(self.key))
        time_mock.return_value = self.t0 + 3
        self.door.tick()
        query = packet.Packet(123, ord('D'), '\x02\x00\x00\x00\x00', False)
        self.interface.writeMessage.assert_called_once_with('A', query.toMessage(self.key))
    
    def test_persist_sequence_number_normal(self):
        self.do_accept(self.packet_press, self.persisted_min_rx_seq)
        parser = ConfigParser.RawConfigParser()
        parser.read(self.config_file_path)
        min_rx_seq = int(parser.get(self.door_name, 'rx_sequence'))
        self.assertEqual(min_rx_seq, self.persisted_min_rx_seq + self.persisted_min_rx_seq_leap)

    def test_persist_sequence_number_small_leap(self):
        self.do_accept(self.packet_press, self.persisted_min_rx_seq + 50)
        parser = ConfigParser.RawConfigParser()
        parser.read(self.config_file_path)
        min_rx_seq = int(parser.get(self.door_name, 'rx_sequence'))
        self.assertEqual(min_rx_seq, self.persisted_min_rx_seq + 50 + self.persisted_min_rx_seq_leap)

    def test_persist_sequence_number_large_leap(self):
        self.do_accept(self.packet_press, self.persisted_min_rx_seq + 2**17)
        parser = ConfigParser.RawConfigParser()
        parser.read(self.config_file_path)
        min_rx_seq = int(parser.get(self.door_name, 'rx_sequence'))
        self.assertEqual(min_rx_seq, self.persisted_min_rx_seq + 2**17 + self.persisted_min_rx_seq_leap)

    def test_no_persist_sequence_number_normal(self):
        self.do_accept(self.packet_press, self.persisted_min_rx_seq)
        self.do_accept(self.packet_release, self.persisted_min_rx_seq + 1)
        parser = ConfigParser.RawConfigParser()
        parser.read(self.config_file_path)
        min_rx_seq = int(parser.get(self.door_name, 'rx_sequence'))
        self.assertEqual(min_rx_seq, self.persisted_min_rx_seq + self.persisted_min_rx_seq_leap)

    def test_no_persist_sequence_number_small_leap(self):
        self.do_accept(self.packet_press, self.persisted_min_rx_seq + 50)
        self.do_accept(self.packet_release, self.persisted_min_rx_seq + 50 + 1)
        parser = ConfigParser.RawConfigParser()
        parser.read(self.config_file_path)
        min_rx_seq = int(parser.get(self.door_name, 'rx_sequence'))
        self.assertEqual(min_rx_seq, self.persisted_min_rx_seq + 50 + self.persisted_min_rx_seq_leap)

    def test_no_persist_sequence_number_large_leap(self):
        self.do_accept(self.packet_press, self.persisted_min_rx_seq + 2**17)
        self.do_accept(self.packet_release, self.persisted_min_rx_seq + 2**17 + 1)
        parser = ConfigParser.RawConfigParser()
        parser.read(self.config_file_path)
        min_rx_seq = int(parser.get(self.door_name, 'rx_sequence'))
        self.assertEqual(min_rx_seq, self.persisted_min_rx_seq + 2**17 + self.persisted_min_rx_seq_leap)


    @patch('time.time')
    def test_tick(self, time_mock):
        query = packet.Packet(0, ord('D'), '\x02\x00\x00\x00\x00', False)
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
        query = packet.Packet(1, ord('D'), '\x02\x00\x00\x00\x00', False)
        self.interface.writeMessage.assert_called_with('A', query.toMessage(self.key))
        self.assertEqual(self.interface.writeMessage.call_count, 2)

    @patch('time.time')
    def test_initial_door_state(self, time_mock):
        time_mock.return_value = self.t0 + 10
        self.door.tick()
        # 0x02 is LOCKED
        query = packet.Packet(0, ord('D'), '\x02\x00\x00\x00\x00', False)
        self.interface.writeMessage.assert_called_with('A', query.toMessage(self.key))

    @patch('time.time')
    def test_unlock(self, time_mock):
        time_mock.return_value = self.t0
        self.door.unlock()

        time_mock.return_value = self.t0 + 10
        self.door.tick()
        # 0x04 is UNLOCKED
        query = packet.Packet(0, ord('D'), '\x04\x00\x00\x00\x00', False)
        self.interface.writeMessage.assert_called_with('A', query.toMessage(self.key))
 
    @patch('time.time')
    def test_lock(self, time_mock):
        time_mock.return_value = self.t0
        self.door.unlock()
        time_mock.return_value = self.t0 + 10
        self.door.tick()
       
        self.door.lock()
        time_mock.return_value = self.t0 + 20
        self.door.tick()
        # 0x02 is LOCKED
        query = packet.Packet(1, ord('D'), '\x02\x00\x00\x00\x00', False)
        self.interface.writeMessage.assert_called_with('A', query.toMessage(self.key))
 
    @patch('time.time')
    def test_timed_relock(self, time_mock):
        time_mock.return_value = self.t0
        self.door.unlock(20)

        time_mock.return_value = self.t0 + 10
        self.door.tick()
        # 0x04 is UNLOCKED
        query = packet.Packet(0, ord('D'), '\x04\x00\x00\x00\x00', False)
        self.interface.writeMessage.assert_called_with('A', query.toMessage(self.key))
        
        time_mock.return_value = self.t0 + 21
        self.door.tick()
        # 0x02 is LOCKED
        query = packet.Packet(1, ord('D'), '\x02\x00\x00\x00\x00', False)
        self.interface.writeMessage.assert_called_with('A', query.toMessage(self.key))
       
    def test_status_callback(self):
        callback = mock.MagicMock()
        self.door.add_state_listener(callback)
        self.do_not_accept(self.packet_unlocking, self.persisted_min_rx_seq)
        callback.assert_called_with(self.door)

    def test_status_update_locked(self):
        self.do_not_accept(self.packet_locked, self.persisted_min_rx_seq)
        self.assertTrue(self.door.is_locked())

    def test_status_update_perm_unlocked(self):
        self.do_not_accept(self.packet_perm_unlocked, self.persisted_min_rx_seq)
        self.assertTrue(self.door.is_perm_unlocked())
    


if __name__ == '__main__':
    unittest.main()
