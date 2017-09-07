restrictions-crack: main.o encode.o decode.o crack.o fastpbkdf2.o
	gcc -pthread -g -o restrictions-crack main.o encode.o decode.o crack.o fastpbkdf2.o -lssl -lcrypto -lm -std=c11

main.o:	main.c 
	gcc -c -g main.c -std=c11

crack.o: crack.c crack.h 
	gcc -c -g crack.c -std=c11

encode.o: encode.c b64.h
	gcc -c -g encode.c -std=c11

decode.o: decode.c b64.h
	gcc -c -g decode.c -std=c11

fastpbkdf2.o: fastpbkdf2.c fastpbkdf2.h
	gcc -c -g fastpbkdf2.c -std=c11

clean:
	rm -f *.o restrictions-cracker
	
