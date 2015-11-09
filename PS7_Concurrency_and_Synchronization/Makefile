all: testprog

testprog: PS7.c sem
	gcc PS7.c tas64.S sem.o -o testprog.exe

sem: sem.c
	gcc -c sem.c -o sem.o

partA: partA.c sem
	gcc partA.c tas64.S sem.o -o partA.exe

tex: *.tex
	pdflatex PS5.tex

debug: PS5.c
	gcc -g PS5.c -o testprog.exe

clean:
	rm *.o
	rm *.exe