#include "advent2020.h"

namespace {

// Bitwise transformations
struct xf {
	// Reverse 10-bit number
	static uint16_t rev(uint16_t b) {
		b = (b & 0x00ff) << 8 | (b & 0xff00) >> 8;
		b = (b & 0x0f0f) << 4 | (b & 0xf0f0) >> 4;
		b = (b & 0x3333) << 2 | (b & 0xcccc) >> 2;
		b = (b & 0x5555) << 1 | (b & 0xaaaa) >> 1;
		return b >> 6;
	}

	// Reflect across diagonal
	static uint64_t diag(uint64_t b) {
		b = ss<28, 0x00000000f0f0f0f0>(b);
		b = ss<14, 0x0000cccc0000cccc>(b);
		b = ss< 7, 0x00aa00aa00aa00aa>(b);
		return b;
	}

	// Reflect across antidiagonal
	static uint64_t anti(uint64_t b) {
		b = ss<36, 0x000000000f0f0f0f>(b);
		b = ss<18, 0x0000333300003333>(b);
		b = ss< 9, 0x0055005500550055>(b);
		return b;
	}

	// Reflect east-west
	static uint64_t ew(uint64_t b) {
		b = ss<4, 0x0f0f0f0f0f0f0f0f>(b);
		b = ss<2, 0x3333333333333333>(b);
		b = ss<1, 0x5555555555555555>(b);
		return b;
	}

	// Reflect north-south
	static uint64_t ns(uint64_t b) {
		b = ss<32, 0x00000000ffffffff>(b);
		b = ss<16, 0x0000ffff0000ffff>(b);
		b = ss< 8, 0x00ff00ff00ff00ff>(b);
		return b;
	}

    private:
	// shift + swap masked bits
	template<int S, uint64_t M>
	static uint64_t ss(uint64_t b) {
		auto x = b & (M<<S), y = b & M;
		b ^= (~M ^ M<<S) ? x | y : b;
		return b | x>>S | y<<S;
	}
};

struct tile {
	int id;
	// Four edges with bits in north-south / west-east order
	std::array<uint16_t, 4> edge;
	// 8x8 tile core
	uint64_t core;

	tile() : id(), edge(), core() { }

	// Parse the tile
	tile(input_t &in) : edge(), core() {
		parse::skip(in, 5); // "Tile "
		id = parse::positive<int>(in);
		parse::skip(in, 2); // ":\n"

		int m = 1; // current e-w bit

		auto row = [&]() {
			auto& [ n, e, s, w ] = edge;
			w |= m & -(*in.s & 1); parse::skip(in, 1);
			auto c = parse::octet(in, 7);
			e |= m & -(*in.s & 1); parse::skip(in, 2);
			m <<= 1;
			return c;
		};

		auto& [ n, e, s, w ] = edge;

		n = row() << 1;
		for (int i = 0; i < 64; i += 8) {
			core |= uint64_t(row()) << i;
		}
		s = row() << 1;

		// Copy corners from e-w to n-s
		n |= (w & 1) | ((e << 9) & 0x200);
		s |= (w >> 9) | (e & 0x200);
	}

	// Get oriented eastern edge
	uint16_t east(int ori) {
		ori = 0x07432561 >> (ori * 4);
		auto e = edge[ori & 3];
		return (ori & 4) ? xf::rev(e) : e;
	}

	// Get oriented southern edge
	uint16_t south(int ori) {
		auto e = edge[(ori ^ 2) & 3];
		return (ori & 4) ? xf::rev(e) : e;
	}

	// Get oriented core
	uint64_t get_core(int ori) {
		auto c = core;
		int m = 0x9c281450U >> (ori * 4);
		if (m & 1) c = xf::diag(c);
		if (m & 2) c = xf::anti(c);
		if (m & 4) c = xf::ns(c);
		if (m & 8) c = xf::ew(c);
		return c;
	}
};

// Tile index & orientation
struct tile_ori {
	int idx, ori;
	tile_ori() : idx(), ori() { }
	tile_ori(int idx, int ori) : idx(idx), ori(ori) { }
};

// Mapping of edges to tiles
struct tile_index {
	std::array<std::array<tile_ori, 2>, 1024> I = { };
	// For identifying unpaired edges
	std::bitset<1024> Parity;

	// Add a tile to the index
	void add(tile &t, int idx) {
		auto set = [&](int e, int ori) {
			I[e][1] = std::exchange(I[e][0], { idx, ori });
			Parity.flip(e);
		};
		int ori = 0;
		for (auto e : t.edge) {
			set(e, ori);
			set(xf::rev(e), 4|ori++);
		}
	}

	// Check if edge is part of the border
	bool parity(uint16_t e) const {
		return Parity.test(e);
	}

	// Find tile and orientation by north edge (exclude original tile)
	tile_ori match_north(int idx, uint16_t e) const {
		return I[e][I[e][0].idx == idx];
	}

