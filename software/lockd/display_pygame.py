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

