all: client host

client: client.c constants
	gcc client.c -o client.exe constants.o

host: host.c
	gcc host.c -o host.exe

constants: constants.c constants.h
	gcc -c constants.c -o constants.o

tex: *.tex
	pdflatex PS6.tex

debug: client.c host.c constants
	gcc -g host.c -o debughost.exe constants.o
	gcc -g client.c -o debugclient.exe constants.o

clean:
	rm -f *.exe *.o *.stackdump *~