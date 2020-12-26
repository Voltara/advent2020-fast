#include "advent2020.h"

namespace {

struct row {
	std::array<__m256i, 2> R;

	row() : R() {
	}

	void flip(int x) {
		auto p = (uint64_t *) &R[0];
		p[x / 21] ^= 1LL << (3 * (x % 21));
	}

	int count() const {
		auto p = (uint64_t *) &R[0];
		int c = 0;
		for (int i = 0; i < 8; i++) {
			c += __builtin_popcountll(p[i]);
		}
		return c;
	}

	row operator + (const row &o) const {
		row r;
		r.R[0] = _mm256_add_epi64(R[0], o.R[0]);
		r.R[1] = _mm256_add_epi64(R[1], o.R[1]);
		return r;
	}

	row operator - (const row &o) const {
		row r;
		r.R[0] = _mm256_sub_epi64(R[0], o.R[0]);
		r.R[1] = _mm256_sub_epi64(R[1], o.R[1]);
		return r;
	}

	static auto com(__m256i v) {
		return _mm256_xor_si256(v, _mm256_set1_epi32(-1));
	}

	void next(const row &C) {
		const auto M = _mm256_set1_epi64x(0x1249249249249249);
		for (int i = 0; i < 2; i++) {
			auto r = _mm256_or_si256(com(C.R[i]), R[i]);
			r = _mm256_and_si256(r, _mm256_and_si256(
						_mm256_srli_epi64(com(r), 1),
						_mm256_srli_epi64(r, 2)));
			R[i] = _mm256_and_si256(r, M);
		}
	}

	row& shr(bool) {
		auto l = _mm256_srli_epi64(R[0], 3), cl = _mm256_slli_epi64(R[0], 60);
		auto h = _mm256_srli_epi64(R[1], 3), ch = _mm256_slli_epi64(R[1], 60);
		auto ml = _mm256_permute2x128_si256(ch, cl, _MM_SHUFFLE(0,0,0,3));
		auto mh = _mm256_permute2x128_si256(ch, ch, _MM_SHUFFLE(3,0,0,3));
		R[0] = _mm256_or_si256(l, _mm256_alignr_epi8(ml, cl, 8));
		R[1] = _mm256_or_si256(h, _mm256_alignr_epi8(mh, ch, 8));
		return *this;
	}

	row& shl(bool) {
		auto l = _mm256_slli_epi64(R[0], 3), cl = _mm256_srli_epi64(R[0], 60);
		auto h = _mm256_slli_epi64(R[1], 3), ch = _mm256_srli_epi64(R[1], 60);
		auto ml = _mm256_permute2x128_si256(cl, cl, _MM_SHUFFLE(0,0,3,0));
		auto mh = _mm256_permute2x128_si256(ch, cl, _MM_SHUFFLE(0,0,0,3));
		R[0] = _mm256_or_si256(l, _mm256_alignr_epi8(cl, ml, 8));
		R[1] = _mm256_or_si256(h, _mm256_alignr_epi8(ch, mh, 8));
		return *this;
	}

	row shr() const {
		return row{*this}.shr(true);
	}

	row shl() const {
		return row{*this}.shl(true);
	}
};

}

output_t day24(input_t in) {
	int part1 = 0, part2 = 0;

	std::vector<row> V(256), L(256), R(256);

	auto eq = [](__m256i v, char c) {
		auto eq = _mm256_cmpeq_epi8(v, _mm256_set1_epi8(c));
		uint32_t r = _mm256_movemask_epi8(eq);
		return uint64_t(r);
	};

	auto eq2 = [eq](__m256i l, __m256i h, char c) {
		return eq(l, c) | eq(h, c) << 32;
	};

	int min_y = INT32_MAX, max_y = INT32_MIN;
	while (in.len) {
		__m256i l = _mm256_loadu_si256((__m256i_u *) in.s);
		__m256i h = _mm256_loadu_si256((__m256i_u *) in.s + 1);

		// Find positions of all n, s, w, e
		auto n = eq2(l, h, 'n'), s = eq2(l, h, 's');
		auto w = eq2(l, h, 'w'), e = eq2(l, h, 'e');

		// Find line length and mask the values
		auto mask = n | s | w | e;
		mask = mask ^ (mask + 1);
		parse::skip(in, __builtin_ctzll(~mask));
		n &= mask, s &= mask, w &= mask, e &= mask;

		// Compute mapped hex coordinates
		int x = 84, y = 128;
		x += __builtin_popcountll(e & ~(n << 1));
		x -= __builtin_popcountll(w & ~(s << 1));
		y += __builtin_popcountll(s);
		y -= __builtin_popcountll(n);

		min_y = std::min(min_y, y);
		max_y = std::max(max_y, y);

		V[y].flip(x);
	}

	for (auto r : V) part1 += r.count();

	for (int step = 1; step <= 100; step++) {
		min_y--, max_y++;
		for (int i = min_y; i <= max_y; i++) R[i] = V[i] + V[i].shr();
		for (int i = min_y; i <= max_y; i++) L[i] = V[i] + V[i].shl();
		for (int i = min_y; i <= max_y; i++) {
			V[i].next(R[i] + L[i] - V[i] + R[i+1] + L[i-1]);
		}
	}

	for (auto r : V) part2 += r.count();

	return { part1, part2 };
}
