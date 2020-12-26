#include "advent2020.h"

output_t day03(input_t in) {
	std::vector<uint32_t> V;
	V.reserve(512);

	// AVX2 - convert 32 bytes of "..#..##...#...\n" to bitmask
	auto to_bits = [](char *s) {
		__m256i v = _mm256_loadu_si256((__m256i *) s);
		v = _mm256_cmpeq_epi8(v, _mm256_set1_epi8('#'));
		return _mm256_movemask_epi8(v);
	};

	/* Rows are all length 31.  Memory safety depends on the
	 * NUL-padding byte added by the input reader, in case
	 * the input is missing a final newline character.
	 */
	for (; in.len >= 31; in.s += 32, in.len -= 32) {
		V.push_back(to_bits(in.s));
	}

	// Count trees using the given (dx,dy) increment
	auto tree = [V](int dx, int dy) {
		int shift = 0, count = 0;
		for (int i = 0; i < V.size(); i += dy) {
			count += (V[i] >> shift) & 1;
			shift += dx;
			shift -= 31 & -(shift >= 31);
		}
		return count;
	};

	int64_t part1 = tree(3, 1);
	int64_t part2 = part1 * tree(1, 1) * tree(5, 1) * tree(7, 1) * tree(1, 2);

	return { part1, part2 };
}
