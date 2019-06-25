import commands
import os

os.system( 'nvcc `pkg-config --cflags opencv` -o `basename blurCU.cu .cu` blurCU.cu `pkg-config --libs opencv` -lm' )

kernel = [ 3, 5, 7, 9, 11, 13, 15 ]
threads = [ 1024 ]
blocks = [ 1, 2, 4, 8, 16 ]
images = [ '720', '1080', '4k' ]

f = open( 'OutsCU.txt', 'a' )

for j in images:
	print ("Image: " + j)
	for k in kernel:
		for b in blocks:
			name = './blurCU ' + j + '.jpg out.jpg ' + str( k ) + ' ' + str( 1024 ) + " " + str( b ) 
			a = commands.getoutput(name)
			print ("Block: "+ str(b) +" Threads: " + " Kernel: " + str(k) + " Time: " + a)
			f.write( a + " ")
		f.write("\n")

f.close()
