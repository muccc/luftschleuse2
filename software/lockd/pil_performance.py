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
from PIL import Image, ImageDraw
from PIL import ImageFont

X = 98
Y = 70
F = 1

image = Image.new("RGBA", (X*F,Y*F), (0,0,0))
draw = ImageDraw.Draw(image)

usr_font = ImageFont.truetype("Arial.ttf", 8)

t = time.time()

for i in range(100):
    image.paste((0,0,0))
    draw.text((0,0), "Hello World",'white', font=usr_font)


print time.time() - t

