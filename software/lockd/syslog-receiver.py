#!/usr/bin/env python
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

from socket import *

# Set the socket parameters
host = "localhost"
port = 23514
buf = 1024
addr = (host,port)

while 1:
    try:
        # Create socket and bind to address
        UDPSock = socket(AF_INET,SOCK_DGRAM)
        UDPSock.bind(addr)

        # Receive messages
        while 1:
            data,addr = UDPSock.recvfrom(buf)
            if not data:
                print "Client has exited!"
                break
            else:
                print data

        # Close socket
        UDPSock.close()
    except Exception,e:
        print e
    import time
    time.sleep(1)
