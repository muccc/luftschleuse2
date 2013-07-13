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
        self.sock.sendto(self.message, self.target)

    def update_state(self, state):
        self.message = state.get_state_as_string()

