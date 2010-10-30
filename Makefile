hermes : hermes.o nwio.o 
	gcc -o hermes hermes.o nwio.o  -Wall

main.o : hermes.c 
	gcc -c hermes.c

nwio.o : nwio.c 
	gcc -c nwio.c

clean : 
	rm hermes hermes.o nwio.o


