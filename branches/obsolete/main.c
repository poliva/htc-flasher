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
 * $Id: main.c,v 1.11 2007/05/18 15:09:07 pau Exp $
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

int fd;
char buf[BUFSIZE];
int DEBUG    = 0;
int VERBOSE  = 0;
char *MODEL  = NULL;
char *device = "/dev/ttyUSB0";

void help_show_message()
{
        printf(
        "Usage: HTCflasher -m [model] [option]\n"
	"\n"
	" model:\n"
	"    hermes1 - HTC Hermes SPL 1.04, 1.06, 1.09\n"
	"    hermes2 - HTC Hermes SPL 1.11 & 2.04\n"
	"    trinity - HTC Trinity all SPL versions\n"
	"\n"
        " options:\n"
        "    -F nbh [filename]  flash nbh file\n"
        "    -F radio [filename]  flash raw radio file (needs 'rtask')\n"
        "    -r [filename]    dump radio rom (needs 'rtask')\n"
	"    -b [startaddr-endaddr] [filename] dump NAND\n"
	"    -s [serial]      use another serial device (default:/dev/ttyUSB0)\n"
        "    -i               get device info\n"
	"    -p               enter serial prompt (minicom/mtty like)\n"
        "    -v               verbose mode\n"
        "    -d               show debug information\n"
        "    -h               shows this help message\n");
}

void showcommandshelp()
{
	printf("Please see XDA-Developers wiki for the list of available commands\n");
	printf("http://wiki.xda-developers.com/index.php?pagename=Hermes_BootLoader\n");
	printf("http://wiki.xda-developers.com/index.php?pagename=Trinity_BootLoader\n");
	printf("\n");
}

