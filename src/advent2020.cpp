#include "advent2020.h"

#ifdef _GNU_SOURCE
#include <sys/mman.h>
#include <linux/mman.h>
#endif

hugemem::hugemem(int _sz) : is_mmap(true), sz(), ptr(NULL) {
	static bool warned = false;
	constexpr int pagesize = 1 << 21; // 2MB
	sz = (_sz + pagesize - 1) & ~(pagesize - 1);

#ifdef _GNU_SOURCE
	ptr = (int64_t *) mmap(NULL, sz, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_POPULATE|MAP_HUGETLB|MAP_HUGE_2MB, -1, 0);
	if (ptr == MAP_FAILED) {
		ptr = NULL;
	}
#endif
	if (!ptr) {
		is_mmap = false;
		ptr = new int64_t[sz / 8];
		if (!warned) {
			warned = true;
			printf("Warning: Unable to allocate huge pages!\n");
		}
	}
}

hugemem::~hugemem() {
#ifdef _GNU_SOURCE
	if (is_mmap) {
		munmap(ptr, sz);
		return;
	}
#endif
	delete ptr;
}
