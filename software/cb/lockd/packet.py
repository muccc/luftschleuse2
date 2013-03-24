from struct import pack, unpack

class Packet:
    '''
    Data: 16 Byte AES block

    0   4   SEQ
    4   1   CMD
    5   5   DATA
    10  6   "SESAME"
    '''
    # The AVR is a little endian device
    msgformat = "<IB5s6s"
    magic = 'SESAME'

    def __init__(self, seq, cmd, data):
        self.seq = seq
        self.cmd = cmd
        self.data = data
    
    @classmethod
    def fromMessage(cls, message):
        seq, cmd, data, magic = unpack(cls.msgformat, message[0:16])
        if len(message) > 16:
            print 'Warning: Discarded %d bytes of data'%(len(message)-16)
        if magic == 'SESAME':
            return cls(seq, cmd, data)
        else:
            return None

    def toMessage(self):
        message = pack(self.msgformat, self.seq, self.cmd, self.data, self.magic)
        return message

    def __str__(self):
        return "Packet: seq=%d, cmd=%c, data=%s"%(self.seq, self.cmd, self.data)

