import commands
import os

os.system("g++ gblurMP.cpp -fopenmp -o blurMP `pkg-config --cflags --libs opencv`")

kernel = [3, 5, 7, 9, 11, 13, 15]
threads = [1, 2 ,4, 8, 16]
images = [ '720', '1080', '4k']


f = open("Outs MP", 'a')
f.write("Imagen Hilos Kernel Tiempo\n")
for i in images:
    for k in kernel:
        for t in threads:
            name = './blurMP ' + str(i) + '.jpg ' + str(t) + ' ' + str(k)
            a = commands.getoutput(name)
            print "Image: " + i + " Threads: " + str(t) + " Kernel: " + str(k) + " Time: " + a
            f.write(i + " " + str(t) + " " + str(k) + " " + a + "\n")
f.close()                

os.system("g++ gblurPX.cpp -fopenmp -o blurPX `pkg-config --cflags --libs opencv`")

f = open("Outs Posix", 'a')
f.write("Imagen Hilos Kernel Tiempo\n")
for i in images:
    for k in kernel:
        for t in threads:
            name = './blurPX ' + str(i) + '.jpg ' + str(t) + ' ' + str(k)
            a = commands.getoutput(name)
            print "Image: " + i + " Threads: " + str(t) + " Kernel: " + str(k) + " Time: " + a
            f.write(i + " " + str(t) + " " + str(k) + " " + a + "\n")
f.close()                
