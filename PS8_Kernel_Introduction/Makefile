all: part1 part2 part3 part4 part5

part1: part1.c
	gcc part1.c -o part1.exe

part2: part2.S
	as --64 part2.S -o part2.o
	ld -m elf_x86_64 part2.o -o part2.exe

part3: part3.S
	as --64 part3.S -o part3.o
	ld -m elf_x86_64 part3.o -o part3.exe

part4: part4.S
	as --64 part4.S -o part4.o
	ld -m elf_x86_64 part4.o -o part4.exe

part5: part5.c
	gcc part5.c -o part5.exe

tex: *.tex
	pdflatex PS8.tex

clean:
	rm -f *.o *.exe *.out
