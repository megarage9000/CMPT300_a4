all:
	gcc -pthread -o UnixLs main.c

demo:
	gcc -pthread -o demo infodemo.c
clean:
	-rm *.o $(objects) UnixLs

cleanDemo:
	-rm demo
