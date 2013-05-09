import time
import socket

class Announcer:
    def __init__(self):
        self.timestamp = time.time()
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind(('0.0.0.0', 2080))
        self.target = ('83.133.178.75', 2080)
        self.message = 'fnord'

    def tick(self):
        if time.time() - self.timestamp > 1:
            self.announce()

    def announce(self):
        self.sock.sendto(self.message, self.target)

    def update_state(self, state):
        self.message = state.get_state_as_string()

