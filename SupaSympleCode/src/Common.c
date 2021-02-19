#include "SupaSyC/Common.h"

bool StringStartsWith(const char *str, const char *start)
{
	uint32_t strLen = strlen(str), startLen = strlen(start);
	bool sz = strLen >= startLen;
	int exact = strncmp(str, start, startLen);
	return sz && !exact;
}

void *_Alloc(uint32_t count, uint32_t sz)
{
	void *ptr = calloc(count, sz);
	assert(ptr);
	return ptr;
}