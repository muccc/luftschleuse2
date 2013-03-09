from packet import Packet
from Crypto.Cipher import AES
import time

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
        self.supply_voltage = 0
        self.command_time = 0
        self.command_accepted = None
        self.command = None

    def unlock(self, permanent):
        if permanent:
            self._send_command(command=ord('D'), data='\x02')
        else:
            self._send_command(command=ord('D'), data='\x01')
    def lock(self):
        self._send_command(command=ord('D'), data='\x00')

    def update(self, message):
        message = self.cipher.decrypt(message)
        print list(message)
        p = Packet.fromMessage(message)
        if p.cmd==83:
            self.supply_voltage = ord(p.data[3])*0.1
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
        elif p.cmd==ord('A'):
            accepted = ord(p.data[0]) == 1
            if not self.command_accepted:
                if accepted:
                    print 'Command at %d was accepted', self.command_time
                    self.command_accepted = True
                else:
                    print 'Command at %d was NOT accepted', self.command_time

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
        state = state.strip()
        state = state + ' Voltage=%.1f V'%self.supply_voltage
        return state

    def tick(self):
        return
        if time.time() - self.command_time > 5:
            if self.command_accepted == False:
                print 'Error: Command at %d was not accepted!'
            elif self.command_accepted == None:
                print 'Error: Command was not received'

    def _send_command(self, command, data):
        p = Packet(seq=self.txseq, cmd=command, data=data)
        msg = self.cipher.encrypt(p.toMessage())
        self.interface.writeMessage(self.address, msg)
        self.command_accepted = None
        self.command_time = time.time()


