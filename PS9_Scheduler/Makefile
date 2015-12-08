all: sched.c sched.h savectx64.o test.c
	gcc -o test.exe test.c sched.c savectx64.o

savectx64:
	gcc -c savectx64.S -o savectx64.o

tex: *.tex
	pdflatex PS9.tex

clean:
	rm -f *.o *.exe *.out
