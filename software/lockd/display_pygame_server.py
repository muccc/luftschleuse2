import socket
import pygame

class Display:
    def __init__(self, x=98, y=70):
        self.X = x
        self.Y = y
     
        #self.screen = pygame.display.set_mode((x, y))

    def update(self, size, data):
        self.screen = pygame.display.set_mode(size)
        mode = 'RGB'
        surface = pygame.image.fromstring(data, size, 'RGB')
        self.screen.blit(surface, (0,0))
        pygame.display.update()


d = Display()
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind(("", 2324))
while True:
    data = s.recv(4096*16)
    x = ord(data[0])
    y = ord(data[1])
    data = data[2:]
    if len(data) != x * y * 3:
        print "received wrong data size."
    else:
        d.update((x,y), data)

