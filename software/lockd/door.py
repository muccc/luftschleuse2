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
from packet import Packet
import time
import logging
from doorlogic import DoorLogic
import ConfigParser
import Queue

class Door:
    DOOR_CLOSED        = (1<<0)
    LOCK_LOCKED        = (1<<1)
    LOCK_UNLOCKED      = (1<<2)
    LOCK_LOCKING       = (1<<3)
    LOCK_UNLOCKING     = (1<<4)
    HANDLE_PRESSED     = (1<<5)
    LOCK_MANUAL_UNLOCKED = (1<<6)
    
    def __init__(self, name, config, interface, input_queue, buttons):
        self.name = name
        self.logger = logging.getLogger('logger')
        
        self.tx_seq = 0
        self.min_rx_seq_leap = int(config.get(name, 'rx_sequence_leap'))
        self.address = config.get(name, 'address')
        self.timeout = int(config.get(name, 'timeout'))

        if config.get(name, 'inital_unlock') == 'True':
            self.initial_unlock = True
        else:
            self.initial_unlock = False

        key = config.get(name, 'key')
        self.key = [int(x) for x in key.split()]

        self.config_file = config.get(name, 'sequence_number_container_file')

        self.interface = interface
        self.persisted_min_rx_seq = self.read_rx_sequence_number_from_container()

        self.min_rx_seq = self.persisted_min_rx_seq
        self.closed = False
        self.locked = False
        self.unlocked = False
        self.supply_voltage = 0
        self.relock_time = 0
        self.desired_state = Door.LOCK_LOCKED
        self.pressed_buttons = 0
        self.periodic_timeout = time.time() + 1;
        self.state_listeners = set()
        self.manual_unlocked = False
        self.input_queue = input_queue
        self._old_state = None
        self.buttons = buttons
        self.timedout = True
        self.status_update_timestamp = time.time()
        self.bad_key = False
        self.wrong_rx_seq = False
        self.locking = False 
        self.unlocking = False
        self.handle_pressed = False
        self.priority = 10
        self.tx_msg_queue = Queue.Queue()


    def read_rx_sequence_number_from_container(self):
        config = None
        
        try:
            config = ConfigParser.RawConfigParser()
            config.read(self.config_file)
        except:
            self.logger.error("%s: Could not open rx sequence number \
                                container at %s"%(self.name, self.config_file))
        
        if config is not None: 
            if config.has_section(self.name):
                if config.has_option(self.name, "rx_sequence"):
                    return int(config.get(self.name, "rx_sequence"))
        
            self.logger.error("%s: Could not read rx sequence number \
                            from container at %s"%(self.name, self.config_file))
        
        # TODO: what to do now? Raise an exception? Continue?
        return 0
           
    def write_rx_sequence_number_to_container(self, min_rx_seq):
        self.logger.debug("%s: Writing sequence number: %d" % (self.name, min_rx_seq))
        config = ConfigParser.RawConfigParser()

        config.add_section(self.name)
        config.set(self.name, "rx_sequence", min_rx_seq)

        f = open(self.config_file,'w');
        config.write(f)
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
            self._set_bad_key(True)
            return False
        
        self._set_bad_key(False)

        if p.seq_sync:
            self.logger.debug("%s: Sync packet with seq: %d" % (self.name, p.seq))
            # This message contains a synchronization message for our
            # tx sequence number.
            self.tx_seq = p.seq
            return False

        if not p.seq >= self.min_rx_seq:
            self.logger.debug("%s: Seq %d not ok. Sending seq update to %d." %
                    (self.name, p.seq, self.min_rx_seq))
            self._set_wrong_rx_seq(True)

            # The door sent a sequence number which is too low.
            # Inform it about what we expect.
            p = Packet(seq=self.min_rx_seq, cmd=0, data='', seq_sync=True)
            msg = p.toMessage(key = self.key)

            self.logger.debug('%s: Msg to door: %s' %
                    (self.name, list(p.toMessage())))
            self.interface.writeMessage(self.priority, self.address, msg, self.tx_msg_queue)
            return False
        
        self._set_wrong_rx_seq(False)

        if p.seq >= self.persisted_min_rx_seq:
            self.persisted_min_rx_seq = p.seq + self.min_rx_seq_leap
            self.write_rx_sequence_number_to_container(self.persisted_min_rx_seq)

        self.min_rx_seq  = p.seq + 1


        if p.cmd == ord('S'):
            self.status_update_timestamp = time.time()
            self.supply_voltage = ord(p.data[3])*0.1
            
            pressed_buttons = ord(p.data[0])
            self.logger.debug('master: pressed_buttons = %d' % pressed_buttons)
            for pin in self.buttons:
                if pressed_buttons & pin and not self.pressed_buttons & pin:
                    self.pressed_buttons |= pin
                    self.input_queue.put({'origin_name': self.name,
                        'origin_type': DoorLogic.Origin.DOOR,
                        'input_name': self.buttons[pin],
                        'input_type': DoorLogic.Input.BUTTON,
                        'input_value': True})
                elif not pressed_buttons & pin and self.pressed_buttons & pin:
                    self.input_queue.put({'origin_name': self.name,
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
            self.manual_unlocked = doorstate & Door.LOCK_MANUAL_UNLOCKED \
                            == Door.LOCK_MANUAL_UNLOCKED
            self.logger.info('%s: Door state: %s'%(self.name, self.get_state()))
            self.logger.info('%s: Desired door state: %s'%(self.name, self.get_desired_state()))
            
            if self._old_state != p.data:
                self._old_state = p.data
                self.notify_state_listeners()
        return True

    def is_locked(self):
        return self.locked

    def is_closed(self):
        return self.closed

    def is_manual_unlocked(self):
        return self.manual_unlocked

    def is_timedout(self):
        return self.timedout

    def is_bad_key(self):
        return self.bad_key

    def is_wrong_rx_seq(self):
        return self.wrong_rx_seq

    def _set_wrong_rx_seq(self, wrong_rx_seq):
        notify_listeners = False
        if not self.wrong_rx_seq == wrong_rx_seq:
            notify_listeners = True

        self.wrong_rx_seq = wrong_rx_seq
        
        if notify_listeners:
            self.notify_state_listeners()

    def _set_bad_key(self, bad_key):
        notify_listeners = False
        if not self.bad_key == bad_key:
            notify_listeners = True

        self.bad_key = bad_key
        
        if notify_listeners:
            self.notify_state_listeners()

    def _set_timedout(self, timedout):
        notify_listeners = False
        if not self.timedout == timedout:
            notify_listeners = True

        self.timedout = timedout
        
        if notify_listeners:
            self.notify_state_listeners()

    def add_state_listener(self, listener):
        self.state_listeners.add(listener)

    def notify_state_listeners(self):
        self.logger.info('%s: Door state changed: %s'%(self.name, self.get_state()))
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
        if self.manual_unlocked:
            state += ' MANUAL_UNLOCKED'
        if self.is_bad_key():
            state += ' BAD KEY'
        if self.is_wrong_rx_seq():
            state += ' BAD RX SEQ'
        if self.is_timedout():
            state += 'TIMEOUT'

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

        if time.time() - self.status_update_timestamp >= self.timeout:
            self._set_timedout(True)
        else:
            self._set_timedout(False)
 
    def _send_command(self, command, data):
        p = Packet(seq=self.tx_seq, cmd=command, data=data, seq_sync=False)
        msg = p.toMessage(key = self.key)

        self.logger.debug('%s Msg to door: %s'%(self.name, list(p.toMessage())))
        self.interface.writeMessage(self.priority, self.address, msg, self.tx_msg_queue)
        self.tx_seq += 1

