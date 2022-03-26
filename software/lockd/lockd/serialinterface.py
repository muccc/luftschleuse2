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
import serial
import string
import sys
import time
import logging
import socket
import select
import threading
import queue
from dataclasses import dataclass, field

@dataclass(order=True)
class PrioQueueItem:
    priority: int
    item: queue.Queue=field(compare=False)


class SerialInterface(threading.Thread):
    def  __init__ ( self, path2device, baudrate, timeout=0):
      threading.Thread.__init__(self)    
      self.logger = logging.getLogger('logger')

      self.portopen = False
      self.dummy = False
      self.udp = False

      if path2device == '/dev/null':
            self.dummy = True
            self.portopen = True
            self.timeout = timeout
      
      if path2device == 'udp': 
            self.udp = True
            self.portopen = True
            self.timeout = timeout
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self.sock.bind(('127.0.0.1', 31000))
            self.txtarget = ('127.0.0.1', 32000)
            self.sock.setblocking(0)

      while not self.portopen:
        try:
            self.ser = serial.Serial(path2device, baudrate)
            self.path2device = path2device
            self.baudrate = baudrate
            self.timeout = timeout
            self.ser.flushInput()
            self.ser.flushOutput()
            if timeout:
                self.ser.timeout = timeout
            self.portopen = True
            self.last = time.time()

        except serial.SerialException:
            self.logger.warning("Exception while opening %s"%path2device)
        time.sleep(1)

      self.logger.info("Opened %s"%path2device)

      self.input_queue = queue.PriorityQueue()

      self.setDaemon(True)
      self.start()

    def run(self):
        while True:
            pqitem = self.input_queue.get()
            (priority, queue) = (pqitem.priority, pqitem.item)
            if queue.empty():
                continue

            msg, delay = queue.get()
            #print 'writing %s' % list(msg)
            #self.logger.debug('writing %s' % list(msg))

            if self.udp:
                self.sock.sendto(msg, self.txtarget)
                continue

            if self.dummy:
                continue

            try:
                self.ser.write(msg)
                # Hack to avoid collisions, needs to be
                # converted to some sort of queue management
                time.sleep(delay)
            except :
                pass
                #self.reinit()
 
    def close(self):
        try:
            self.portopen = False
            self.ser.close()
        except serial.SerialException:
            pass

    def reinit(self):
        self.close()
        self.logger.warning("reopening")
        while not self.portopen:
            self.__init__(self.path2device, self.baudrate, self.timeout)
            time.sleep(1)
        self.logger.debug("done")

    def writeMessage(self, priority, command, message, queue, delay=0):
        enc = b"\\" + command + bytes([b if b != b'\\' else b'\\\\' for b in message]) + b"\\9";
        self.write(priority, enc, queue, delay)

    def write(self, priority, data, queue, delay=0):
        queue.put((data, delay))
        self.input_queue.put(PrioQueueItem(priority=priority, item=queue))

    def readMessage(self):
        data = ""
        escaped = False
        stop = False
        start = False
        inframe = False
        command = b''
        
        #if self.udp:
        #    time.sleep(self.timeout)
        #    return (False, '')
        if self.dummy:
            time.sleep(self.timeout)
            return (False, '')

        while True:
            starttime = time.time()
            c = []
            try:
                if self.udp:
                    ready = select.select([self.sock], [], [], self.timeout)
                    if ready[0]:
                        c = self.sock.recv(1)
                else:
                    c = self.ser.read(1)
                #print list(c)
            except KeyboardInterrupt:
                raise KeyboardInterrupt
            except Exception as e:
                self.logger.warning("port broken 2")
                self.reinit()
                return (False, '')
            endtime = time.time()
            if len(c) == 0:             #A timout occured
            #    if endtime-starttime < (self.timeout * 0.5):
            #        self.reinit()
            #        raise Exception('No data after %f seconds. Port seems broken.'%(endtime-starttime))
            #    else:
                    #print 'TIMEOUT'
                    return (False, '')
            if escaped:
                if c == b'\\':
                    d = b'\\'
                elif c == b'9':
                    stop = True
                    inframe = False
                else:
                    start = True
                    inframe = True
                    command = c
                    data = b""
                escaped = False
            elif c == b'\\':
                escaped = 1
            else:
                d = c
                
            if start and inframe:
                start = False
            elif stop:
                #print 'received message: len=%d data=%s'%(len(data),data)
                #print 'received message. command=',command, "data=" ,list(data)
                #print time.time() - self.last
                self.last = time.time()
                #print (command, data)
                return (command, data)
            elif escaped == False and inframe:
                data += d

