import os, Image
path = "/home/xiao/Pictures"
files = os.listdir(path)
counter = 0
for file in files:
    counter = counter + 1
    img = Image.open(path + "/" + file)
    img.save(path + "/" + "%03d.jpg" % counter)
