cordic: cordic.o fixedpoint.o
	gcc cordic.o fixedpoint.o -o cordic -lm
cordic.o: cordic.c
	gcc -c cordic.c -lm
fixedpoint.o: fixedpoint.c
	gcc -c fixedpoint.c -lm
fxconv: fxconv.o fixedpoint.o
	gcc fxconv.o fixedpoint.o -o fxconv -lm
fxconv.o: fxconv.c
	gcc -c fxconv.c -lm

clean:
	rm -f *.o cordic fxconv
