#include "advent2020.h"

output_t day02(input_t in) {
	int part1 = 0, part2 = 0;

	auto eq = [](__m256i v, char c) {
		auto eq = _mm256_cmpeq_epi8(v, _mm256_set1_epi8(c));
		return uint32_t(_mm256_movemask_epi8(eq));
	};

	while (in.len) {
		int from = parse::positive<int>(in);
		int to = parse::positive<int>(in);

		if (!from || to < from) break;

		// " x: 12345678"
		if (in.len < 4 + to) break;

		char c = in.s[1];
		in.s += 3, in.len -= 3;

		// Use SIMD instructions to match characters
		__m256i v = _mm256_loadu_si256((__m256i_u *) in.s);

		// Find the line length
		auto len = __builtin_ctz(eq(v, '\n'));
		parse::skip(in, len);

		// Find all matching characters on current line
		int match = eq(v, c) & ((1 << len) - 1);

		// Part 1: Check if number of matches is in range
		int count = __builtin_popcount(match);
		part1 += (count >= from) && (count <= to);

		// Part 2: Either or
		part2 += 1 & ((match >> from) ^ (match >> to));
	}

	return { part1, part2 };
}
