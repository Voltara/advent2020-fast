#include "advent2020.h"

constexpr int64_t MOD = 20201227;

static int modpow(int64_t n, int e) {
	int64_t p = 1;
	for (; e; e >>= 1) {
		if (e & 1) (p *= n) %= MOD;
		(n *= n) %= MOD;
	}
	return p;
}

static int dlog116099(int n) {
	constexpr int R = 341;

	auto H = [](int64_t n) {
		return ((1295*n) ^ (n>>8) ^ (n>>21)) % 2837;
	};

	std::array<int,2837> Dk{}, Dv{};
	for (int64_t br = 1, r = 0; r < R; r++) {
		int h = H(br);
		Dk[h] = br, Dv[h] = r;
		br = br * 19372176 % MOD;
	}

	int64_t y = modpow(n, 174);
	for (int i = 0; i < R; i++) {
		auto h = H(y);
		if (Dk[h] == y) {
			return i * R + Dv[h];
		}
		y = y * 16585664 % MOD;
	}

	return 0;
}

static int dlog29(int n) {
	const __m256i L = _mm256_set_epi64x(
			0xffffff1a1c6615dc, 0x696ac3fae8143b9e,
			0x8018b1cea1f4be4c, 0x757d308c0064b001);
	__m256i v = _mm256_set1_epi8(modpow(n, 696594) % 255);
	return __builtin_ctz(_mm256_movemask_epi8(_mm256_cmpeq_epi8(L, v)));
}

static int dlog3(int n) {
	return modpow(n, 6733742) >> 5 & 3;
}

static int dlog2(int n) {
	return modpow(n, 10100613) != 1;
}

static int dlog(int n) {
	int64_t l = 0;
	l += 18227544LL * dlog116099(n);
	l += 18808038LL * dlog29(n);
	l += 13467484LL * dlog3(n);
	l += 10100613LL * dlog2(n);
	return l % (MOD - 1);
}

output_t day25(input_t in) {
	int k0 = 0, k1 = 0;

	for (; in.len; parse::skip(in, 1)) {
		if (*in.s == '\n') {
			std::swap(k0, k1);
		} else {
			k0 = 10 * k0 + *in.s - '0';
		}
	}

	auto part1 = modpow(k0, dlog(k1));

	return { part1, 0 };
}
