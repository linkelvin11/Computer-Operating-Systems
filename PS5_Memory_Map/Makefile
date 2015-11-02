all: testprog

testprog: PS5.c testfile.txt
	gcc PS5.c -o testprog.exe
	head -c 8195 </dev/urandom > testfile.txt
	head -c 10 </dev/urandom > smallfile.txt

tex: *.tex
	pdflatex PS5.tex

debug: PS5.c
	gcc -g PS5.c -o testprog.exe

clean:
	rm *.o
	rm *.exe