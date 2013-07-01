import display

from PIL import ImageDraw
from PIL import ImageFont


class DisplayController:
    def __init__(self, display):
        self._display = display
        self._draw = ImageDraw.Draw(display)
        self._small_font_size = 8
        self._large_font_size = 16
        self._small_font = ImageFont.truetype('eigenbau.ttf', self._small_font_size)
        self._large_font = ImageFont.truetype('eigenbau.ttf', self._large_font_size)
        self._display.paste((0,0,0))
        self.render_large((10,70/2-16), 'Booting...', 'white');
        self._display.update()
    
    def render_small(self, xy, text, color):
        self._draw.text((xy[0]+1, xy[1]), text, color, font=self._small_font)
        
    def render_large(self, xy, text, color):
        self._draw.text((xy[0]+1, xy[1]), text, color, font=self._large_font)


