EXECUTABLE=server client
all:
	
	gcc server.c header_clang.o -o server
	gcc client.c header_clang.o -o client

clean:
	rm -f $(EXECUTABLE)