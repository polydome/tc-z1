CC=gcc
LIBS=lib/mongoose.c lib/mjson.c
SRC=src/*.c
HEADERS=include/
BUILD=build

all: server

server: $(SRC) $(LIBS)
	[ -d $(BUILD) ] || mkdir -p $(BUILD)
	$(CC) -I$(HEADERS) $(SRC) $(LIBS) -o $(BUILD)/$@

server-static: $(SRC) $(LIBS)
	[ -d $(BUILD) ] || mkdir -p $(BUILD)
	$(CC) -static -I$(HEADERS) $(SRC) $(LIBS) -o $(BUILD)/$@

clean:
	rm -rf build

.PHONY: all clean