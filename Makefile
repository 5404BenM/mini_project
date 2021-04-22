CC=gcc
CFLAGS= -std=c99 -g
generate_graph: generate_graph.o
	$(CC) $(CFLAGS) -o generate_graph generate_graph.o
	#rm *.o

debug: generate_graph.o
	$(CC) $(CFLAGS) -o generate_graph generate_graph.o
	#rm *.o

clean:
	rm *.o
	rm generate_graph
	rm generate_graph.exe

