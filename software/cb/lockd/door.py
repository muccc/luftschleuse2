from packet import Packet
from Crypto.Cipher import AES

class Door:
    DOOR_CLOSED        = (1<<0)
    LOCK_LOCKED        = (1<<1)
    LOCK_UNLOCKED      = (1<<2)
    LOCK_LOCKING       = (1<<3)
    LOCK_UNLOCKING     = (1<<4)
    HANDLE_PRESSED     = (1<<5)
    LOCK_PERM_UNLOCKED = (1<<6)


    def __init__(self, name, address, txseq, rxseq, key, interface):
        self.name = name
        self.address = address
        self.txseq = txseq
        self.rxseq = rxseq
        self.key = ''.join([chr(int(x)) for x in key.split()])
        self.cipher = AES.new(self.key, AES.MODE_ECB)
        self.interface = interface
        
        self.open = False
        self.closed = False
        self.locked = False
        self.unlocked = False

    def is_open(self):
        return self.open

    def is_closed(self):
        return self.closed

    def is_locked(self):
        return self.locked

    def is_unlocked(self):
        return self.unlocked

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
        if p.cmd==83:
            print [ord(x) for x in p.data]
            print 'Voltage=%.1f V'%(ord(p.data[3])*0.1)
            doorstate = ord(p.data[1])
            state = ''
            self.closed = doorstate & Door.DOOR_CLOSED \
                            == Door.DOOR_CLOSED
            self.locked = doorstate & Door.LOCK_LOCKED \
                            == Door.LOCK_LOCKED 
            self.unlocked = doorstate & Door.LOCK_UNLOCKED \
                            == Door.LOCK_UNLOCKED
            self.locking = doorstate & Door.LOCK_LOCKING \
                            == Door.LOCK_LOCKING
            self.unlocking = doorstate & Door.LOCK_UNLOCKING \
                            == Door.LOCK_UNLOCKING
            self.handle_pressed = doorstate & Door.HANDLE_PRESSED \
                            == Door.HANDLE_PRESSED
            self.perm_unlocked = doorstate & Door.LOCK_PERM_UNLOCKED \
                            == Door.LOCK_PERM_UNLOCKED

            print 'Door state:', self.get_state()

    def get_state(self):
        state = ''
        if self.closed:
            state += ' CLOSED'
        if self.locked:
            state += ' LOCKED'
        if self.unlocked:
            state += ' UNLOCKED'
        if self.locking:
            state += ' LOCKING'
        if self.unlocking:
            state += ' UNLOCKING'
        if self.handle_pressed:
            state += ' HANDLE_PRESSED'
        if self.perm_unlocked:
            state += ' PERM_UNLOCKED'
        return state.strip()
