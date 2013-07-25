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
import serialinterface
import time
import sys
import display
from PIL import ImageDraw
from PIL import ImageFont

ser = serialinterface.SerialInterface(sys.argv[1], 115200, timeout=.1)

lcd = display.Display(ser)
draw = ImageDraw.Draw(lcd)

font_size = int(sys.argv[3])

usr_font = ImageFont.truetype(sys.argv[2], font_size)

i = True
while 1:
    i = not i 
    #lcd.paste((0xff,0,0))
    #lcd.update()
    #time.sleep(1)

    #lcd.paste((0,255,0))
    #lcd.update()
    #time.sleep(1)

    #lcd.paste((0,0,255))
    #lcd.update()
    #time.sleep(1)

    lcd.paste((0,0,0))
    #draw.ellipse((98/2-i,70/2-i,98/2+i,70/2+i))
    #lcd.paste((255,255,255))
    #draw.text((0,0), "Hello World",(255,0,0), font=usr_font)

    draw.text((0,0), "Hello World",'white', font=usr_font)
    if i:
        draw.text((0,font_size), "Hello World",'red', font=usr_font)
    draw.text((0,font_size*2), "Hello World",'blue', font=usr_font)

    lcd.update()
    time.sleep(1)

