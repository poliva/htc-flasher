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
 * $Id: models.c,v 1.5 2007/05/15 22:03:19 pau Exp $
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
#include "main.h"

struct model_t models[4] = {
	{
		// HTC Hermes SPL 1.x (common base 1.50a)
		.name       = "hermes1",
		.flashNBH   = &hermes1_flashNBH,
		.flashRadio = &hermes1_flashRadio, // 1.04 only
		.dumpRadio  = &hermes1_dumpRadio, // 1.04 only
		.progressbar= &spl_progressbar
	}, {
		// HTC Hermes SPL-1.11 & 2.x (common base 1.51d)
		.name       = "hermes2",
		.flashNBH   = &hermes2_flashNBH,
		.flashRadio = NULL,
		.dumpRadio  = NULL,
		.progressbar= &spl2_progressbar
	}, {
		// HTC Trinity (common base 1.51d)
		.name       = "trinity",
		.flashNBH   = &hermes2_flashNBH,
		.flashRadio = NULL,
		.dumpRadio  = NULL,
		.progressbar= &spl2_progressbar

	} , {
		.name       = NULL
	}
};
struct model_t *model;

model_t *resolve_model(char *name)
{
	int i;

	for(i=0; models[i].name; i++)
		if (!strcmp(models[i].name, name))
			return &models[i];

	return NULL;
}
