main : LiDE.o nwio.o 
	gcc -o lide LiDE.o nwio.o -Wall

LiDE.o : LiDE.c 
	gcc -c LiDE.c

nwio.o : nwio.c 
	gcc -c nwio.c

clean : 
	rm LiDE.o nwio.o
