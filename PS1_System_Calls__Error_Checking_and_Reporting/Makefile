all: copycat tex

copycat: PS1.c
	gcc PS1.c -o copycat

tex: *.tex
	pdflatex PS1.tex

clean:
	ls -1 | grep -E -v 'PS1.c|Makefile|test.py|lipsum*|*.tex' | xargs rm -f