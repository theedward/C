STATserver: STATserver.o SMBserver.o client.o
	gcc -o STATserver STATserver.o
	gcc -o SMBserver SMBserver.o
	gcc -o client client.o

STATserver.o: STATserver.c 
	gcc -g -c STATserver.c

SMBserver.o: SMBserver.c 
	gcc -g -c SMBserver.c

client.o: client.c 
	gcc -g -c client.c

clean:
	rm -f *.o client
	rm -f *.o SMBserver
	rm -f *.o STATserver
