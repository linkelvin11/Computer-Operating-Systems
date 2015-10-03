all: catgrepmore

catgrepmore: PS4.c
	gcc PS4.c -o catgrepmore

tex: *.tex
	pdflatex PS4.tex

debug: PS4.c
	gcc -g PS4.c -o catgrepmore

clean:
	ls -1 | grep -E -v 'PS4.c|Makefile|test.txt|*.tex' | xargs rm -rf