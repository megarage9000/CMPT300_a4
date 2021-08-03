all:
	gcc -pthread -o UnixLs main.c
clean:
	-rm *.o $(objects) UnixLs
