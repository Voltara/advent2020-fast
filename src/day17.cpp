#include "advent2020.h"

namespace {

// Masks: 0x11111..., 0x77777..., 0x88888..., 0xccccc...
constexpr auto M1 = __uint128_t(-1) / 15;
constexpr auto M7 = M1 * 7, M8 = M1 * 8, Mc = M1 * 12;

// Saturating add 4-bit fields (max value 8)
__uint128_t sadd(__uint128_t x, __uint128_t y) {
	auto s = (x + (y & M7)) | (y & M8);
	auto h = M8 | ((s >> 3) & M1);
	return s & (h - M1);
}

// 3D solver
struct solver1 {
	std::array<std::array<__uint128_t,20>,8> _G{}, _H{}, _I{};

	int y_min = 6, y_max = 14, z_max = 1;

	auto & G(int z, int y) { return _G[z][y]; }
	auto & H(int z, int y) { return _H[z][y]; }
	auto & I(int z, int y) { return _I[z][y]; }

	void step() {
		// x-sum
		for (auto z = 0; z < z_max; z++) {
			for (int y = y_min; y < y_max; y++) {
				auto r = G(z,y);
				H(z,y) = sadd(G(z,y), sadd(r<<4, r>>4));
			}
		}

		// y-sum
		for (auto z = 0; z < z_max; z++) {
			__uint128_t prev = 0;
			I(z,y_min-1) = H(z,y_min);
			for (auto y = y_min; y < y_max; y++) {
				auto r = H(z,y);
				I(z,y) = sadd(r, sadd(prev, H(z,y+1)));
				prev = r;
			}
			I(z,y_max) = prev;
		}
		y_min--, y_max++;

		// z-sum
		z_max++;
		for (auto z = 0; z < z_max - 1; z++) {
			for (auto y = y_min; y < y_max; y++) {
				H(z,y) = sadd(I(z,y), sadd(I(std::abs(z-1),y), I(z+1,y)));
			}
		}
		for (auto y = y_min; y < y_max; y++) {
			H(z_max-1,y) = I(z_max-2,y);
		}

		// transition rule
		for (auto z = 0; z < z_max; z++) {
			for (int y = y_min; y < y_max; y++) {
				auto r = (H(z,y) - G(z,y)) | G(z,y);
				r ^= Mc, r &= r >> 2, r &= r >> 1, r &= M1;
				G(z,y) = r;
			}
		}
	}

	int count() {
		int c = 0;
		for (int z = 0; z < z_max; z++) {
			for (int y = y_min; y < y_max; y++) {
				int add = 0;
				auto r = G(z,y);
				add += _mm_popcnt_u64(uint64_t(r));
				add += _mm_popcnt_u64(uint64_t(r>>64));
				add <<= !!z;
				c += add;
			}
		}
		return c;
	}
};

// 4D solver
struct solver2 {
	std::array<std::array<std::array<__uint128_t,20>,8>,8> _G{}, _H{}, _I{};

	int y_min = 6, y_max = 14, z_max = 1, w_max = 1;

	auto & G(int w, int z, int y) { return _G[w][z][y]; }
	auto & H(int w, int z, int y) { return _H[w][z][y]; }
	auto & I(int w, int z, int y) { return _I[w][z][y]; }

	void step() {
		// x-sum
		for (auto w = 0; w < w_max; w++) {
			for (auto z = w; z < z_max; z++) {
				for (int y = y_min; y < y_max; y++) {
					auto r = G(w,z,y);
					H(w,z,y) = sadd(G(w,z,y), sadd(r<<4, r>>4));
				}
			}
		}

		// y-sum
		for (auto w = 0; w < w_max; w++) {
			for (auto z = w; z < z_max; z++) {
				__uint128_t prev = 0;
				I(w,z,y_min-1) = H(w,z,y_min);
				for (auto y = y_min; y < y_max; y++) {
					auto r = H(w,z,y);
					I(w,z,y) = sadd(r, sadd(prev, H(w,z,y+1)));
					prev = r;
				}
				I(w,z,y_max) = prev;
			}
		}
		y_min--, y_max++;

		// z-sum
		for (auto y = y_min; y < y_max; y++) {
			for (auto w = 0; w < w_max; w++) {
				auto prev = w ? I(w-1,w,y) : I(0,1,y);
				for (auto z = w; z < z_max; z++) {
					auto r = I(w,z,y);
					H(w,z,y) = sadd(r, sadd(prev, I(w,z+1,y)));
					prev = r;
				}
				H(w,z_max,y) = prev;
				if (w) H(w,w-1,y) = sadd(I(w-1,w,y), sadd(I(std::abs(w-2),w,y), I(w,w,y)));
			}
		}
		z_max++;

		// w-sum
		for (auto y = y_min; y < y_max; y++) {
			for (auto z = 0; z < z_max; z++) {
				__uint128_t prev = H(1,z,y);
				for (auto w = 0; w <= z; w++) {
					auto r = H(w,z,y);
					I(w,z,y) = sadd(r, sadd(prev, H(w+1,z,y)));
					prev = r;
				}
			}
		}
		w_max++;

		// transition rule
		for (auto w = 0; w < w_max; w++) {
			for (auto z = w; z < z_max; z++) {
				for (int y = y_min; y < y_max; y++) {
					auto r = (I(w,z,y) - G(w,z,y)) | G(w,z,y);
					r ^= Mc, r &= r >> 2, r &= r >> 1, r &= M1;
					G(w,z,y) = r;
				}
			}
		}
	}

	int count() {
		int c = 0;
		for (int w = 0; w < w_max; w++) {
			for (int z = w; z < z_max; z++) {
				for (int y = y_min; y < y_max; y++) {
					int add = 0;
					auto r = G(w,z,y);
					add += _mm_popcnt_u64(uint64_t(r));
					add += _mm_popcnt_u64(uint64_t(r>>64));
					add <<= !!z + !!w + (w != z);
					c += add;
				}
			}
		}
		return c;
	}
};

}

output_t day17(input_t in) {
	solver1 S1;
	solver2 S2;

	__uint128_t row = 0;
	int y = 0;
	for (; in.len--; in.s++) {
		if (*in.s == '\n') {
			if (y >= 8) abort();
			S1.G(  0,S1.y_min+y) = row << 24;
			S2.G(0,0,S2.y_min+y) = row << 24;
			row = 0;
			y++;
		} else {
			row = (row << 4) | (*in.s == '#');
		}
	}

	for (int i = 0; i < 6; i++) S1.step();
	int part1 = S1.count();

	for (int i = 0; i < 6; i++) S2.step();
	int part2 = S2.count();

	return { part1, part2 };
}
