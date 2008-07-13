CC?=gcc
CFLAGS=-m32 -Wall

all:
	${CC} dsmbuild.c -o dsmbuild ${CFLAGS}
	${CC} dsminfo.c -o dsminfo ${CFLAGS}
	${CC} sofbuild.c -o sofbuild ${CFLAGS}
	${CC} nbimg.c -o nbimg ${CFLAGS}

clean:
	-rm -f dsmbuild dsminfo sofbuild nbimg
