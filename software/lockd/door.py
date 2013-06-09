from packet import Packet
import time
from aes import AES
import logging
from doorlogic import DoorLogic

class Door:
    DOOR_CLOSED        = (1<<0)
    LOCK_LOCKED        = (1<<1)
    LOCK_UNLOCKED      = (1<<2)
    LOCK_LOCKING       = (1<<3)
    LOCK_UNLOCKING     = (1<<4)
    HANDLE_PRESSED     = (1<<5)
    LOCK_PERM_UNLOCKED = (1<<6)
    
    def __init__(self, name, address, txseq, rxseq, key, interface, initial_unlock, input_queue):
        self.name = name
        self.address = address
        self.txseq = txseq
        self.rxseq = rxseq
        
        self.key = [int(x) for x in key.split()]
        self.aes = AES()
        
        self.interface = interface
        
        self.open = False
        self.closed = False
        self.locked = False
        self.unlocked = False
        self.supply_voltage = 0
        self.command_time = 0
        self.command_accepted = None
        self.command = None
        self.periodic = 10
        self.relock_time = 0
        self.desired_state = Door.LOCK_LOCKED
        self.buttons_toggle_state = None
        self.logger = logging.getLogger('logger')
        self.pressed_buttons = 0
        self.initial_unlock = initial_unlock
        self.periodic_timeout = time.time() + 1;
        self.state_listeners = set()
        self.perm_unlocked = False
        self.input_queue = input_queue

    def unlock(self, relock_timeout=0):
        self.desired_state = Door.LOCK_UNLOCKED
        if relock_timeout:
            self.relock_time = time.time() + relock_timeout
        else:
            self.relock_time = 0

        #if timeout:
        #    self._send_command(command=ord('D'), data='\x02')
        #else:
        #    self._send_command(command=ord('D'), data='\x01')

    def lock(self):
        self.desired_state = Door.LOCK_LOCKED
        self._send_command(command=ord('D'), data='\x00')

    def update(self, message):
    	if len(message) != 16:
            self.logger.warning("The received message is not 16 bytes long")
    	    return
        message = self.aes.decrypt([ord(x) for x in message], self.key,
                    AES.keySize["SIZE_128"])
        message = ''.join([chr(x) for x in message])

        self.logger.debug("Decoded message: %s"%str(list(message)))
        
        p = Packet.fromMessage(message)
        if p.cmd==83:
            self.supply_voltage = ord(p.data[3])*0.1
            
            pressed_buttons = ord(p.data[0])
            self.logger.debug('door: pressed_buttons = %d', pressed_buttons)
            if pressed_buttons & 0x01 and not self.pressed_buttons & 0x01:
                self.pressed_buttons |= 0x01
                self.input_queue.put({
                    'origin_type': DoorLogic.Origin.DOOR,
                    'origin_name': self.name,
                    'input_type': DoorLogic.Input.BUTTON,
                    'input_name': 'manual_control',
                    'input_value': ''})
            elif not pressed_buttons & 0x01:
                self.pressed_buttons &= ~0x01

            if pressed_buttons & 0x02 and not self.pressed_buttons & 0x02:
                self.pressed_buttons |= 0x02
                self.input_queue.put({
                    'origin_type': DoorLogic.Origin.DOOR,
                    'origin_name': self.name,
                    'input_type': DoorLogic.Input.SENSOR,
                    'input_name': 'bell_code',
                    'input_value': ''})
            elif not pressed_buttons & 0x02:
                self.pressed_buttons &= ~0x02
           
            doorstate = ord(p.data[1])
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
            self.logger.info('Door state: %s'%self.get_state())
            self.logger.info('Desired door state: %s'%self.get_desired_state())

            self.notify_state_listeners()

        elif p.cmd==ord('A'):
            accepted = ord(p.data[0]) == 1
            if not self.command_accepted:
                if accepted:
                    self.logger.info('Command at %d was accepted'%self.command_time)
                    self.command_accepted = True
                else:
                    self.logger.warning('Command at %d was NOT accepted'% self.command_time)

    def is_locked(self):
        return self.locked

    def is_perm_unlocked(self):
        return self.perm_unlocked

    def add_state_listener(self, listener):
        self.state_listeners.add(listener)

    def notify_state_listeners(self):
        for listener in self.state_listeners:
            listener(self)

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

    def get_desired_state(self):
        state = ''
        if self.desired_state & Door.LOCK_LOCKED:
            state += ' LOCKED'
        if self.desired_state & Door.LOCK_UNLOCKED:
            state += ' UNLOCKED'
        
        state = state.strip()
        return state

    def tick(self):
        if time.time() > self.periodic_timeout:
            self.periodic_timeout = time.time() + .2
            self._send_command(ord('D'), chr(self.desired_state))
        
        if self.relock_time:
            if time.time() > self.relock_time:
                self.desired_state = Door.LOCK_LOCKED
                self.relock_time = 0
        '''
        if time.time() - self.command_time > 5:
            if self.command_accepted == False:
                print 'Error: Command at %d was not accepted!'
            elif self.command_accepted == None:
                print 'Error: Command was not received'
        '''
    def _send_command(self, command, data):
        p = Packet(seq=self.txseq, cmd=command, data=data)
        msg = self.aes.encrypt([ord(x) for x in p.toMessage()], self.key,
                    AES.keySize["SIZE_128"])
        msg = ''.join([chr(x) for x in msg])

        self.logger.debug('Msg to door %s: %s'%(self.name, list(p.toMessage())))
        self.interface.writeMessage(self.address, msg)
        '''
        self.command_accepted = None
        self.command_time = time.time()
        '''


