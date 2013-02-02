from packet import Packet
from Crypto.Cipher import AES

class Door:
    def __init__(self, name, address, txseq, rxseq, key, interface):
        self.name = name
        self.address = address
        self.txseq = txseq
        self.rxseq = rxseq
        self.key = ''.join([chr(int(x)) for x in key.split()])
        self.cipher = AES.new(self.key, AES.MODE_ECB)
        self.interface = interface

    def is_open(self):
        return False

    def is_closed(self):
        return False

    def is_locked(self):
        return False

    def is_unlocked(self):
        return False

    def unlock(self, permanent):
        if permanent:
            p = Packet(seq=self.txseq, cmd=ord('D'), data='\x02')
        else:
            p = Packet(seq=self.txseq, cmd=ord('D'), data='\x01')

        msg = self.cipher.encrypt(p.toMessage())
        self.interface.writeMessage(self.address, msg)

    def lock(self):
        p = Packet(seq=self.txseq, cmd=ord('D'), data='\x00')
        msg = self.cipher.encrypt(p.toMessage())
        self.interface.writeMessage(self.address, msg)

    def update(self, message):
        message = self.cipher.decrypt(message)
        p = Packet.fromMessage(message)
        print p

