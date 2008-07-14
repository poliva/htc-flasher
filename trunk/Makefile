all:
	cd src && ${MAKE} all
	cd RomKitchen && ${MAKE} all
install:
	cp src/HTCFlasher /usr/bin/
	cp src/yang /usr/bin/
	cp gui/HTCFlasherGUI /usr/bin/
	cp gui/yangGUI /usr/bin/
	cp RomKitchen/dsmbuild /usr/bin/
	cp RomKitchen/sofbuild /usr/bin/
	cp RomKitchen/dsminfo /usr/bin/
	cp RomKitchen/nbimg /usr/bin/
	cp RomKitchen/nbsplit /usr/bin/
	cp htcflasher.desktop /usr/share/applications/
	cp yang.desktop /usr/share/applications/
	install -m 444 man/HTCFlasher.1 /usr/share/man/man1/
	install -m 444 man/HTCFlasherGUI.1 /usr/share/man/man1/
	install -m 444 man/yang.1 /usr/share/man/man1/
clean:
	cd src && ${MAKE} clean
	cd RomKitchen && ${MAKE} clean
remove:
	rm /usr/bin/HTCFlasher
	rm /usr/bin/HTCFlasherGUI
	rm /usr/bin/yang
	rm /usr/bin/yangGUI
	rm /usr/bin/dsmbuild
	rm /usr/bin/dsminfo
	rm /usr/bin/sofbuild
	rm /usr/bin/nbimg
	rm /usr/bin/nbsplit
	rm /usr/share/applications/htcflasher.desktop
	rm /usr/share/applications/yang.desktop
	rm /usr/share/man/man1/HTCFlasher.1
	rm /usr/share/man/man1/HTCFlasherGUI.1
	rm /usr/share/man/man1/yang.1
