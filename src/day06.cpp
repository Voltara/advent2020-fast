#include "advent2020.h"

output_t day06(input_t in) {
	int part1 = 0, part2 = 0;

	uint32_t all_or = 0, all_and = -1;

	auto check = [&]() {
		part1 += __builtin_popcount(all_or);
		part2 += __builtin_popcount(all_and);
	};

	while (in.len) {
		auto start = in.s;
		uint32_t mask = 0;
		for (; *in.s >= 'a'; in.s++, in.len--) {
			mask |= 1 << (*in.s - 'a');
		}
		if (in.s == start) {
			check();
			all_or = 0, all_and = -1;
		} else {
			all_or |= mask, all_and &= mask;
		}
		if (*in.s == '\r') in.s++, in.len--;
		in.s++, in.len--;
	}

	// Final group
	check();

	return { part1, part2 };
}
