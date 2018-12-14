#ifndef MPMALLOC_H
#define MPMALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

void* mpcalloc(size_t nmemb, size_t size);
void* mpmalloc(size_t size);
void* mprealloc(void* addr, size_t size);
void mpfree(void* addr);

#ifdef __cplusplus
}
#endif

#endif
