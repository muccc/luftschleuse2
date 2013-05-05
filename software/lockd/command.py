import socket
import threading
from doorlogic import DoorLogic

class UDPCommand(threading.Thread):
    def __init__(self, host, port, input_queue):
        threading.Thread.__init__(self)    
        self.port = port
        self.host = host
        self.input_queue = input_queue

        self.sock = socket.socket(socket.AF_INET, # Internet
                        socket.SOCK_DGRAM) # UDP
        self.sock.bind((host, port))
        self.setDaemon(True)
        self.start()
 
    def run(self):
        while True:
            data, addr = self.sock.recvfrom(1024) # buffer size is 1024 bytes
            print "received message:", data
            self.input_queue.put({'origin_name': 'udp',
                     'origin_type': DoorLogic.Origin.NETWORK,
                     'input_name': '',
                     'input_type': DoorLogic.Input.COMMAND,
                     'input_value': data.strip()})

