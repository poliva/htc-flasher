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
 * $Id: flasher.c,v 1.9 2007/05/18 10:50:13 pau Exp $
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

void getCID (char *cid)
{
	send ("info 2");
	get ("CRLF 1");
	get ("HTCE");

	memcpy (cid,buf+4,8);
	cid[8]=0;
	strip(cid);
}

void getSecLevel (char *seclevel)
{
	int i=0,j=0;
	char *str = "Level = ";

	send ("task 32");
	get ("CRLF 2");

	for (j=0;j<BUFSIZE;j++) {
		if (buf[j]==str[i]) i++; else i=0;
		if (i==8) break;
	}

	memcpy (seclevel,buf+j+1,2);
	seclevel[2]=0;
	strip(seclevel);
}

void getSPLversion (char *splversion)
{
	int i=0,j=0,k=0;

	send ("info 7");
	get ("CRLF 11");

	char *str = "Bootloader Version";
	for(j=0;j<BUFSIZE;j++) {
		if (buf[j]==str[i]) i++; else i = 0;
		if (i==18) break; 
	}

	for (k=0;k<8;k++) {
		splversion[k]=buf[j+k+4];
		if (buf[j+k+5]=='\r') {
			break;
		}
	}
	splversion[k+1]=0;
	strip(splversion);
}

void getRadioBLver(char *htcboot)
{
	enterRadioBL ();

	// get rversion
	send ("rversion");
	get ("rs 4");
	memcpy (htcboot,buf,4);
	htcboot[4]=0;
	exitRadioBL ();
	strip(htcboot);
}

void getQualcommBLver(char *qcboot)
{
	enterRadioBL ();

	// get rversion
	send ("rqversion");
	get ("rs 4");
	memcpy (qcboot,buf,4);
	qcboot[4]=0;
	exitRadioBL ();
	strip(qcboot);
}

void getDeviceName(char *devname)
{
	int i=0,j=0,k=0;

	send ("info 7");
	get ("CRLF 11");

	char *str = "Device Name:";
	for(j=0;j<BUFSIZE;j++) {
		if (buf[j]==str[i]) i++; else i = 0;
		if (i==12) break; 
	}

	for (k=0;k<10;k++) {
		devname[k]=buf[j+k+1];
		if (buf[j+k+2]==',') {
			break;
		}
	}
	devname[k+1]=0;
	strip(devname);
	if (!strcmp(devname,"H"))
			strcpy(devname,"HERMES");
	if (!strcmp(devname,"T"))
			strcpy(devname,"TRINITY");

}

void getCommonBase(char *commonbase)
{
	int i=0,j=0,k=0;

	send ("info 7");
	get ("CRLF 11");

	char *str = "Common Base Version";
	for(j=0;j<BUFSIZE;j++) {
		if (buf[j]==str[i]) i++; else i = 0;
		if (i==19) break; 
	}

	for (k=0;k<20;k++) {
		commonbase[k]=buf[j+k+4];
		if (buf[j+k+5]=='\r') {
			break;
		}
	}
	commonbase[k+1]=0;
	strip(commonbase);

}

unsigned long getChecksum(char *addr, char *len, int f)
{

	int i=0,j,k;
	char command[100];
	char checksum[10];
	unsigned long i_checksum;

	sprintf (command,"checksum %s %s",addr,len);

	if (f) {
		fsend (command);
		get ("CRLF 1");
	} else {
		send (command);
		get ("CRLF 2");
	}


	char *str = "CRC checksum=0x";
	for(j=0;j<BUFSIZE;j++) {
		if (buf[j]==str[i]) i++; else i = 0;
		if (i==15) break; 
	}

	for (k=0;k<10;k++) {
		checksum[k]=buf[j+k+1];
		if (buf[j+k+2]=='\r') {
			break;
		}
	}
	if (j==BUFSIZE) printf ("[] ERROR in getChecksum() !!!\n");
	checksum[k+1]=0;
	strip(checksum);
	sscanf(checksum, "%08lx", &i_checksum);

	return (i_checksum);

}
