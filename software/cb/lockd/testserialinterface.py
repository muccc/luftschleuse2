import serialinterface
import sys

ser = serialinterface.SerialInterface(sys.argv[1], 115200, timeout=1)

ser.writeMessage('0', 'dummy\\0\\9 message')


