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
 * $Id: io.c,v 1.7 2007/05/18 10:50:13 pau Exp $
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

#include "main.h"
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

void send (const char *command) {
	// send command to bootloader
	// automatically adds '\r' after the command
	if (DEBUG) printf ("\nSEND: %s\n", command);
	write (fd, command, strlen (command));
	write (fd, "\r", 1);
}

void rsend (const char *command) {
	// send command to bootloader
	// user must control CRLF at the end of the command
	if (DEBUG) printf ("\nSEND: %s\n", command);
	write (fd, command, strlen (command));
}

void fsend (const char *command) {
	// send command to bootloader
	// after ruustart, commands need to be 100-byte len
	// we add padding with '\0'.

	int i;
	char command2[101];
	for (i=0; i<strlen(command); i++) {
		command2[i]=command[i];
	}
	command2[i]='\r';
	for (i=i+1; i <100 ; i++) {
		command2[i]=0;
        }
        write (fd, command2, 100);
	if (DEBUG) printf ("\nFSEND: %s\n", command2);
}

void get (const char *command) {

/*
 * This function can be called in 3 different ways:
 * 
 * get ("HTCE"):
 * 	- returns fd buffer until we reach a HTC End block
 * 	- this is useful for commands which return data encapsulated in HTCS + HTCE blocks
 * 	- get("HTCE") doesn't print the output by default
 * get ("CRLF n"):
 * 	- returns fd buffer until we reach n times the combination CR+LF
 * 	- this is useful for geting the echo of our own commands when run with n=1
 * 	- it is useful for parsing commands that return several lines of output like 'info 7'
 * get ("rs n"):
 * 	- returns the last n characters of the fd buffer
 * 	- this is useful for commands in radio bootloader (like 'rversion') that
 * 	  do not send CR+LF after the command output
 *
 */	

	char rec = 0;
	unsigned long i,j = 0;

	if (!strcmp (command, "HTCE") ) {
		// get data from USB until we get HTCE
		i=0; j=0;
		while (1) {
			read (fd, &rec, 1);
			buf[i] = rec;
			if (DEBUG) printf ("%ld: %c - %x\n\r", i, rec, rec);
			if (i >3 && buf[i-3]=='H' && buf[i-2]=='T' && buf[i-1]=='C' && buf[i]=='E') {
				if (DEBUG) { // only print if debug enabled
					printf ("\nGET: ");
					for (j = 0; j < i ; j++) printf ("%c", buf[j]);
				}
				break;
			}
	 		i++;
			if (i > sizeof(buf)) { // control overflow
				fprintf (stderr, "can't find HTCE\n");
				break;
			}
		}

	}

	else if (!strncmp (command , "CRLF ",5) ) {
		// get data from USB until we get '\n' + '\r'
		int count=0;
		int bsize;
		char b[20];
		strcpy (b,command+5);
		sscanf ( b , "%d", &bsize);
		i=0; j=0;
		while (1) {
			
			read (fd, &rec, 1);
  			buf[i] = rec;
  			if (DEBUG) printf ("%ld: %c - %x\n\r", i, rec, rec);
  			if (i >1 && buf[i - 1] == 0xd && buf[i] == 0xa) {
				count++;
      				if (count==bsize) {
					if (DEBUG) printf ("\nGET: ");
					if (VERBOSE) for (j = 0; j < i; j++) printf ("%c", buf[j]);
					if (VERBOSE) printf ("\n");
					break;
				}
    			}
  			i++;
			if (i > sizeof(buf)) {
				fprintf (stderr, "cant find CRLF\n");
				break;
			}
		}
	}

	else if (!strncmp (command , "rs ",3) ) {
		// read specified number of bytes from USB
		char c[20];
		int isize ;
		strcpy (c,command+3);
		sscanf ( c , "%d", &isize);
		//printf ("GETparam: %s %d\n",c,isize);
		if (VERBOSE) printf ("\nGET: ");
      		for (i = 0; i < isize; i++) {
	  		read (fd, &rec, 1);
			if (DEBUG) printf ("%ld: %c - %x\n\r", i, rec, rec);
	  		buf[i] = rec;
	  		if (VERBOSE) printf ("%c", buf[i]);
		}
		if (VERBOSE) printf ("\n");
	}
}

void opendev()
{

	struct termios tc;     // 115200 baud, 8n1, no flow control
	fd = open (device, O_RDWR | O_NOCTTY);
	if (fd < 0) {
		perror (device);
		fprintf (stderr, "cannot connect to %s\n", device);
		exit (1);
	}


	// Get serial device parameters 
	tcgetattr(fd, &tc);

	tc.c_iflag &= ~( IGNBRK | BRKINT | PARMRK | ISTRIP
	                      | INLCR | IGNCR | ICRNL | IXON );
	tc.c_oflag &= ~OPOST;
	tc.c_lflag &= ~( ECHO | ECHONL | ICANON | ISIG | IEXTEN );
	tc.c_cflag &= ~( CSIZE | PARENB | CRTSCTS );
	tc.c_cflag |= CS8 | CREAD | CLOCAL ;
	tc.c_cc[VMIN] = 1;
	tc.c_cc[VTIME] = 3;

	// Set port speed to 9600 baud 
	cfsetospeed(&tc, B19200);
	cfsetispeed (&tc, B0);

	// Set TCSANOW mode of serial device 
	tcsetattr(fd, TCSANOW, &tc);

}

/*
void opendev()
{
	struct termios tc;     // 115200 baud, 8n1, no flow control
	fd = open (device, O_RDWR | O_SYNC);
	if (fd < 0) {
		perror (device);
		fprintf (stderr, "cannot connect to %s\n", device);
		exit (1);
	}

	tcgetattr(fd, &tc);
	tc.c_iflag = IGNPAR;
	tc.c_oflag = 0;
	tc.c_cflag = CS8 | CREAD | CLOCAL;
	tc.c_lflag = 0;
	cfsetispeed(&tc, B115200);
	cfsetospeed(&tc, B115200);
	tcsetattr(fd, TCSANOW, &tc);

	rsend ("\r");
	get ("rs 2");

	if (buf[0] == 'U' || buf[0] == 'C')
		get ("CRLF 1"); //already init
}
*/
