CC?=gcc
OBJ=nbhextract.o utils.o io.o crc32.o flasher.o info.o prompt.o nbh.o main.o
BIN=HTCFlasher
CFLAGS=-Wall

all: ${OBJ}
	${CC} ${OBJ} -o ${BIN}
	${CC} nbh.o yang.c -o yang

clean:
	-rm -f ${OBJ} ${BIN} yang
