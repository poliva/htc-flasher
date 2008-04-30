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
 * $Id: utils.c,v 1.5 2007/05/16 19:42:29 pau Exp $
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

static int control_cs = 5;
int control_c(int sig)
{
	control_cs--;
	if (control_cs == 0)
		exit (1);
	printf("^C%d\n", control_cs);
	return 0;
}

void spl2_progressbar(int pc)
{
	char msg[100];
	sprintf(msg, "progress %d", pc);
	fsend(msg);
}

void spl_progressbar(int pc)
{
	int tmp, cols = 23;
	char *aux;
	char msg[100];
	char msg2[200];

	aux=msg;
	(pc<0)?pc=0:(pc>100)?pc=100:0;
	sprintf(aux,"%.3d%% [", pc);
	aux=aux+strlen(aux);
	cols-=7;
	for(tmp=(cols*pc/100);tmp;tmp--) sprintf(aux++,"#");
	for(tmp=cols-(cols*pc/100);tmp;tmp--) sprintf(aux++,"-");
	sprintf(aux,"]");
	sprintf (msg2,"shmsg 11 4 \"%s\"",msg);
	//printf ("%s\n",msg2);
	send (msg2);
	get("CRLF 1");
}

void progressbar(int pc)
{
	char *columns = getenv("COLUMNS");
	int tmp, cols = 80;

	(pc<0)?pc=0:(pc>100)?pc=100:0;
	printf("\e[K %3d%% [", pc);
	if (columns)
		cols = atoi(columns);
	cols-=10;
	for(tmp=cols*pc/100;tmp;tmp--) printf("#");
	for(tmp=cols-(cols*pc/100);tmp;tmp--) printf("-");
	printf("]\r");
	fflush(stdout);
}

void strip(char *string) {
        int i=0;
        for(i=0;string[i]==' ';i++);
        if (i)strcpy(string,string+i);
        for(i=strlen(string)-1;i&&string[i]==' ';i--)string[i]='\0';
}

/*
int strip(char *string)
{
        int i,j=0;
        char bu[100];


        if((string == NULL) || (strlen(string) <= 0) || (strlen(string) > sizeof(bu)))
                return 0;

        for (i=0;i<strlen(string);i++){

                if (string[i]!=32) {  //remove space
                        if ( (string[i] < 33) || (string[i] > 126) ) break;
                        else bu[j]=string[i];
                        j++;
                }
        }
        bu[j+1]=0;
        memcpy(str,bu,strlen(bu)); //str must be global
        return 0;
}
*/
