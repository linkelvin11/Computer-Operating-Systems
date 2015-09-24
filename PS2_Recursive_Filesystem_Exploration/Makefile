all: walker

walker: PS2.c
	gcc -Wno-format -Wno-coverage-mismatch PS2.c -o walker

tex: *.tex
	pdflatex PS2.tex

debug:
	gcc -g PS2.c -o walker

clean:
	ls -1 | grep -E -v 'PS2.c|Makefile|test.py|*.tex|evilfs.img' | xargs rm -rf