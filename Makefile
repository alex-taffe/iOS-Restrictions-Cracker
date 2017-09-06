restrictions-crack: main.o
	gcc -g -o restrictions-crack main.o

main.o:	main.c crack.c crack.h encode.o decode.o
	gcc -c -g main.c crack.c encode.o decode.o

encode.o: encode.c b64.h
	gcc -c -g encode.c

decode.o: decode.c b64.h
	gcc -c -g decode.c

clean:
	rm -f *.o restrictions-cracker
	
