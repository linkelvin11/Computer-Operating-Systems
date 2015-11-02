all: testprog

testfile:
	head -c 8195 </dev/urandom > testfile.txt
	head -c 10 </dev/urandom > smallfile.txt

testprog: PS5.c testfile
	gcc PS5.c -o testprog.exe

tex: *.tex
	pdflatex PS5.tex

debug: PS5.c
	gcc -g PS5.c -o testprog.exe

clean:
	rm *.o
	rm *.exe