#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "misc_utils.h"

int getfsize(char* filename)
{
	FILE* fp_src;
	int src_size;
	
	if(!(fp_src = fopen(filename, "rb"))) return 0;
	fseek(fp_src, 0, SEEK_END);
	src_size = ftell(fp_src);
	fclose(fp_src);
	
	return src_size;
}

unsigned char* file2mem(char* filename)
{
	FILE* fp_src;
	unsigned char* buf;
	int src_size;
	
	if(!(fp_src = fopen(filename, "rb"))){
		printf("Error! Source file %s not found. :(\n", filename);
		return 0;
	}
	
	fseek(fp_src, 0, SEEK_END);
	src_size = ftell(fp_src);
	fseek(fp_src, 0, SEEK_SET);
	
	buf = malloc(src_size);
	fread(buf, src_size, 1, fp_src);
	fclose(fp_src);
	
	return buf;	
}

void* memmem(const void* haystack, size_t hlen, const void* needle, size_t nlen)
{
	int needle_first;
	const void* p = haystack;
	size_t plen = hlen;

	if (!nlen) return NULL;

	needle_first = *(unsigned char *)needle;

	while (plen >= nlen && (p = memchr(p, needle_first, plen - nlen + 1)))
	{
		if (!memcmp(p, needle, nlen)) return (void *)p;
		p++;
		plen = hlen - (p - haystack);
	}
	
	return NULL;
}

int cmp_int(const void* a, const void* b)
{
	return (*(int*)a - *(int*)b);
}