#include "advent2020.h"

output_t day15(input_t in) {
	std::vector<int> seed;

	while (in.len) {
		seed.push_back(parse::positive<int>(in));
		parse::skip(in, 1);
	}

	constexpr int LIMIT1 = 2020, LIMIT2 = 30000000;

	// Try to get 2MB pages if possible
	hugemem alloc(LIMIT2 * sizeof(int));
	auto last = (int *) alloc.ptr;

	auto cur = seed.back(), i = 1;

	// Use standard pages to avoid competing for limited 2MB TLB entries
	auto filter = new std::bitset<LIMIT2>;

	auto step = [&] {
		if (cur < (i >> 6)) {
			// Skip the filter for small values
			cur = std::exchange(last[cur], i);
			if (cur) cur = i - cur;
		} else if (filter->test(cur)) {
			cur = i - std::exchange(last[cur], i);
		} else {
			filter->set(cur);
			last[cur] = i;
			cur = 0;
		}
	};

	seed.pop_back();
	for (auto k : seed) filter->set(k), last[k] = i++;

	for (; i < LIMIT1; i++) step();
	int part1 = cur;

	for (; i < LIMIT2; i++) step();
	int part2 = cur;

	delete filter;

	return { part1, part2 };
}
