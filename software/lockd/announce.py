import time
import socket

class Announcer:
    def __init__(self, open):
        self.timestamp = time.time()
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind(('0.0.0.0', 2080))
        self.target = ('83.133.178.75', 2080)
        self.open = open

    def tick(self):
        if time.time() - self.timestamp > 1:
            self.announce()

    def announce(self):
        self.sock.sendto(self.msg(), self.target)

    def msg(self):
        if self.open:
            return 'p'
        else:
            return 'n'
    
    def announce_open(self):
        self.open = True

    def announce_closed(self):
        self.open = False
