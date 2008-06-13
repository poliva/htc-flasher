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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "main.h"

unsigned long magicHeader[]={'H','T','C','I','M','A','G','E'};
const char magicHeader2[]={'R','0','0','0','F','F','\n'};

struct HTCIMAGEHEADER {
	char device[32];
	unsigned long sectiotypes[32];
	unsigned long sectiooffsets[32];
	unsigned long sectiolengths[32];
	char CID[32];
	char version[16];
	char language[16];
};
struct HTCIMAGEHEADER HTCIMAGEHEADER;

/* getSectionName - returns the name based on ID */
const char *getSectionName(unsigned long section)
{
	if (section == 0x100)
		return "IPL";
	if (section == 0x101)
		return "G3IPL";
	if (section == 0x102)
		return "G4IPL";
	if (section == 0x200)
		return "SPL";
	if (section == 0x201)
		return "G3SPL";
	if (section == 0x202)
		return "G4SPL";
	if (section == 0x300 || section == 0x301)
		return "GSM";
	if (section == 0x400 || section == 0x401)
		return "OS";
	if (section == 0x600)
		return "MainSplash";
	if (section == 0x601)
		return "SubSplash";
	if (section == 0x700 || section == 0x900)
		return "ExtROM";
	return "unknown";
}

/* bufferedReadWrite - dump rom file parts */
int bufferedReadWrite(FILE *input, FILE *output, unsigned long length)
{
	unsigned char data[2048];
	unsigned long nread;

	while (length > 0) {
		nread = length;
		if (nread > sizeof(data))
			nread = sizeof(data);
		nread = fread(data, 1, nread, input);
		if (!nread)
			return 0;
		if (fwrite(data, 1, nread, output) != nread)
			return 0;
		length -= nread;
	}
	return 1;
}

/* extractNB - extract NB files from NBH */
int extractNB(FILE *input, int index, unsigned long type, unsigned long start, unsigned long len)
{
	int retval=0;
	char filename[1024];
	FILE *output;

	sprintf(filename, "%02d_%s.nb", index, getSectionName(type));
	printf("[] Extracting: %s\n", filename);

	output = fopen(filename, "wb");
	if (output == NULL)
		fprintf(stderr, "[!!] Could not open '%s'\n", filename);

	if (fseek(input, start, SEEK_SET) != 0)
		fprintf(stderr, "[!!] Could not seek to position %08lX\n", start);

	if (bufferedReadWrite(input, output, len))
		retval = 1;
	else
		fprintf(stderr, "[!!] Could not read complete file\n");

	fclose(output);

	return retval;
}

/* extractNBH - Extract all ROM parts from NBH file */
void extractNBH(char *filename)
{
	int i;
	FILE *input;
	FILE *output;
	FILE *tmpfile;
	char magic[5000];
	unsigned char signature[16];
	unsigned long blockIndex = 0;
	unsigned long offset = 0;
	unsigned long blockLen;
	unsigned long signLen;
	unsigned char flag;
	unsigned char blockSign[5000];
	struct HTCIMAGEHEADER header;

	/* NBH2DBH */
	input = fopen(filename,"rb");
	if (input == NULL) {
		fprintf(stderr, "[!!] Could not open '%s'\n", filename);
		exit(1);
	}

	if (fread(magic, 1, sizeof(magicHeader2), input) != sizeof(magicHeader2)) {
		fprintf(stderr, "[!!] '%s' is not a valid NBH file\n",filename);
		fclose(input);
		exit(1);
	}

	if (memcmp(magicHeader2, magic, sizeof(magicHeader2)) != 0) {
		fprintf(stderr, "[!!] '%s' is not a valid NBH file\n",filename);
		fclose(input);
		exit(1);
	}

	if (fread(signature, 1, 16, input) != 16) {
		fprintf(stderr, "[!!] Could not read initial signature\n");
		fclose(input);
		exit(1);
	}

	if (DEBUG) {
		printf("Initial signature: ");
		for (i = 0; i < 16; i++)
			printf("%02X", signature[i]);
		printf("\n");
	}

	output=fopen("tempfile.dbh","wb");
	if (output == NULL) {
		fprintf(stderr, "[!!] Could not open 'tempfile.dbh'\n");
		fclose(output);
		exit(1);
	}

	blockIndex = 0;
	offset = 0;

	while (!feof(input)) {

		if (fread(&blockLen, 4, 1, input) && fread(&signLen, 4, 1, input) && fread(&flag, 1, 1, input)) {

			if (!bufferedReadWrite(input, output, blockLen)) {
				fprintf(stderr, "[!!] Error in block %ld (%08lX - %08lX)\n", blockIndex, offset, offset + blockLen);
				break;
			}

			if (fread(blockSign, 1, signLen, input) != signLen) {
				fprintf(stderr, "Error in block signature %ld\n",blockIndex);
				memset(blockSign, 0, sizeof(blockSign));
				break;
			}

			if (DEBUG) {
				printf("Signature for block %04ld (%08lX - %08lX): ", blockIndex, offset, offset + blockLen);
				for (i = 0; i < signLen; i++)
					printf("%02X", blockSign[i]);
				printf("\n");
			}

		} else {
			fprintf(stderr, "[!!] Error in block %ld\n", blockIndex);
			break;
		}

		blockIndex++;
		offset += blockLen;

		/* end */
		if (flag == 0x02)
			break;
	}
	fclose(output);
	fclose(input);

	/* DBHExtract */
	tmpfile = fopen("tempfile.dbh", "rb");
	if (tmpfile == NULL) {
		fprintf(stderr, "[!!] Could not open 'tempfile.dbh'\n");
		fclose(tmpfile);
		exit(1);
	}

	if (fread(magic,1,sizeof(magicHeader),tmpfile)!=sizeof(magicHeader)) {
		fprintf(stderr, "[!!] tempfile.dbh is not a valid .dbh file\n");
		fclose(tmpfile);
		exit(1);
	}

	if (memcmp(magicHeader,magic,sizeof(magicHeader))!=0) {
		fprintf(stderr, "[!!] tempfile.dbh is not a valid .dbh file\n");
		fclose(tmpfile);
		exit(1);
	}

	if (fread(&header,1,sizeof(HTCIMAGEHEADER),tmpfile)!=sizeof(HTCIMAGEHEADER)) {
		fprintf(stderr,"[!!] Could not read header\n");
		fclose(tmpfile);
		exit(1);
	}

	printf("\n");
	printf("Device:   %s\n", header.device);
	printf("CID:      %s\n", header.CID);
	printf("Version:  %s\n", header.version);
	printf("Language: %s\n", header.language);
	printf("\n");

	for (i = 0; i < (sizeof(header.sectiotypes) / sizeof(header.sectiotypes[0])); i++) {
		if (header.sectiotypes[i]!=0) {
			if (!extractNB(tmpfile, i, header.sectiotypes[i], header.sectiooffsets[i], header.sectiolengths[i]))
				fprintf(stderr,"[!!] Error while extracting file %d\n", i);
		}
	}

	printf ("[] Done!\n");

	fclose(tmpfile);
	unlink("tempfile.dbh");
}
