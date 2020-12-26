#ifndef _ADVENT2020_PARSE_H
#define _ADVENT2020_PARSE_H

namespace parse {

static void skip(input_t &in, int n) {
	if (in.len < n) abort();
	in.s += n, in.len -= n;
}

template<typename T>
static T positive(input_t &in) {
	bool have = false;
	for (T n = 0; in.len; in.s++, in.len--) {
		uint8_t d = *in.s - '0';
		if (d <= 9) {
			n = 10 * n + d;
			have = true;
		} else if (have) {
			return n;
		}
	}
	return 0;
}

// Fixed-width positive integer
template<typename T>
static T positive(input_t &in, int len, bool strict) {
	T n = 0;
	if (in.len < len) return 0;
	for (; len--; in.s++, in.len--) {
		uint8_t d = *in.s - '0';
		if (d > 9) return 0;
		n = 10 * n + d;
	}

	// In strict mode, reject any prefix of a longer number
	if (strict && in.len && *in.s >= '0' && *in.s <= '9') {
		return 0;
	}

	return n;
}

// Extract the high bits after shifting each of the next eight bytes left
static inline int octet(input_t &in, int shift) {
	auto m = *reinterpret_cast<__m64 *>(in.s);
	parse::skip(in, 8);
	return _mm_movemask_pi8(_mm_slli_si64(m, shift)); // SSE
}

}

#endif
