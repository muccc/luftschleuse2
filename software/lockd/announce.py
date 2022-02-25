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
import socket

class Announcer:
    def __init__(self, host, port):
        self.timestamp = time.time()
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind(('0.0.0.0', 2080))
        self.target = (host, port)
        self.message = 'unknown'

    def tick(self):
        if time.time() - self.timestamp > 1:
            self.announce()
            self.timestamp = time.time()

    def announce(self):
        self.sock.sendto(bytes(self.message, encoding="ascii"), self.target)

    def update_state(self, state):
        self.message = state.get_state_as_string()
        f = open("/tmp/system_state", "w")
        f.write(state.get_state_as_string() + "\n")
        f.close()

