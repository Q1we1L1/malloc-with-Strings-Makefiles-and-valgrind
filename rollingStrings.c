/*
 * rollingString.c
 * Author: Qiwei Li
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NR_SLOT	16
#define FMT_LEN	64

char *readOneStr(FILE *fp, int *err)
{
	char *s = NULL, fmt[FMT_LEN], ch;
	int len;
	int ret = 0, _ret;

	/* Read the length */
	while ((_ret = fscanf(fp, "%d", &len)) != EOF) {
		if (_ret == 0) {
			fscanf(fp, "%c", &ch);
			printf("OOPS: Non-numeric input detected.  The byte was: 0x%02x='%c'.\n", ch, ch);
			ret = 1;
			continue;
		}

		if (len <= 0) {
			printf("OOPS: The length read from input, %d, was zero or negative.\n", len);
			ret = 1;
			continue;
		}

		break;
	}
	if (_ret == EOF)
		goto out;

	/* Allocate memory for the string */
	s = malloc(sizeof(char) * (len + 1));
	if (!s) {
		printf("ERROR in readOnStr: malloc() failure\n");
		ret = 1;
		goto out;
	}

	/* Prepare format and then read the string */
	sprintf(fmt, "%%%ds", len);
	_ret = fscanf(fp, fmt, s);
	if (_ret != 1) {
		printf("OOPS: Hit EOF when trying to read a string of length %d.\n", len);
		ret = 1;
		/* Free and reset s */
		free(s);
		s = NULL;
	}

out:
	if (ret)
		*err = 1;
	return s;
}

void dumpStrs(char *arrayOfStrings[])
{
	int i;

	printf("Current strings:\n");
	for (i = 0; i < NR_SLOT; i++)
		printf("  %d: %s\n", i, arrayOfStrings[i] ? arrayOfStrings[i] : "<null>");
}

int cmpStr(const void *a, const void *b)
{
	return strcmp(*(const char **)a, *(const char **)b);
}

int main(int argc, char *argv[])
{
	char *arrayOfStrings[NR_SLOT] = {NULL};
	int ret = 0, i;
	int pos = 1;

	/* Check number of parameters */
	printf("There are %d command-line arguments\n", argc - 1);
	if (argc <= 1)
		return 0;

	/* Init the arrayOfStrings */
	arrayOfStrings[0] = "HEAD";
	arrayOfStrings[NR_SLOT - 1] = "TAIL";

	/* Process input filename(s) one by one */
	for (i = 1; i < argc; i++) {
		FILE *fp;
		char *s;

		printf("--- FILE: %s (argv[%d]) ---\n", argv[i], i);
		fp = fopen(argv[i], "r");
		if (!fp) {
			fprintf(stderr, "ERROR: Cannot open the file '%s'\n", argv[i]);
			ret = 1;
			continue;
		}

		while ((s = readOneStr(fp, &ret)) != NULL) {
			/* This slot is empty, fill it */
			if (!arrayOfStrings[pos]) {
				arrayOfStrings[pos] = s;

				/* Sort the array */
				qsort(arrayOfStrings + 1, pos, sizeof(char *), cmpStr);

				pos++;
				/* Reset pos to 1 if all slots are filled */
				if (pos >= NR_SLOT - 1)
					pos = 1;
			} else {    /* All slots are filled */
				/* Free the old string if exists */
				free(arrayOfStrings[pos]);

				/* The pos should always be 1 */
				arrayOfStrings[pos] = s;

				/* Sort the array */
				qsort(arrayOfStrings + 1, NR_SLOT - 2, sizeof(char *), cmpStr);
			}

			dumpStrs(arrayOfStrings);
		}

		printf("--- END OF FILE ---\n");
		fclose(fp);
	}

	/* Free the arrayOfStrings */
	for (i = 1; i < NR_SLOT - 1; i++) {
		if (arrayOfStrings[i])
			free(arrayOfStrings[i]);
	}

	return ret;
}
