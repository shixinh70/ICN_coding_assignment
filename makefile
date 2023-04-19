EXECUTABLE=server client
all:
	
	gcc server.c header_gcc.o -o server
	gcc client.c header_gcc.o -o client

clean:
	rm -f $(EXECUTABLE)