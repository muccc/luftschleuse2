import serialinterface
import time
import sys
import display

ser = serialinterface.SerialInterface("/dev/ttyUSB0", 115200, timeout=.1)

lcd = display.Display(ser)

while 1:

    lcd.fill(0xff,0,0)
    lcd.update()
    time.sleep(1)

    lcd.fill(0,255,0)
    lcd.update()
    time.sleep(1)

    lcd.fill(0,0,255)
    lcd.update()
    time.sleep(1)

    lcd.fill(0,0,0)
    lcd.update()
    time.sleep(1)

