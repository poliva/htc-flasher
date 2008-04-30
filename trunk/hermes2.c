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
 * $Id: hermes2.c,v 1.9 2007/05/17 11:37:17 pau Exp $
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
#include <errno.h>
#include "main.h"

void hermes2_authBL()
{
	rsend ("r");
	get ("rs 1");
	rsend ("u");
	get ("rs 1");
	rsend ("u");
	get ("rs 1");
	rsend ("s");
	get ("rs 1");
	rsend ("t");
	get ("rs 1");
	rsend ("a");
	get ("rs 1");
	rsend ("r");
	get ("rs 1");
	rsend ("t");
	get ("rs 1");
	rsend ("\r");
	get ("rs 1");

	fsend ("password BsaD5SeoA");
	get ("HTCE");
	get ("CRLF 1");

}


int hermes2_flashNBH (char *flashfile)
{
	int fd2,readbytes;
	int n = 0x0;
	off_t len;
	char command[100];
	char nbhbuffer[HERM2BLOCK];
	unsigned long dataCrc;

	hermes2_authBL();

	// get nbh file
	fd2 = open (flashfile, O_RDONLY);
	if (fd2 < 0) {
		perror (flashfile);
		fprintf (stderr, "cannot open %s\n",flashfile);
		exit (1);
	}
	len = lseek(fd2, 0, SEEK_END);
	lseek(fd2, 0, SEEK_SET);

	// flash NBH with wdata
	for (n = 0x0; n < len; n=n+HERM2BLOCK) {

		unsigned int p=(int)(((float)n/len)*100);
		progressbar (p);
		spl2_progressbar (p);
		
		if ( (len-n) >= HERM2BLOCK ) readbytes = HERM2BLOCK;
		else readbytes=len-n;

		memset (nbhbuffer,0x00, HERM2BLOCK);
		int readlen = read (fd2, nbhbuffer, readbytes);

		if (readbytes != HERM2BLOCK) 
			dataCrc = Crc32 (nbhbuffer, readlen+3, 0);
		else	
			dataCrc = Crc32 (nbhbuffer, readlen, 0);

		sprintf (command,"wdata %x %lx", readlen, dataCrc);

		fsend (command);
		rsend ("HTCS");
		write (fd, nbhbuffer, readlen);
		rsend ("HTCE");
		get ("HTCE");


		/* 
		 * After we send a block of data encapsulated in HTCS + HTCE the bootloader
		 * returns a message encoded in HTCS + msg + 4byte checksum + HTCE, these
		 * are the known return values:
		 * 
		 * HTCS + 01 00 01 00 00 00 00 00 --> block received OK
		 * HTCS + 01 80 03 00 01 00 00 00 --> checksum error, resend packet
		 * HTCS + 01 00 02 00 00 00 00 00 --> NBH update complete
		 * HTCS + 02 80 06 00 00 00 00 00 --> Image cert is error
		 * 
		 */

		int found=0,j=0;
		char *str = "\x48\x54\x43\x53\x01\x80\x03\x00\x01\x00\x00\x00"; // HTCS+ checksum error
		for (j=0;j<BUFSIZE;j++) {
			if (!strcmp(&buf[j],str)){
				found=1;
				break;
			}
		}

		if (found) { // if we have a checksum error (the bucle above matched),
			     // then we need to resend the packet with the right checksum
			     // there seems to be a bug in the SPL here, because the last
			     // checksum is sometimes computed incorreclty :-?
			     // Checksum error, buffer=0xCD329116, input=0x3E8DC354

			int i=0;
			char *str2= "Checksum error, buffer=";
			char newchecksum[8];

			for (j=0;j<BUFSIZE;j++) { //find if the checksum error is on buf
				if (buf[j]==str2[i]) i++; else i=0;
				if (i==23) break;
			}
		
			for (j=0;j<8;j++) {
				newchecksum[j]=buf[i+j+2];
			}
			newchecksum[8]=0;

			if (VERBOSE) printf("\nCRC_ERROR: resending packet!\n");

			sprintf (command,"wdata %x %s", readlen, newchecksum);
			fsend (command);
			rsend ("HTCS");
			write (fd, nbhbuffer, readlen);
			rsend ("HTCE");
			get ("HTCE");
		}

	}

	spl2_progressbar(100);
	progressbar(100);
	sleep (1);
	fsend ("ResetDevice");
	sleep (1);

	close (fd2);
	printf ("\nDone!\n");
	return 0;
}

int hermes2_do_rbmc (char *dumpfile, int i_rbmcstart, int i_rbmcend)
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

	hermes2_authBL();

	//for (n = 0x50000000; n < 0x50280000; n = n + 0x80000) {
	for (n = i_rbmcstart; n < i_rbmcend; n = n + 0x20000) {   // we take only 128k
	
		unsigned int p=(100/voltes)*volta;
		progressbar (p);
		spl2_progressbar (p);
		volta++;

		sprintf (command,"rbmc file %x 80000", n); // we request 512k

		fsend ("set 1e 1");
		fsend (command);
		
		get ("rs 125");
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

	spl2_progressbar(100);
	progressbar(100);

	close(fd3);
	printf ("\nDone!\n");
	return 0;
}


