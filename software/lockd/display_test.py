import serialinterface
import time
import sys
import display
from PIL import ImageDraw
from PIL import ImageFont

ser = serialinterface.SerialInterface("/dev/ttyUSB0", 115200, timeout=.1)

lcd = display.Display(ser)
draw = ImageDraw.Draw(lcd)

usr_font = ImageFont.truetype("Arial.ttf", 8)

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
        draw.text((0,8), "Hello World",'red', font=usr_font)
    draw.text((0,16), "Hello World",'blue', font=usr_font)

    lcd.update()
    time.sleep(1)

