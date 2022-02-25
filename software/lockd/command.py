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
import socket
import threading
import select
import time

from doorlogic import DoorLogic

class UDPCommand(threading.Thread):
    def __init__(self, host, port, input_queue):
        # TODO: rewrite to use TCP
        threading.Thread.__init__(self)    
        self.port = port
        self.host = host
        self.input_queue = input_queue

        self.sock = socket.socket(socket.AF_INET, # Internet
                        socket.SOCK_DGRAM) # UDP
        self.sock.bind((host, port))
        self.setDaemon(True)
        self.start()

    def empty_socket(self):
        """remove the data present on the socket"""
        input = [self.sock]
        while 1:
            inputready, o, e = select.select(input,[],[], 0.0)
            if len(inputready)==0: break
            for s in inputready: s.recv(1)
 
    def run(self):
        while True:
            data, addr = self.sock.recvfrom(1024) # buffer size is 1024 bytes
            print("received message:", data)
            self.input_queue.put({'origin_name': 'udp',
                     'origin_type': DoorLogic.Origin.NETWORK,
                     'input_name': '',
                     'input_type': DoorLogic.Input.COMMAND,
                     'input_value': data.strip()})

            # We do not allow more than two commands per second
            time.sleep(2)
            self.empty_socket()

