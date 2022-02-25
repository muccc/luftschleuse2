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
import packet

class PacketTest(unittest.TestCase):

    def setUp(self):
        pass

    def test_constructor(self):
        seq = None
        cmd = None
        data = None
        seq_sync = False
        self.assertIsInstance(packet.Packet(seq, cmd, data, seq_sync), packet.Packet)

    def test_encoding(self):
        seq = 0xaa55
        cmd = ord('S')
        data = b"abcde"
        seq_sync = False
        p = packet.Packet(seq, cmd, data, seq_sync)
        message = p.toMessage()

        self.assertEqual(message, b"\x55\xaa\x00\x00SabcdeSESAME")

    def test_decoding(self):
        message = b"\xaa\x55\x00\x00TabcdeSESAME"
        p = packet.Packet.fromMessage(message)
        self.assertIsInstance(p, packet.Packet)
        self.assertEqual(p.seq, 0x55aa)
        self.assertEqual(p.cmd, ord('T'))
        self.assertEqual(p.data, b'abcde')
        self.assertEqual(p.seq_sync, False)

    def test_decoding_encrypted(self):
        message = bytes.fromhex("01aebf56f797f8fca488a572386a8add")
        p = packet.Packet.fromMessage(message,
                key = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0])
 
        self.assertIsInstance(p, packet.Packet)
        self.assertEqual(p.seq, 0x55aa)
        self.assertEqual(p.cmd, ord('T'))
        self.assertEqual(p.data, b'abcde')
        self.assertEqual(p.seq_sync, False)

    def test_encoding_sync(self):
        seq = 0xaa55
        cmd = ord('S')
        data = b"abcde"
        seq_sync = True
        p = packet.Packet(seq, cmd, data, seq_sync)

        message = p.toMessage()
        self.assertEqual(message, b"\x55\xaa\x00\x00SabcdeSYNCME")

    def test_encoding_sync_encrypted(self):
        seq = 0xaa55
        cmd = ord('S')
        data = b"abcde"
        seq_sync = True
        p = packet.Packet(seq, cmd, data, seq_sync)

        message = p.toMessage(key = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0])
        self.assertEqual(message,
                bytes.fromhex("ec7d816b5c120fb04ce7a4c4f579207d"))

    def test_decoding_sync(self):
        message = b"\xaa\x55\x00\x00TabcdeSYNCME"
        p = packet.Packet.fromMessage(message)
        self.assertIsInstance(p, packet.Packet)
        self.assertEqual(p.seq, 0x55aa)
        self.assertEqual(p.cmd, ord('T'))
        self.assertEqual(p.data, b'abcde')
        self.assertEqual(p.seq_sync, True)
    
    def test_bad_packet(self):
        message = b"\xaa\x55\x00\x00TabcdeSYNCMe"
        p = packet.Packet.fromMessage(message)
        self.assertIsNone(p)

 
if __name__ == '__main__':
    unittest.main()
