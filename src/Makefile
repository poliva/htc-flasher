CC?=gcc
OBJ=nbhextract.o utils.o io.o crc32.o flasher.o info.o prompt.o main.o
BIN=HTCflasher
CFLAGS=-Wall

all: ${OBJ}
	${CC} ${OBJ} -o ${BIN}

clean:
	-rm -f ${OBJ} ${BIN}
