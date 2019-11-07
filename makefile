server: server.o http.o action.o
	gcc server.o http.o action.o -o server -O2
server.o: server.c http.h action.h
	gcc server.c -c -o server.o -O2
http.o: http.c http.h
	gcc http.c -c -o http.o -O2
action.o: action.c action.h
	gcc action.c -c -o action.o -O2
clean:
	rm *.o server
