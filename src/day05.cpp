#include "advent2020.h"

// cumulative xor of 0..n
static int cxor(int n) {
	return (n & (((n << 1) & 2) - 1)) ^ ((n >> 1) & 1);
}

output_t day05(input_t in) {
	// SSSE3
	auto to_bits = [](char *s) {
		__m128i v = _mm_loadu_si128((__m128i *) s);
		v = _mm_shuffle_epi8(v, _mm_set_epi64x(
					0x0001, 0x0203040506070809));
		v = _mm_slli_epi64(v, 5);
		return ~_mm_movemask_epi8(v) & 0x3ff;
	};

	int min = 0x3ff, max = 0, xor_all = 0;

	for (; in.len >= 10; in.s += 11, in.len -= 11) {
		if (*in.s < ' ') in.s++, in.len--; // In case of CRLF
		int n = to_bits(in.s);
		min = std::min(min, n);
		max = std::max(max, n);
		xor_all ^= n;
	}

	int part1 = max;
	int part2 = xor_all ^ cxor(min - 1) ^ cxor(max);

	return { part1, part2 };
}
