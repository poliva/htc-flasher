/*
 * Copyright (C) 2007
 *       xda-developers.com
 *
 * Author:
 *       Pau Oliva - pof <pof@eslack.org>
 *
 * Contributors:
 *       pancake - <pancake@youterm.com>
 *
 * $Id: decode.c,v 1.4 2007/05/15 22:03:19 pau Exp $
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

#include <string.h>
#include "main.h"

void DecodeSecurityBuffer (unsigned char *szBuffer) { //By TheBlasphemer
	unsigned char pSolution[16];
	memset (pSolution, 20, sizeof (pSolution));
	unsigned long nFirstCrc = Crc32 (szBuffer, 16, 0);
	unsigned long nSecondCrc = Crc32 (szBuffer, 32, 0);
	unsigned long nThirdCrc = nFirstCrc + nSecondCrc;
	memcpy (pSolution, &nFirstCrc, sizeof (unsigned long));
	memcpy (&pSolution[4], &nSecondCrc, sizeof (unsigned long));
	memcpy (&pSolution[8], &nThirdCrc, sizeof (unsigned long));
	unsigned long nFourthCrc = 0;
	unsigned long i;
	for (i = 0; i < 12; i++) nFourthCrc += pSolution[i];
	memcpy (&pSolution[12], &nFourthCrc, sizeof (unsigned long));
	for (i = 0; i < 16; i++) {
		if (pSolution[i] < '0') pSolution[i] = '0';
		else if (pSolution[i] > '~') pSolution[i] = '~';
		else if (pSolution[i] == '=') pSolution[i] = '>';
		else if (pSolution[i] == '?') pSolution[i] = '@';
	}
	memset (szBuffer, 0, 32);
	memcpy (szBuffer, pSolution, 16);
}

void DecodeRadioSecurityBuffer (unsigned char *pBuffer) { //By TheBlasphemer
	unsigned char pSillyStuff[]={0x1F,0x00,0x1E,0x01,0x1D,0x16,0x0A,0x0F,0x0E,0x08,0x03,0x19,0x06,0x17,0x11,0x0B,0x0A,0x11,0x00,0x16,0x04,0x0C,0x1D,0x07,0x0F,0x1A,0x01,0x0B,0x0D,0x10,0x17,0x1F};
	unsigned char pTraditionalBuffer[32];
	unsigned long i;
	for (i = 0; i < 32; i++) pTraditionalBuffer[i] = 0xCF - (i * 4);
	for (i = 0; i < 16; i++) pTraditionalBuffer[i * 2] = pBuffer[pSillyStuff[i]];
	DecodeSecurityBuffer (pTraditionalBuffer);
	for (i = 0; i < 32; i++) pBuffer[i] = 0xFF - (i * 3);
	for (i = 0; i < 16; i++) pBuffer[pSillyStuff[16 + i]] = pTraditionalBuffer[i];
}
