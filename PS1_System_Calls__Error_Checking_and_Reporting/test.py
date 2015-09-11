# Test Script for copycat

import os
import math

cmd = ''
print(str(2^2))
for i in range(0, 19):
	buffersize = str(pow(2,i))
	cmd = "time ./copycat -b " + buffersize + " -o xout.txt xa*"
	#print(cmd)
	print("buffersize = " + buffersize)
	os.system(cmd)