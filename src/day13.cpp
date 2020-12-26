#include "advent2020.h"

output_t day13(input_t in) {
	auto start = parse::positive<int64_t>(in);
	parse::skip(in, 1);

	int64_t part1 = 0, part2 = 0, min_wait = INT64_MAX, prod = 1;

	for (int idx = 0; in.len; idx++) {
		if (*in.s == 'x') {
			parse::skip(in, 2);
			continue;
		}

		auto mod = parse::positive<int64_t>(in);
		parse::skip(in, 1);

		// Part 1
		auto wait = fastmod(mod - start % mod, mod);
		if (wait == mod) wait = 0;
		if (wait < min_wait) {
			part1 = mod * wait;
			min_wait = wait;
		}

		// Part 2
		part2 -= modinv(prod, mod) * (part2 + idx) % mod * prod;
		prod *= mod;
	}

	// Fixup Part 2 if it's negative
	part2 += prod & -(part2 < 0);

	return { part1, part2 };
}
