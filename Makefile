LIBS=lib/mongoose.c lib/mjson.c

all: server

server: server.c
	gcc -Ilib/ server.c client.c $(LIBS) -o $@

server-static: server.c
	gcc -static -Ilib/ server.c client.c $(LIBS) -o $@

simple: main.c
	gcc main.c -o $@

clean:
	rm -f server server-static

.PHONY: all clean