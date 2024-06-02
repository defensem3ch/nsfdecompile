#ifdef DEBUG
	#define dprintf printf
#else
	#define dprintf(format, args...) ((void)0)
#endif

int getfsize(char* filename);
unsigned char* file2mem(char* filename);
void* memmem(const void* haystack, size_t hlen, const void* needle, size_t nlen);

int cmp_int(const void* a, const void* b);
