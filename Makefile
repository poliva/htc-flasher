all:
	cd src && ${MAKE} all
install:
	cp src/HTCFlasher /usr/bin/
	cp gui/HTCFlasherGUI /usr/bin/
	cp htcflasher.desktop /usr/share/applications/
	install -m 444 man/HTCFlasher.1 /usr/share/man/man1/
	install -m 444 man/HTCFlasherGUI.1 /usr/share/man/man1/
clean:
	cd src && ${MAKE} clean
