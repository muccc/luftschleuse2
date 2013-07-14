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
        data = "abcde"
        seq_sync = False
        p = packet.Packet(seq, cmd, data, seq_sync)
        message = p.toMessage()

        self.assertEquals(message, "\x55\xaa\x00\x00SabcdeSESAME")

    def test_decoding(self):
        message = "\xaa\x55\x00\x00TabcdeSESAME"
        p = packet.Packet.fromMessage(message)
        self.assertIsInstance(p, packet.Packet)
        self.assertEquals(p.seq, 0x55aa)
        self.assertEquals(p.cmd, ord('T'))
        self.assertEquals(p.data, 'abcde')
        self.assertEquals(p.seq_sync, False)

    def test_encoding_sync(self):
        seq = 0xaa55
        cmd = ord('S')
        data = "abcde"
        seq_sync = True
        p = packet.Packet(seq, cmd, data, seq_sync)
        message = p.toMessage()

        self.assertEquals(message, "\x55\xaa\x00\x00SabcdeSYNCME")

    def test_decoding_sync(self):
        message = "\xaa\x55\x00\x00TabcdeSYNCME"
        p = packet.Packet.fromMessage(message)
        self.assertIsInstance(p, packet.Packet)
        self.assertEquals(p.seq, 0x55aa)
        self.assertEquals(p.cmd, ord('T'))
        self.assertEquals(p.data, 'abcde')
        self.assertEquals(p.seq_sync, True)

    
    def test_bad_packet(self):
        message = "\xaa\x55\x00\x00TabcdeSYNCMe"
        p = packet.Packet.fromMessage(message)
        self.assertIsNone(p)
 
if __name__ == '__main__':
    unittest.main()
