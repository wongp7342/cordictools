cordic: cordic.o fixedpoint.o
	gcc cordic.o fixedpoint.o -o cordic -lm
vcordic: vcordic.o fixedpoint.o
	gcc vcordic.o fixedpoint.o -o vcordic -lm
vcordic.o: vcordic.c
	gcc -c vcordic.c -lm
cordic.o: cordic.c
	gcc -c cordic.c -lm
fixedpoint.o: fixedpoint.c
	gcc -c fixedpoint.c -lm
fxconv: fxconv.o fixedpoint.o
	gcc fxconv.o fixedpoint.o -o fxconv -lm
fxconv.o: fxconv.c
	gcc -c fxconv.c -lm
trigangles: trigangles.o fixedpoint.o
	gcc trigangles.o fixedpoint.o -o trigangles -lm
trigangles.o: trigangles.c
	gcc -c trigangles.c -lm

clean:
	rm -f *.o cordic fxconv
