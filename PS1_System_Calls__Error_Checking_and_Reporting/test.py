# Test Script for copycat

import os
import math

def command(com):
	print(com)
	os.system(com)

# generate test files
os.system("make clean")
os.system("make")
os.system("split -n 10 lipsum.txt")

print("\n---test unspecified buffersize---")
command("./copycat -b")

print("\n---test invalid buffer sizes---")
command("./copycat -b buffersize -o xout.txt lipsum.txt")

print("\n---test multiple buffers---")
command("./copycat -b 10 -b 50 -o xout.txt lipsum.txt")

print("\n---test unspecified output file---")
command("./copycat -o")

print("\n---test multiple output files---")
command("./copycat -o xout.txt -o xout2.txt")

print("\n--test invalid output files---")
command("chmod 000 xout.txt")
command("./copycat -o xout.txt lipsum.txt")
command("chmod 777 xout.txt")

print("\n---test invalid input files---")
command("./copycat -o xout.txt nonexistent file")

print("\n---test multiple file concatenation---")
command("split -n 10 lipsum.txt")
command("./copycat -o xout.txt xa*")
command("diff xout.txt lipsum.txt")

print("\n---test speed for multiple buffer sizes---")
for i in range(0, 19):
	buffersize = str(pow(2,i))
	print("buffersize = " + str(buffersize) + " bytes")
	os.system("/usr/bin/time -f \"%E real,\t%U user,\t%S sys\" ./copycat -b " + buffersize + " -o xout.txt xa*")
os.system("make clean")