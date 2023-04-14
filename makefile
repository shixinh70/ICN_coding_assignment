EXECUTABLE=server client
all:
	gcc header.c -c -o header.o
	gcc server.c header.o -o server
	gcc client.c header.o -o client

clean:
	rm -f $(EXECUTABLE)