all: walker tex

walker: PS1.c
	gcc PS2.c -o walker

tex: *.tex
	pdflatex PS2.tex

clean:
	ls -1 | grep -E -v 'PS2.c|Makefile|test.py|*.tex' | xargs rm -f