int main(int argc, char **argv)
{
	int c,j,k;
	int info=0;
	int flash=0;
	int dumpradio=0;
	int rbmc=0;
	int i_rbmcstart,i_rbmcend;
	int shell = 0;
	char flashfile[500];
	char dumpfile[500];
	char rbmcaddr[30];
	char rbmcstart[10];
	char rbmcend[10];
	char rompart[30];
	char cid[10];
	char seclevel[10];
	char splversion[20];
	char htcboot[10];
	char qcboot[10];
	char commonbase[20];
	char devname[10];

	printf ("=== XDA-Developers HTCflasher v2.0 by pof\n");
	printf ("=== Open source RUU for HTC devices\n\n");
	
	if (argc < 2) {
		help_show_message();
		return 1;
	}

	while ((c = getopt(argc, argv, "F:r:ivdVhps:m:b:")) != -1) {
		switch( c ) {
		case 's':
			device = optarg;
			break;
		case 'F':
			strcpy (rompart,optarg);
			strcpy (flashfile,optarg+strlen(optarg)+1);
			if ( optarg+strlen(optarg)+1 >= argv[argc-1]+strlen(argv[argc-1]) || flashfile[0] =='-') { 
				fprintf (stderr, "[] ERROR: Param error!\n");
				help_show_message();
				return 1;
			}
			printf ("[] Flash %s file: %s\n",rompart,flashfile);
			flash = 1;
			break;
		case 'i':
			info = 1;
			break;
		case 'r':
			strcpy(dumpfile,optarg);
			dumpradio = 1;
			break;
		case 'b':
			strcpy (rbmcaddr,optarg);
			strcpy (dumpfile,optarg+strlen(optarg)+1);
			if ( optarg+strlen(optarg)+1 >= argv[argc-1]+strlen(argv[argc-1]) || flashfile[0] =='-') { 
				fprintf (stderr, "[] ERROR: Param error!\n");
				help_show_message();
				return 1;
			}
			for (j=0;j<strlen(rbmcaddr);j++){
				if (rbmcaddr[j]!='-') {
					rbmcstart[j]=rbmcaddr[j];
				}
				else {
					break;
				}
			}
			rbmcstart[j]=0;
			if (j==strlen(rbmcaddr)) {
				fprintf (stderr, "[] ERROR: syntax [startaddr-endaddr]!\n");
				return 1;
			}
			j++;
			for (k=j;k<strlen(rbmcaddr);k++){	
				rbmcend[k-j]=rbmcaddr[k];
			}
			rbmcend[k-j]=0;

			if (strlen(rbmcstart)!=8 || strlen(rbmcend)!=8){
				fprintf (stderr, "[] ERROR: memory addres must be 8 bytes!\n");
				return 1;
			}
			sscanf(rbmcstart, "%08x", &i_rbmcstart);
			sscanf(rbmcend, "%08x", &i_rbmcend);
			if (i_rbmcstart >= i_rbmcend) {
				fprintf (stderr, "[] ERROR: startaddr must be lower than endaddr!\n");
				return 1;
			}
			rbmc = 1;
			break;
		case 'v':
			printf("[] verbose mode enabled\n");
			VERBOSE = 1;
			break;
		case 'd':
			printf("[] debug mode enabled\n");
			DEBUG = 1;
			break;
		case 'm':
			MODEL = optarg;
			model = resolve_model(MODEL);
			break;
		case 'p':
			printf("[] serial prompt shell. Use ^C to exit.\n");
			shell = 1;
			break;
		case 'h':
			help_show_message();
			return 0;
		}
	}


	signal(SIGINT, (void (*)(int))control_c);

	opendev();

	if (shell) {
		char str[513];
		int pid;
		pid = fork();
		if (pid) {
			while(1) {
				fgets(str, 512, stdin);
				if (!strncmp (str,"retuoR",6)) {
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
				else {
					if (str[0]=='?')
						showcommandshelp();
					else
					if (write(fd, str, strlen(str)-1) == -1) {
						perror("write");
						exit (1);
					}

					write(fd, "\r\n", 2) ;
				}
			}
		} else {
			char buf;
			while(1) {
				if (read(fd, &buf,1) == -1) {
					perror("read");
					exit (1);
				}
				write(1, &buf, 1);
			}
		}
		close(fd);
		exit (0);
	} //end shell

	if (info == 1) {
		VERBOSE = 0; // we disable verbose in info
		printf ("[] Getting device info... please wait\n\n");
		
		getSecLevel(seclevel);
		getCID(cid);
		getSPLversion(splversion);
		getDeviceName(devname);
		getCommonBase(commonbase);
		printf(" Device=%s\n",devname);
		printf(" SecLevel=%s\n",seclevel);
		printf(" CID=%s\n",cid);
		printf(" SPL=%s\n",splversion);
		printf(" CommonBase=%s\n",commonbase);
		if ( (!strcmp(splversion,"1.04") || !strcmp(splversion,"1.10.Oli") || !strcmp(splversion,"1.01"))) {
			printf("\n[] Querying radio information... please wait\n\n");
			getRadioBLver(htcboot);
			getQualcommBLver(qcboot);
			printf(" Radio bootloader=%s\n",htcboot);
			printf(" Qualcomm bootloader=%s\n",qcboot);
		}
		printf("\nDone!\n");
	} // end info=1

	if (model == NULL && info==0) {
		fprintf (stderr, "\n[] ERROR: Please specify mandatory param -m [model]\n");
		fprintf (stderr, "Type 'HTCflasher -h' for a list of available models\n");
		return 1;
	}

	if (rbmc == 1) {
		getSecLevel(seclevel);
		if (seclevel[0]!='0') {
			fprintf (stderr, "\n[] ERROR: You need a SuperCID device or hacked SPL to use rbmc\n");
		}
		printf ("[] Doing rbmc \n");
		printf ("\t@START: %s\n\t@END: %s\n\tFILE: %s\n",rbmcstart,rbmcend,dumpfile);
		if (!strcmp(MODEL,"hermes1"))
			hermes1_do_rbmc (dumpfile,i_rbmcstart,i_rbmcend);
		else
			hermes2_do_rbmc (dumpfile,i_rbmcstart,i_rbmcend);
	} // end rbmc=1

	else if (dumpradio == 1) {
		// make sure spl is <= 1.04 to dump radio
		getSPLversion(splversion);
		if (splversion[3] > '4') { 
			fprintf (stderr, "[] ERROR: Your SPL version is %s\n", splversion);
			fprintf (stderr, "[] ERROR: you need SPL <= 1.04 to dump radio\n");
			exit (1);
		}
		printf("WARNING: The dumped radio can't be flashed back!!\n");
		authRadioBL();
		CALL(dumpRadio, dumpfile);
		exitRadioBL();
		printf("WARNING: The dumped radio can't be flashed back!!\n");
	} //end dumpradio=1

	else if (flash == 1) {
		if (!strcmp(rompart,"radio")) {
			// make sure spl is <= 1.04 to flash radio
			getSPLversion(splversion);
			if (splversion[3] > '4') { 
				fprintf (stderr, "[] ERROR: Your SPL version is %s\n", splversion);
				fprintf (stderr, "[] ERROR: you need SPL <= 1.04 to flash raw radio only\n");
				exit (1);
			}
			authRadioBL();
			CALL(flashRadio, flashfile);
			exitRadioBL();
			send ("task 8");
			get("CRLF 1");
		}

		if (!strcmp(rompart,"nbh")) {
			CALL(flashNBH, flashfile);
		}
	} //end flash=1
	
	close (fd);
	return 0;
}
