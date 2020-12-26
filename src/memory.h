#ifndef _ADVENT2020_MEMORY
#define _ADVENT2020_MEMORY

class hugemem {
	bool is_mmap;
	int sz;

    public:
	int64_t *ptr;

	hugemem(int _sz);
	~hugemem();
};

#endif
