#include <stdlib.h>
#include "driver/stdout.h"
#include "lib/mpmalloc.h"

void* mpcalloc(size_t nmemb, size_t size)
{
	void* ret = calloc(nmemb, size);
#ifdef ADDR_20BIT
	kout << "calloc:" << dec << (uint32_t)nmemb << "x" << (uint32_t)size << "@" << ret << endl;
#else
	kout << "calloc:" << dec << nmemb << "x" << size << "@" << ret << endl;
#endif
	return ret;
}

void* mpmalloc(size_t size)
{
	void* ret = malloc(size);
#ifdef ADDR_20BIT
	kout << "malloc:" << dec << (uint32_t)size << "@" << ret << endl;
#else
	kout << "malloc:" << dec << size << "@" << ret << endl;
#endif
	return ret;
}

void* mprealloc(void* addr, size_t size)
{
	void* ret = realloc(addr, size);
#ifdef ADDR_20BIT
	kout << "realloc:" << addr << ":" << dec << (uint32_t)size << "@" << ret << endl;
#else
	kout << "realloc:" << addr << ":" << dec << size << "@" << ret << endl;
#endif
	return ret;
}

void mpfree(void* addr)
{
	kout << "free:" << addr << endl;
	free(addr);
}
