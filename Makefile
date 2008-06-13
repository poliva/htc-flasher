all:
	cd src && ${MAKE} all
install:
	cp src/HTCFlasher /usr/bin/
	cp gui/HTCFlasherGUI /usr/bin/
clean:
	cd src && ${MAKE} clean
