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

