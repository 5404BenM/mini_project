CC=gcc
CFLAGS= -std=c99 -g
metis_test: metis_test.o generate_graph.o
	$(CC) $(CFLAGS) -o metis_test metis_test.o generate_graph.o /usr/local/lib/libmetis.a
	rm *.o

debug: metis_test.o
	$(CC) $(CFLAGS) -o metis_test metis_test.o /usr/local/lib/libmetis.a
	rm *.o

clean:
	rm *.o
	rm metis_test

