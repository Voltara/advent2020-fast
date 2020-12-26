#include "advent2020.h"

namespace {

struct node {
	std::array<uint8_t, 4> V = { };
};

struct word {
	uint8_t mask;
	int8_t len;

	word(uint8_t mask, int8_t len) : mask(mask), len(len) { }

	word operator+ (const word &o) const {
		return word((o.mask << len) | mask, len + o.len);
	}
};

struct solver {
	std::array<node, 256> N = { };
	std::array<std::vector<word>, 256> W = { };

	solver() {
		W[0].emplace_back(0, 0);
	}

	void set(int idx, int offset, int c) {
		N[idx].V[offset] = c;
	}

	void build_dict(int idx) {
		if (!W[idx].empty()) return;

		auto &n = N[idx];
		if (!n.V[0]) {
			W[idx].emplace_back(n.V[1], 1);
		} else {
			for (int i = 0; i < 4 && n.V[i]; i += 2) {
				build_dict(n.V[i]);
				build_dict(n.V[i+1]);
				for (auto a : W[n.V[i]]) {
					for (auto b : W[n.V[i+1]]) {
						W[idx].emplace_back(a + b);
					}
				}
			}
		}
	}

	std::pair<int,int> solve(input_t &in) {
		// Rule 31 is the complement of Rule 42
		std::bitset<256> w42;
		build_dict(42);
		for (auto w : W[42]) w42.set(w.mask);

		int part1 = 0, part2 = 0;

		while (in.len >= 8) {
			int n_42 = 0, n_31 = 0, state = 0;

			while (*in.s >= 'a') {
				int t = w42.test(parse::octet(in, 7));
				n_42 += t, n_31 += !t;
				state += (!t & !state) | (t & state);
			}
			parse::skip(in, 1);

			int ok = (state == 1) & (n_31 < n_42);
			part1 += ok & (n_42 == 2);
			part2 += ok;
		}

		return { part1, part2 };
	}
};

}

output_t day19(input_t in) {
	solver S;

	while (*in.s >= ' ') {
		int idx = parse::positive<int>(in) & 255;

		parse::skip(in, 2);
		if (*in.s == '"') {
			S.set(idx, 1, in.s[1] & 1);
			parse::skip(in, 4);
		} else {
			for (int i = 0; *in.s >= ' '; i ^= 1) {
				i ^= (i | 2) & -(in.s[1] == '|');
				S.set(idx, i, parse::positive<int>(in));
			}
			parse::skip(in, 1);
		}
	}
	parse::skip(in, 1);

	auto [ part1, part2 ] = S.solve(in);

	return { part1, part2 };
}
