/*
 * compareBytes.c
 * Author: Qiwei Li
 */

#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void printChar(unsigned char c)
{
	if (isprint(c))
		printf("0x%02x='%c'", c, c);
	else
		printf("0x%02x=<unprintable>", c);
}

/* The caller ensures c is not EOF */
void dumpNext(char *filename, FILE *fp, unsigned char c)
{
	int chs[4], ch;
	int nr_ch = 0;
	int i;

	/* Record the first char which is already read */
	chs[nr_ch++] = c;


	/* Read the remaining chars */
	while ((nr_ch < 4) && ((ch = fgetc(fp)) != EOF))
		chs[nr_ch++] = ch;

	if (nr_ch < 4)
		printf("'%s' only has %d bytes left:", filename, nr_ch);
	else
		printf("The next 4 bytes of '%s' are:", filename);

	/* Print the first char */
	printChar(chs[0]);
	/* Print the remaining chars */
	for (i = 1; i < nr_ch; i++) {
		printf(" ");
		printChar(chs[i]);
	}
	printf("\n");
}

int main(int argc, char *argv[])
{
	FILE *f1 = NULL, *f2 = NULL;
	int c1 = EOF, c2 = EOF;
	int pos = 0;
	int ret = 1;

	/* Exactly 3 parameters are needed */
	if (argc != 3) {
		fprintf(stderr, "SYNTAX: %s <file1> <file2>\n", argv[0]);
		return 1;
	}

	/* Open and check the two files */
	f1 = fopen(argv[1], "r");
	if (!f1) {
		perror("Cannot open first file");
		goto out;
	}

	f2 = fopen(argv[2], "r");
	if (!f2) {
		perror("Cannot open second file");
		goto out;
	}

	/* Read and compare the two files byte by byte */
	while (1) {
		c1 = fgetc(f1);
		c2 = fgetc(f2);

		/* Break if the two files diff or both reach EOF */
		if ((c1 != c2) || (c1 == EOF && c2 == EOF))
			break;

		pos++;
	}

	/* Case 0: no differences */
	if (c1 == EOF && c2 == EOF) {
		ret = 0;
		printf("No differences found.\n");
		goto out;
	}

	/* Case 1: one file is shorter */
	if (c1 == EOF) {
		printf("The file '%s' was shorter than '%s'.\n", argv[1], argv[2]);
		dumpNext(argv[2], f2, c2);
	} else if (c2 == EOF) {
		printf("The file '%s' was shorter than '%s'.\n", argv[2], argv[1]);
		dumpNext(argv[1], f1, c1);
	} else {    /* Case 2: two files differ */
		printf("Difference found at byte %d.\n", pos);
		dumpNext(argv[1], f1, c1);
		dumpNext(argv[2], f2, c2);
	}

out:
	if (f1)
		fclose(f1);
	if (f2)
		fclose(f2);
	return ret;
}
