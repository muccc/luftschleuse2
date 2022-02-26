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
import time
import logging
from PIL import Image
import queue

class Display:
    def __init__(self, interface, x=98, y=70):
        self.logger = logging.getLogger('logger')
        self._interface = interface
        self.X = x
        self.Y = y
    
        self._image = Image.new("RGB", (x,y), (0,0,0))
        
        self.priority = 100

        self.tx_msg_queue = queue.Queue()

        self.reset()
    
    def __getattr__(self, name):
        return getattr(self._image, name)

    #def __setattr__(self, name, value):
    #    self.__dict__[name] = value

    def reset(self):
        self._command(0x04)
        time.sleep(1)

    def update(self):
        t = time.time();
        self._start_stream()

        chain = self._image.tobytes()
        chain = [chain[i:i+3] for i in range(0, len(chain), 3)]

        prev = None
        count = 0
        
        pixel_data = b''
        
        def pack_pixel(count, pixel):
            r = pixel[0]
            g = pixel[1]
            b = pixel[2]
            return bytes([(count<<4)|(r>>4), (g&0xF0)|(b>>4)])

        for pixel in chain:
            if pixel == prev:
                if count == 15:
                    pixel_data += pack_pixel(count, pixel)
                    count = 0
                else:
                    count += 1
            else:
                if prev != None:
                    pixel_data += pack_pixel(count, prev)
                prev = pixel
                count = 0

        pixel_data += pack_pixel(count, prev)

        self.logger.debug('Display: prep %f'%(time.time()-t))
        t = time.time()
        self._stream(pixel_data)
        self._stop_stream()
        self.logger.debug('Display: disp %f'%(time.time()-t))

    def _command(self, cmd, data = []):
        msg = bytes([cmd] + data)
        self._interface.writeMessage(self.priority, bytes([0xFE]), msg, self.tx_msg_queue)

    def _start_stream(self):
        self._command(0x01)
        
    def _stream(self, pixel_data):
        multi = 16
        for i in range(0, len(pixel_data), multi):
            self._interface.writeMessage(self.priority, bytes([0xFF]), pixel_data[i:i+multi], self.tx_msg_queue)

    def _stream_raw(self, raw_data):
        for i in range(0, len(raw_data), 120):
            self._interface.writeMessage(self.priority, bytes([0xFD]), raw_data[i:i+120], self.tx_msg_queue)

    def _stop_stream(self):
        self._command(0x02)

