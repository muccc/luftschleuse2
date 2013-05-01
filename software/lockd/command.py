import socket
import threading

class UDPCommand(threading.Thread):
    def __init__(self, host, port, command_queue):
        threading.Thread.__init__(self)    
        self.port = port
        self.host = host
        self.command_queue = command_queue

        self.sock = socket.socket(socket.AF_INET, # Internet
                        socket.SOCK_DGRAM) # UDP
        self.sock.bind((host, port))
        self.setDaemon(True)
        self.start()
 
    def run(self):
        while True:
            data, addr = self.sock.recvfrom(1024) # buffer size is 1024 bytes
            print "received message:", data
            self.command_queue.put(data.strip())

