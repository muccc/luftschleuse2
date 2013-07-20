from packet import Packet
import time
import logging
from doorlogic import DoorLogic
import ConfigParser

class Door:
    DOOR_CLOSED        = (1<<0)
    LOCK_LOCKED        = (1<<1)
    LOCK_UNLOCKED      = (1<<2)
    LOCK_LOCKING       = (1<<3)
    LOCK_UNLOCKING     = (1<<4)
    HANDLE_PRESSED     = (1<<5)
    LOCK_PERM_UNLOCKED = (1<<6)
    
    def __init__(self, name, config_file, config, interface, input_queue, buttons):
        self.name = name
        self.logger = logging.getLogger('logger')
        
        self.tx_seq = 0
        self.persisted_min_rx_seq = int(config.get(name, 'rx_sequence'))
        self.min_rx_seq = self.persisted_min_rx_seq
        self.min_rx_seq_leap = int(config.get(name, 'rx_sequence_leap'))
        self.address = config.get(name, 'address')

        if config.get(name, 'inital_unlock') == 'True':
            self.initial_unlock = True
        else:
            self.initial_unlock = False

        key = config.get(name, 'key')
        self.key = [int(x) for x in key.split()]

        self.interface = interface
        self.config_file = config_file

        self.closed = False
        self.locked = False
        self.unlocked = False
        self.supply_voltage = 0
        self.relock_time = 0
        self.desired_state = Door.LOCK_LOCKED
        self.pressed_buttons = 0
        self.periodic_timeout = time.time() + 1;
        self.state_listeners = set()
        self.perm_unlocked = False
        self.input_queue = input_queue
        self._old_state = None
        self.buttons = buttons

    def write_rx_sequence_number_to_config(self, min_rx_seq):
        config = ConfigParser.RawConfigParser()
        config.read(self.config_file)
        
        self.logger.debug("%s: Writing sequence number: %d" % (self.name, min_rx_seq))

        if config.has_section(self.name):
            if config.has_option(self.name, "rx_sequence"):
                config.set(self.name, "rx_sequence", min_rx_seq)
                f = open(self.config_file,'w');
                config.write(f);
                f.close()
                self.logger.debug("%s: Done" % (self.name))

    def unlock(self, relock_timeout=0):
        self.desired_state = Door.LOCK_UNLOCKED
        if relock_timeout:
            self.relock_time = int(time.time() + relock_timeout)
        else:
            self.relock_time = 0

    def lock(self):
        self.desired_state = Door.LOCK_LOCKED

    def update(self, message):
    	if len(message) != 16:
            self.logger.warning("%s: The received message is not 16 bytes long"%(self.name))
    	    return False
        
        p = Packet.fromMessage(message, key = self.key)
        
        if p == None:
            self.logger.debug("%s: Decoded packet was invalid" % self.name)
            return False
        
        if p.seq_sync:
            self.logger.debug("%s: Sync packet with seq: %d" % (self.name, p.seq))
            # This message contains a synchronization message for our
            # tx sequence number.
            self.tx_seq = p.seq
            return False

        if not p.seq >= self.min_rx_seq:
            self.logger.debug("%s: Seq %d not ok. Sending seq update to %d." %
                    (self.name, p.seq, self.min_rx_seq))
            # The door sent a sequence number which is too low.
            # Inform it about what we expect.
            p = Packet(seq=self.min_rx_seq, cmd=0, data='', seq_sync=True)
            msg = p.toMessage(key = self.key)

            self.logger.debug('%s: Msg to door: %s' %
                    (self.name, list(p.toMessage())))
            self.interface.writeMessage(self.address, msg)
            return False
        
        if p.seq >= self.persisted_min_rx_seq:
            self.persisted_min_rx_seq = p.seq + self.min_rx_seq_leap
            self.write_rx_sequence_number_to_config(self.persisted_min_rx_seq)

        self.min_rx_seq  = p.seq + 1

        if p.cmd == ord('S'):
            self.supply_voltage = ord(p.data[3])*0.1
            
            pressed_buttons = ord(p.data[0])
            self.logger.debug('master: pressed_buttons = %d' % pressed_buttons)
            for pin in self.buttons:
                if pressed_buttons & pin and not self.pressed_buttons & pin:
                    self.pressed_buttons |= pin
                    self.input_queue.put({'origin_name': 'Door1',
                        'origin_type': DoorLogic.Origin.DOOR,
                        'input_name': self.buttons[pin],
                        'input_type': DoorLogic.Input.BUTTON,
                        'input_value': True})
                elif not pressed_buttons & pin and self.pressed_buttons & pin:
                    self.input_queue.put({'origin_name': 'Door1',
                        'origin_type': DoorLogic.Origin.DOOR,
                        'input_name': self.buttons[pin],
                        'input_type': DoorLogic.Input.BUTTON,
                        'input_value': False})
                    self.pressed_buttons &= ~pin
           
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
            self.logger.info('%s: Door state: %s'%(self.name, self.get_state()))
            self.logger.info('%s: Desired door state: %s'%(self.name, self.get_desired_state()))
            
            if self._old_state != p.data:
                self._old_state = p.data
                self.notify_state_listeners()
        return True

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
        if self.relock_time:
            if time.time() > self.relock_time:
                self.logger.info("%s: relock time is over, locking the door again"%self.name)
                self.desired_state = Door.LOCK_LOCKED
                self.relock_time = 0

        if time.time() > self.periodic_timeout:
            self.periodic_timeout = time.time() + 1/3.
            self._send_command(ord('D'), chr(self.desired_state))
 
    def _send_command(self, command, data):
        p = Packet(seq=self.tx_seq, cmd=command, data=data, seq_sync=False)
        msg = p.toMessage(key = self.key)

        self.logger.debug('%s Msg to door: %s'%(self.name, list(p.toMessage())))
        self.interface.writeMessage(self.address, msg)
        self.tx_seq += 1

