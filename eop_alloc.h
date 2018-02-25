/* end of page allocator
 * see https://www.gamasutra.com/blogs/NiklasGray/20171107/309071/Virtual_Memory_Tricks.php
 */

#ifndef _EOP_ALLOC_H_
#define _EOP_ALLOC_H_

#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

/* FIXME */
#define EOP_ARCH_PAGE_SIZE 4096

struct malloc_info {
	unsigned npage;	/* including meta page and guard page */
};

static void set_ro(void *ptr)
{
	mprotect(ptr, EOP_ARCH_PAGE_SIZE, PROT_READ);
}
static void set_rw(void *ptr)
{
	mprotect(ptr, EOP_ARCH_PAGE_SIZE, PROT_READ|PROT_WRITE);
}
/* |meta page(ReadOnly)|content pages(ReadWrite)|guard page(ReadOnly)| */
static void * eop_malloc(size_t size)
{
	void *meta, *content, *guard;
	struct malloc_info *mi;
	size_t aligned_size;

	aligned_size = (size + EOP_ARCH_PAGE_SIZE - 1) / EOP_ARCH_PAGE_SIZE * EOP_ARCH_PAGE_SIZE;
	if (aligned_size == 0)
		return NULL;
	
	if (posix_memalign(&meta, EOP_ARCH_PAGE_SIZE, aligned_size + EOP_ARCH_PAGE_SIZE * 2) != 0)
		return NULL;

	content = meta + EOP_ARCH_PAGE_SIZE;
	guard = content + aligned_size;

	mi = (struct malloc_info *)meta;
	mi->npage = aligned_size / EOP_ARCH_PAGE_SIZE + 1/*meta page*/ + 1/*guard page*/;

	set_ro(meta);
	set_ro(guard);

	return content + (aligned_size - size);
}
static void eop_free(void *ptr)
{
	void *meta, *content, *guard;
	struct malloc_info *mi;

	if (!ptr)
		return;
	
	content = (void*)((long)ptr & ~(EOP_ARCH_PAGE_SIZE - 1));
	meta = content - EOP_ARCH_PAGE_SIZE;
	mi = (struct malloc_info *)meta;
	guard = meta + (mi->npage - 1) * EOP_ARCH_PAGE_SIZE;

	set_rw(meta);
	set_rw(guard);

	free(meta);
}
/* FIXME */
static void *eop_calloc(size_t nmemb, size_t size)
{
	return NULL;
}

#endif
