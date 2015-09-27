all: shale

shale: PS3.c
	gcc PS3.c -o shale

tex: *.tex
	pdflatex PS3.tex

debug: PS3.c
	gcc -g PS3.c -o shale

clean:
	ls -1 | grep -E -v 'PS3.c|Makefile|test.py|*.tex' | xargs rm -rf