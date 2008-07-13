CC?=gcc
CFLAGS=-Wall

all:
	${CC} dsmbuild.c -o dsmbuild ${CFLAGS}
	${CC} dsminfo.c -o dsminfo ${CFLAGS}
	${CC} sofbuild.c -o sofbuild ${CFLAGS}

clean:
	-rm -f dsmbuild dsminfo sofbuild
