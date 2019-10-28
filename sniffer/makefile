sniffer: sniffer.o ethernet.o
	gcc sniffer.o ethernet.o -o sniffer -O2  -lpcap
sniffer.o: sniffer.c ethernet.h
	gcc sniffer.c -c -o sniffer.o -O2  -lpcap
ethernet.o: ethernet.c ethernet.h
	gcc ethernet.c -c -o ethernet.o -O2 -w  -lpcap

clean:
	rm *.o sniffer