	// Find tile and orientation by west edge (exclude original tile)
	tile_ori match_west(int idx, uint16_t e) const {
		auto r = I[e][I[e][0].idx == idx];
		r.ori = (0x25610743 >> (r.ori * 4)) & 7;
		return r;
	}
};

struct grid {
	std::array<std::array<uint64_t,12>,12> G;

	// Count the number of 1-bits
	int popcount() const {
		int count = 0;
		for (auto &row : G) {
			for (auto k : row) {
				count += __builtin_popcountll(k);
			}
		}
		return count;
	}

	// Reflect across the diagonal
	void diag_inplace() {
		for (int y = 0; y < 12; y++) {
			G[y][y] = xf::diag(G[y][y]);
			for (int x = y + 1; x < 12; x++) {
				G[y][x] = xf::diag(G[y][x]);
				std::swap(G[y][x], G[x][y]);
				G[y][x] = xf::diag(G[y][x]);
			}
		}
	}

	// Reflect east-west
	void ew_inplace() {
		for (auto &r : G) {
			for (int x = 0; x < 12; x += 2) {
				r[x] = xf::ew(r[x]);
				std::swap(r[x], r[11 - x]);
				r[x] = xf::ew(r[x]);
			}
		}
	}

	// Return the number of squares occupied by sea monsters,
	// assuming no overlap
	int find_monsters() const {
		auto u = west(), a = u & north(1), b = u & south(1);
		b = b & a.north(3);
		b = b & b.north(6);
		b = b & b.north(6);
		b = *this & b.north(2);
		b = b.west() & a.south(1);
		return b.popcount() * 15;
	};

    private:
	// Shift west by 1 unit
	grid west() const {
		grid r;
		auto &H = r.G;
		constexpr uint64_t MASK = 0x0101010101010101;
		for (int y = 0; y < 12; y++) {
			for (int x = 0; x < 12; x++) {
				H[y][x] = (G[y][x] & ~MASK) >> 1;
				if (x) H[y][x-1] |= (G[y][x] & MASK) << 7;
			}
		}
		return r;
	}

	// Shift north by n units (max 7)
	grid north(int n) const {
		grid r;
		auto &H = r.G;
		int sr = 8 * n, sl = 64 - sr;
		for (int y = 0; y < 12; y++) {
			for (int x = 0; x < 12; x++) {
				H[y][x] = G[y][x] >> sr;
				if (y) H[y-1][x] |= G[y][x] << sl;
			}
		}
		return r;
	}

	// Shift south by n units (max 7)
	grid south(int n) const {
		grid r;
		auto &H = r.G;
		int sr = 8 * n, sl = 64 - sr;
		for (int y = 0; y < 12; y++) {
			for (int x = 0; x < 12; x++) {
				H[y][x] = G[y][x] << sr;
				if (y) H[y][x] |= G[y-1][x] >> sl;
			}
		}
		return r;
	}

	// Bitwise and
	grid operator& (const grid &o) const {
		grid r;
		auto &H = r.G;
		for (int y = 0; y < 12; y++) {
			for (int x = 0; x < 12; x++) {
				H[y][x] = G[y][x] & o.G[y][x];
			}
		}
		return r;
	}

};

}

output_t day20(input_t in) {
	tile_index Tidx;

	std::vector<tile> T;
	T.reserve(144);

	while (in.len > 115) {
		T.emplace_back(in);
		Tidx.add(T.back(), T.size() - 1);
	}
	if (T.size() != 144) abort();

	int64_t part1 = 1;

	tile_ori corner;

	// Solve Part 1 and also find any corner
	for (int idx = 0; idx < T.size(); idx++) {
		auto &t = T[idx];
		int c = 0;
		for (auto e : t.edge) {
			c = (c << 1) | Tidx.parity(e);
		}
		if ((011110 >> c) & 1) {
			part1 *= t.id;
			corner = { idx, c };
		}
	}

	// Orient the corner's borders northwest
	corner.ori = (010520 >> corner.ori) & 7;

	// Assemble the grid
	grid G;
	for (int y = 0; y < 12; y++) {
		auto t = corner;
		corner = Tidx.match_north(t.idx, T[t.idx].south(t.ori));
		G.G[y][0] = T[t.idx].get_core(t.ori);
		for (int x = 1; x < 12; x++) {
			t = Tidx.match_west(t.idx, T[t.idx].east(t.ori));
			G.G[y][x] = T[t.idx].get_core(t.ori);
		}
	}

	// Look for sea monsters in various orientations
	int part2 = 0;
	for (int i = 0; i < 4; i++) {
		if ((part2 = G.find_monsters())) break;
		G.ew_inplace();
		if ((part2 = G.find_monsters())) break;
		G.diag_inplace();
	}

	// Count the non-monster squares
	part2 = G.popcount() - part2;

	return { part1, part2 };
}
