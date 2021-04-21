CC=gcc
CFLAGS=
generate_graph: generate_graph.o
	$(CC) -o generate_graph generate_graph.o
	#rm *.o

debug: generate_graph.o
	$(CC) -g -o generate_graph generate_graph.o
	#rm *.o

clean:
	rm *.o
	rm generate_graph
	rm generate_graph.exe

