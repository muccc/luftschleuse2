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
from struct import pack, unpack
import logging
from Crypto.Cipher import AES

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
    magic = b'SESAME'
    sync_magic = b'SYNCME'

    def __init__(self, seq, cmd, data, seq_sync):
        self.seq = seq
        self.cmd = cmd
        self.data = data
        self.seq_sync = seq_sync
    
    @classmethod
    def fromMessage(cls, message, key = None):
        if key is not None:
            aes = AES.new(bytes(key), AES.MODE_ECB)
            message = aes.decrypt(message)
            logging.getLogger('logger').debug("Decoded message: %s"%str(list(message)))
           
        seq, cmd, data, magic = unpack(cls.msgformat, message[0:16])
        if len(message) > 16:
            print('Warning: Discarded %d bytes of data' % len(message)-16)
        if magic == b'SESAME':
            return cls(seq, cmd, data, False)
        if magic == b'SYNCME':
            return cls(seq, cmd, data, True)
        else:
            return None

    def toMessage(self, key = None):
        if self.seq_sync:
            magic = self.sync_magic
        else:
            magic = self.magic
        message = pack(self.msgformat, self.seq, self.cmd, self.data, magic)
        if key is not None:
            aes = AES.new(bytes(key), AES.MODE_ECB)
            message = aes.encrypt(message)

        return message

    def __str__(self):
        return "Packet: seq=%d, cmd=%c, data=%s"%(self.seq, self.cmd, self.data)

