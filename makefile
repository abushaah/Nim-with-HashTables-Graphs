# to compile:
# make
# ./alphanim turn matchesInBoardRows

alphanim: alphanim.o nimhelp.o nim.o
	clang -Wall -std=c99 -pedantic alphanim.o nimhelp.o nim.o -o alphanim
alphanim.o: alphanim.c nim.h
	clang -Wall -std=c99 -pedantic -c alphanim.c
nimhelp.o: nimhelp.c nim.h
	clang -Wall -std=c99 -pedantic -c nimhelp.c
nim.o: nim.c nim.h
	clang -Wall -std=c99 -pedantic -c nim.c
clean:
	rm *.o
