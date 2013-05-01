from packet import Packet
import time
from aes import AES
import logging

class MasterController:
    def __init__(self, address, txseq, rxseq, key, interface, command_queue):
        self.address = address
        self.txseq = txseq
        self.rxseq = rxseq
        
        self.key = [int(x) for x in key.split()]
        self.aes = AES()
        
        self.interface = interface
        
        self.supply_voltage = 0
        self.periodic = 10
        self.logger = logging.getLogger('logger')
        self.pressed_buttons = 0
        
        self.command_queue = command_queue
        self.all_locked = False

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
            self.logger.debug('master: pressed_buttons = %d', pressed_buttons)

            if pressed_buttons & 0x01 and not self.pressed_buttons & 0x01:
                self.pressed_buttons |= 0x01
                self.command_queue.put('lock')
            elif not pressed_buttons & 0x01:
                self.pressed_buttons &= ~0x01

            if pressed_buttons & 0x02 and not self.pressed_buttons & 0x02:
                self.pressed_buttons |= 0x02
                self.command_queue.put('toggle_announce')
            elif not pressed_buttons & 0x02:
                self.pressed_buttons &= ~0x02

            if pressed_buttons & 0x04 and not self.pressed_buttons & 0x04:
                self.pressed_buttons |= 0x04
            elif not pressed_buttons & 0x04:
                self.pressed_buttons &= ~0x04

            self.logger.info('Master state: %s'%self.get_state())

    def get_state(self):
        state = ''
        state = state + ' Voltage=%.1f V'%self.supply_voltage
        state = state.strip()
        return state

    def tick(self):
        self.periodic-=1
        if self.periodic == 0:
            self.periodic = 2
            self._send_command(ord('S'), '')
            if self.all_locked:
                self._send_command(ord('L'), '\x00\x04')
            else:
                self._send_command(ord('L'), '\x00\x00')
        
    def _send_command(self, command, data):
        p = Packet(seq=self.txseq, cmd=command, data=data)
        self.logger.debug('Msg to mastercontroller: %s'%list(p.toMessage()))

        msg = self.aes.encrypt([ord(x) for x in p.toMessage()], self.key,
                    AES.keySize["SIZE_128"])
        msg = ''.join([chr(x) for x in msg])
        self.interface.writeMessage(self.address, msg)

    def announce_open(self):
        self._send_command(ord('L'), '\x02\x04')

    def announce_closed(self):
        self._send_command(ord('L'), '\x02\x01')
    
    def set_global_state(self, all_locked):
        self.all_locked = all_locked


