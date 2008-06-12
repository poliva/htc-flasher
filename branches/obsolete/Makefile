CC?=gcc
OBJ=crc32.o decode.o flasher.o io.o main.o utils.o
OBJ+=models.o hermes1.o hermes2.o
BIN=HTCflasher
CFLAGS=-Wall

all: ${OBJ}
	${CC} ${OBJ} -o ${BIN}

clean:
	-rm -f ${OBJ} ${BIN}

commit:
	svn commit
	svn2cl
