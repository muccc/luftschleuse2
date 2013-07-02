import time
from PIL import Image
import socket

class Display:
    def __init__(self, x=98, y=70):
        self.X = x
        self.Y = y
    
        self._image = Image.new("RGB", (x,y), (0,0,0))
        self.socket = self.udpHostSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)    

    def __getattr__(self, name):
        return getattr(self._image, name)

    #def __setattr__(self, name, value):
    #    self.__dict__[name] = value

    def update(self):
        size = self._image.size
        data = self._image.tostring()
        data = chr(size[0]) + chr(size[1]) + data

        self.socket.sendto(data, ('127.0.0.1', 2324))


