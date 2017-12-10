#Converts a give Bitmap into a Bitmaparry and load this to the ESP Device
#Version 0.1

from PIL import Image
import sys
import requests

#Load and Resize
#height value depends of LEDs of ESP Device
height = 60
img = Image.open(sys.argv[1])
scale = (height / float(img.size[1]))
width = int((float(img.size[0]) * float(scale)))
img = img.resize((width, height), Image.ANTIALIAS)
rgb_im = img.convert('RGB')
width = img.size[0]
height = img.size[1]

print("Datei:" , sys.argv[1] ,  " Höhe: " , height, " Breite: ", width )


#where 60 RGB LEDs * 8Bit für each RGB Channel. 
arraylength = width * height * 3
bitmap = bytearray(arraylength)
counter = 0

for w in range(width):
    for h in range(height):
       bitmap[counter + 1 ], bitmap[counter], bitmap[counter + 2] = rgb_im.getpixel((w, h))    
       counter+=3

#for Debug can write the "ESP" Array       
#f = open("test.buff", "wb")
#f.write(bitmap)
#f.close()

#Post the Data to the ESP Device
files = {"file":("picture",bitmap,'applicaiton-type')}
r = requests.post("http://192.168.64.49/upload", files=files)
print(r.text)