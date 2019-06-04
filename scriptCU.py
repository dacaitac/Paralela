import commands
import os

os.system( 'nvcc `pkg-config --cflags opencv` -o `basename blurCU.cu .cu` blurCU.cu `pkg-config --libs opencv` -lm' )

kernel = [ 3, 5, 7, 9, 11, 13, 15 ]
threads = [ 16, 32, 64, 128, 256, 512, 1024 ]
blocks = [ 10, 15, 20, 25 ]
images = [ '720', '1080', '4k' ]

f = open( 'OutsCU.txt', 'a' )

for j in images:
	print ("Image: " + j)
	for k in kernel:
		for t in threads:
			for b in blocks:
				name = './blurCU ' + j + '.jpg out.jpg ' + str( k ) + ' ' + str( t ) + " " + str( b ) 
				a = commands.getoutput(name)
				print ("Block: "+ str(b) +" Threads: " + str(t) + " Kernel: " + str(k) + " Time: " + a)
				f.write(j + " " + str(b) + " " + str(t) + " " + str(k) + " " + a + "\n")
f.close()
