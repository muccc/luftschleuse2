import time
from PIL import Image
import pygame

class Display:
    def __init__(self, x=98, y=70):
        self.X = x
        self.Y = y
    
        self._image = Image.new("RGB", (x,y), (0,0,0))
        self.screen = pygame.display.set_mode((x, y))
    
    def __getattr__(self, name):
        return getattr(self._image, name)

    #def __setattr__(self, name, value):
    #    self.__dict__[name] = value

    def update(self):
        mode = self._image.mode
        size = self._image.size
        data = self._image.tostring()
        surface = pygame.image.fromstring(data, size, mode)
        self.screen.blit(surface, (0,0))
        pygame.display.update()

