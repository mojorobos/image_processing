CC=clang
CFLAGS=-L./libs/libjpeg/lib/ -ljpeg -I./libs/libjpeg/include/

SRC = src
INCLUDE = libs/libpeg/include
DEPS = $(INCLUDE)/jpeglib.h
OBJ = $(SRC)/jpeg.o $(SRC)/main.o

%.o: %.c $(DEPS)
	$(CC) $(DEBUG) -c -o $@ $<

processing: $(OBJ)
	$(CC) $(DEBUG) -o $@ $^ $(CFLAGS)

clean:
	rm $(OBJ) processing

debug: DEBUG = -g -O0
debug: processing
