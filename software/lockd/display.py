import time

class Display:
    def __init__(self, interface, x=98, y=70):
        self.interface = interface
        self.x = x
        self.y = y
    
        self.data = [[[0,0,0],]*y]*x

        self.reset()
        self.clear()
        self.update()

    def reset(self):
        self.command(0x04)

    def clear(self):
        self.fill(0x00,0x00,0x00)

    def fill(self, r, g, b):
        for x in range(self.x):
            for y in range(self.y):
                self.data[x][y] = [r,g,b]
    
    def set(self, data):
        self.data = data

    def update(self):
        self.start_stream()
        chain = []
        for y in range(self.y):
            for x in range(self.x):
                chain.append(self.data[x][y])

        prev = None
        count = 0
        
        pixel_data = ''

        for pixel in chain:
            r = pixel[0]
            g = pixel[1]
            b = pixel[2]

            if pixel == prev:
                count += 1
                if count == 15:
                    pixel_data += '%c%c'%((count<<4)|(r>>4),(b&0xF0)|(g>>4))
                    count = 0
            else:
                if prev != None:
                    pixel_data += '%c%c'%((count<<4)|(r>>4),(b&0xF0)|(g>>4))
                prev = pixel
                count = 0
        
        self.stream(pixel_data)
        self.stop_stream()

    def command(self, cmd, data = []):
        msg = "%c"%chr(cmd)
        msg += ''.join([chr(x) for x in data])
        self.interface.writeMessage(chr(0xFE), msg)

    def count_pixels(self, pixel_data):
        pixels = 0
        for i in range(0, len(pixel_data), 2):
            pixels += ord(pixel_data[i])>>4
        return pixels

    def start_stream(self):
        self.command(0x01)
        
    def stream(self, pixel_data):
        for i in range(0, len(pixel_data), 126):
            self.interface.writeMessage(chr(0xFF), pixel_data[i:i+126])
            time.sleep(self.count_pixels(pixel_data[i:i+126])*2e-5)

    def stop_stream(self):
        self.command(0x02)

