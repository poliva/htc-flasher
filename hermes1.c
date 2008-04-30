/*
 * Copyright (C) 2007
 *       xda-developers.com
 *
 * Author:
 *       Pau Oliva - pof <pof@eslack.org>
 *
 * Contributors:
 *       pancake - <pancake@youterm.com>
 *        esteve - <esteve@eslack.org>
 *
 * $Id: hermes1.c,v 1.12 2007/05/17 02:06:43 pau Exp $
 *
 * HTCflasher is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * HTCflasher is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with HTCflasher; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <getopt.h>
#include "main.h"


void hermes1_authBL()
{
	int i = 0;
	unsigned char buffer[32];
	char command[100];

	send ("info 3");
	get ("HTCE");

	for (i = 0; i < 32; i++) {
		buffer[i] = buf[272 + i];
		if (DEBUG) printf ("buffer[%i]=%c\n\r", i, buffer[i]);

	}

	DecodeSecurityBuffer (buffer);
	sprintf (command,"password %s",buffer);
	send (command);
	get ("CRLF 1");
	get ("HTCE");
	if (buf[8] == '1') printf ("Authenticated OK\n");
	else {
		printf ("Auth error\n");
		exit (1);
	}
}

void authRadioBL()
{
	int i = 0;
	unsigned char buffer[32];

	enterRadioBL();

	send ("rinfo");
	get ("HTCE");

	for (i = 0; i < 32; i++) {
		buffer[i] = buf[692 + i];
		if (DEBUG) printf ("buffer[%i]=%c\n\r", i, buffer[i]);
	}

	if (DEBUG) printf ("\nSEND: rpass \\rHTCS\n");

	DecodeRadioSecurityBuffer (buffer);
	unsigned long nSecondCrc = Crc32 (buffer, 32, 0);
	write (fd, "rpass \rHTCS", 11);
	for (i = 0; i < 32; i++) write (fd, &buffer[i], 1);
	write (fd, &nSecondCrc, 4);
	write (fd, "HTCE", 4);
	get ("HTCE");

	if (buf[4] == 'T') printf ("Authenticated OK\n");
	else {
		printf ("Auth error\n");
		exit (1);
	}
}

int hermes1_flashNBH (char *flashfile)
{
	int fd2,readbytes;
	int n = 0x0;
	off_t len;
	char command[100];
	char nbhbuffer[HERM1BLOCK];
	unsigned long dataCrc;

	send ("shmsg 10 2 \"FLASH NBH\"");
	get("CRLF 1");
	hermes1_authBL();
	send ("set 1 0");
	get ("CRLF 1");

	// get nbh file
	fd2 = open (flashfile, O_RDONLY);
	if (fd2 < 0) {
		perror (flashfile);
		fprintf (stderr, "cannot open %s\n",flashfile);
		exit (1);
	}
	len = lseek(fd2, 0, SEEK_END);
	lseek(fd2, 0, SEEK_SET);

	// flash NBH with wdatah
	for (n = 0x0; n < len; n=n+HERM1BLOCK) {

		unsigned int p=(int)(((float)n/len)*100);
		progressbar (p);
		spl_progressbar (p);
		send ("set 1e 1");
		get ("CRLF 1");
		
		if ( (len-n) >= HERM1BLOCK ) readbytes = HERM1BLOCK;
		else readbytes=len-n;

		memset (nbhbuffer,0x00, HERM1BLOCK);
		int readlen = read (fd2, nbhbuffer, readbytes);

		if (readbytes != HERM1BLOCK) 
			dataCrc = Crc32 (nbhbuffer, readlen+3, 0);
		else	
			dataCrc = Crc32 (nbhbuffer, readlen, 0);

		sprintf (command,"wdatah %x %lx", readlen, dataCrc);

		send (command);
		get ("CRLF 1");
		get ("rs 25");
		rsend ("HTCS");
		write (fd, nbhbuffer, readlen);
		rsend ("HTCE");
		get ("HTCE");


		char *str = "\x48\x54\x43\x53\x01\x00\x02\x00\xE7\x05\x8a\x8a";  // HTCS+ block received OK
		int i=0,j=0;
		for (j=0;j<BUFSIZE;j++) { // this should match always, otherwise we have an error sending the NBH
			if (buf[j]==str[i]) i++; else i = 0;
			if (i==12) break; // it matched; i == lenght of the *str
		}

		if (i!=12) { // if there's an eror, print it and cowardly exit
			fprintf(stderr,"[] ERROR flashing NBH file:\n");
			for (j = 0; j < strlen(buf) ; j++) printf ("%c", buf[j]);
			printf ("\n");
			exit (-1);
		}

	}

	progressbar(100);
	spl_progressbar (100);

	close (fd2);
	printf ("\nDone!\n");

	send ("set 14 0");
	get ("HTCE");
	send ("shmsg 10 2 \"             \"");
	get("CRLF 1");
	send ("task 8 ");
	get("CRLF 1");

	return 0;
}

int hermes1_flashRadio (char *flashfile)
{
	int fd2;
	int n = 0x0;
	off_t len;
	char command[100];
	char rwbuffer[RBLOCK];
	unsigned long rwdataCrc;
	float percentage;

	// get radio file
	fd2 = open (flashfile, O_RDONLY);
	if (fd2 < 0) {
		perror (flashfile);
		fprintf (stderr, "cannot open %s\n",flashfile);
		exit (1);
	}
	len = lseek(fd2, 0, SEEK_END);
	lseek(fd2, 0, SEEK_SET);

	// check radio length
	if (len != RADIOSIZE) {
		fprintf (stderr, "[] ERROR: %s must be %d bytes!\n",flashfile,RADIOSIZE);
		exit (1);
	}


	progressbar(0);

	// initial erase of the radio
	for (n = 0x0; n < RADIOSIZE; n = n + RBLOCK) {

		sprintf (command,"rerase %x %x\r\n", n, RBLOCK);
		rsend (command);
		get ("HTCE");
	}

	// flash radio with rwdata
	for (n = 0x0; n < RADIOSIZE; n = n + RBLOCK) {
		
		percentage=((((float)n/RBLOCK * 100) / (float)(RADIOSIZE / RBLOCK))*0.98);
		progressbar((int)percentage);

		read (fd2, rwbuffer, RBLOCK);
		sprintf (command,"rwdata %x %x\r\nHTCS", n, RBLOCK);
		rsend (command);
		write (fd, rwbuffer, RBLOCK);
		rwdataCrc = Crc32 (rwbuffer, RBLOCK, 0);
		write (fd, &rwdataCrc, 4);
		write (fd, "HTCE", 4);
		get ("HTCE");
	}

	progressbar(99);

	if (VERBOSE) printf ("Calculate Radio checksum, this takes a while...\n");
	send ("rchecksum 80000 d00000");
	get ("HTCE");
	
	progressbar(100);

	close (fd2);
	printf ("\nDone!\n");
	return 0;
}

int hermes1_dumpRadio (char *dumpfile)
{
	int n = 0x0;
	int i, start, end, readcmd = 0;
	float percentage=0;
	char command[100];

	int fd3 = open (dumpfile, O_WRONLY|O_CREAT, 0666 );
	if (fd3 < 0) {
		perror (dumpfile);
		fprintf (stderr, "cannot create dump file!\n");
		exit (1);
	}

	printf ("Dumping radio...\n");

	// radio bootloader <= 0106 uses command 'rrbmc'
	// radio bootloader >= 0107 uses command 'rdpram'
	send ("rversion");
	get ("rs 4");

	if (buf[3] <= 6) readcmd=1;
	else readcmd=0;

	for (n = 0x0; n < RADIOSIZE; n = n + 0x8000) {
		
		percentage=(((float)n/0x8000 * 100) / (float)(RADIOSIZE / 0x8000));
		progressbar((int)percentage);

		if (readcmd) sprintf (command,"rrbmc file %x 8000", n);
		else sprintf (command,"rdpram file %x 8000", n);

		send (command);
		get ("HTCE");
		// data is in buf;

		//TODO: with rdpradm start and end are always fixed positions
		//      start: 56 , end: 32828-4 
		//      check if it is the same with rrbmc, this can be done much easily

		i=0;
		for (i=0; i < sizeof(buf) ; i++) { //search HTCS position
			if (i>3 && buf[i-3]=='H' && buf[i-2]=='T' && buf[i-1]=='C' && buf[i]=='S') {
				start=i+1;
				break;
			}
		}

		for (i=start; i < sizeof(buf) ; i++) { //search HTCE position
			if (i>3 && buf[i-3]=='H' && buf[i-2]=='T' && buf[i-1]=='C' && buf[i]=='E') {
				end=i-3; //extract HTCE
				break;
			}
		}

		for (i=start; i < end-4 ; i++) { //copy only real data (extract also CRC)
			write (fd3, &buf[i], 1);
		}

	}

	progressbar(100);

	close(fd3);
	printf ("\nDone!\n");
	return 0;
}

void enterRadioBL()
{
	// enter radio bootloader
	char devname[10];
	getDeviceName(devname);

	rsend ("rtask a\r\n");
	
	if (!strcmp(devname,"TRINITY") || !strcmp(devname,"TRIN100"))
		get ("CRLF 1");
	else
		get ("CRLF 2");
}

void exitRadioBL ()
{
	// exit radio bootloader
	write (fd,"r",1);
	sleep (1);
	write (fd,"e",1);
	sleep (1);
	write (fd,"t",1);
	sleep (1);
	write (fd,"u",1);
	sleep (1);
	write (fd,"o",1);
	sleep (1);
	write (fd,"R",1);
	sleep (1);

}


int hermes1_do_rbmc (char *dumpfile, int i_rbmcstart, int i_rbmcend)
{
	// This is crap :(
	// if someone knows how to fix this please send me a patch, I give out!
	// if you want a better (but not perfect) dump, use 'cu'.

	int n = 0x0;
	int i = 0;
	int volta=0;
	int start,end;
	char command[100];
	off_t len;

	printf ("\n[] WARNING: This feature is not working properly!\n");
	printf ("[] The dumped file will be most probably corrupt\n");
	printf ("[] either there is a communication problem with USB\n");
	printf ("[] or the bootloader is not sending the size requested\n\n");

	int fd3 = open (dumpfile, O_WRONLY|O_CREAT, 0666 );
	if (fd3 < 0) {
		perror (dumpfile);
		fprintf (stderr, "[] ERROR: cannot create dump file!\n");
		exit (1);
	}

	len=i_rbmcend-i_rbmcstart;
	int voltes=len/0x20000;

	hermes1_authBL();

	//for (n = 0x50000000; n < 0x50280000; n = n + 0x80000) {
	for (n = i_rbmcstart; n < i_rbmcend; n = n + 0x20000) {   // we take only 128k
	
		unsigned int p=(100/voltes)*volta;
		progressbar (p);
		spl_progressbar (p);
		volta++;

		sprintf (command,"rbmc file %x 80000", n); // we request 512k

		send ("set 1e 1");
		get ("CRLF 1");
		send (command);
		get ("rs 155");

		if (VERBOSE) {
			for (i=0; i<125;i++) {
				printf("%c",buf[i]);
				//printf ("%d: %c - %x\n\r", i, buf[i], buf[i]);
			}
		}

		memset (buf, 0, sizeof(buf));

		get ("rs 4");
		if (VERBOSE) {
			printf ("[] This must be HTCS, if not something is not working: ");
			for (i=0; i<4 ; i++) {
				printf("%c",buf[i]);
				//printf ("%d: %c - %x\n\r", i, buf[i], buf[i]);
			}
			printf ("\n===\n");
		}

		// get the 128k from *buf
		get ("rs 131072"); 

		start=0;
		end=131072;
		for (i=start; i < end ; i++) { 
			write (fd3, &buf[i], 1);
		}

		get ("HTCE"); // get until HTCE as size is not fixed
		
	}

	spl_progressbar(100);
	progressbar(100);

	close(fd3);
	printf ("\nDone!\n");
	return 0;
}
