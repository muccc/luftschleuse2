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
import logging
import time
import os

from PIL import ImageDraw
from PIL import ImageFont

from . import display

FONT_FILE_PATH = os.path.join(os.path.dirname(__file__), 'assets', 'eigenbau.ttf')

class DisplayController:
    def __init__(self, display, max_update_rate):
        self.logger = logging.getLogger('logger')
        self._display = display
        self._draw = ImageDraw.Draw(display)
        self._small_font_size = 8
        self._large_font_size = 16
        self._small_font = ImageFont.truetype(FONT_FILE_PATH, self._small_font_size)
        self._large_font = ImageFont.truetype(FONT_FILE_PATH, self._large_font_size)
        self.clear()
        self.render_large((10,70/2-16), 'Booting...', 'white');
        self._update()
        self._schedule_update = False
        self._min_update_time = 1 / max_update_rate
    
    def render_small(self, xy, text, color):
        self._draw.text((xy[0]+1, xy[1]), text, color, font=self._small_font)
        pass
        
    def render_large(self, xy, text, color):
        self._draw.text((xy[0]+1, xy[1]), text, color, font=self._large_font)
        pass

    def clear(self):
        self._display.paste((0,0,0), box=(0,0,98,70))
        pass

    def update(self):
        self._schedule_update = True

    def _update(self):
        self.logger.debug('Refreshing display content.')
        self._display.update()
        self._last_update_time = time.time()

    def tick(self):
        if self._schedule_update and time.time() - \
                self._last_update_time >= self._min_update_time:
            self._update()
            self._schedule_update = False
