all: server

server: server.c
	gcc -Ilib/ server.c lib/mongoose.c -o $@

server-static: server.c
	gcc -static -Ilib/ server.c lib/mongoose.c -o $@

simple: main.c
	gcc main.c -o $@

clean:
	rm -f server server-static

.PHONY: all clean