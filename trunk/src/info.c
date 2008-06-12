/* 
 * HTCFlasher
 *
 * Copyright (C) 2007-2008 Pau Oliva Fora - <pof@eslack.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3 as published
 * by the Free Software Foundation.
 *
 */

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "main.h"

/* getinfo - this should work on any modern HTC device */
void getinfo()
{
	char *result;

	send("info 2");
	get();
	result = parseHTCBlock();
	if (strlen(result) != 0)
		printf("[] CID: %s\n", result);

	send("getdevinfo");
	get();
	result = parseHTCBlock();
	if (strlen(result) != 0)
		printf("[] ModelID: %s\n", result);
	else {
		authBL();
		fsend("getdevinfo");
		get();
		result = parseHTCBlock();
		if (strlen(result) != 0)
			printf("[] ModelID: %s\n", result);
		else
			printf("[!!] Your device can't be identified!\n");
		fsend("ruurun 0");
		fsend("ruurun 0");
		get();
		fsend("ResetDevice");
		fsend("ResetDevice");
		get();
	}
}
