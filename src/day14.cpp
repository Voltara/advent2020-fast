#include "advent2020.h"

namespace {

struct cell {
	int64_t addr, value;
	cell() { }
	cell(int64_t addr, int64_t value) : addr(addr), value(value) { }
};

}

output_t day14(input_t in) {
	uint64_t mask0 = 0, mask1 = 0;

	std::vector<std::pair<uint16_t,int64_t>> P1;
	std::vector<cell> P2;
	P2.reserve(100000);

	while (in.len > 5) {
		if (in.s[1] == 'a') {
			parse::skip(in, 7);
			mask0 = mask1 = 0;
			for (int i = 0; i < 36; i++) {
				mask0 <<= 1, mask1 <<= 1;
				if (in.s[i] == '0') {
					mask0++;
				} else if (in.s[i] == '1') {
					mask1++;
				}
			}
			parse::skip(in, 37);
		} else if (in.s[1] == 'e') {
			parse::skip(in, 4);
			auto addr = parse::positive<int64_t>(in);
			parse::skip(in, 4);
			auto value = parse::positive<int64_t>(in);
			parse::skip(in, 1);

			P1.emplace_back(addr, (value & ~mask0) | mask1);

			uint64_t floating = 0, keep = mask0 | mask1;
			uint64_t flot = 0xfffffffff ^ keep;
			addr = (addr & mask0) | mask1;
			do {
				P2.emplace_back(addr + floating, value);
				floating = (floating + keep + 1) & flot;
			} while (floating);
		} else {
			break;
		}
	}

	int64_t part1 = 0, part2 = 0;

	std::bitset<65536> B;
	for (int i = P1.size() - 1; i >= 0; i--) {
		auto [ addr, value ] = P1[i];
		if (!B.test(addr)) {
			B.set(addr);
			part1 += value;
		}
	}

	// Stable sort
	std::vector<cell> T(P2.size());
	std::array<int, 1 << 12> hist;
	for (int s = 0; s < 36; s += 12) {
		hist.fill(0);
		for (auto &c : P2) {
			hist[(c.addr >> s) & 0xfff]++;
		}
		int idx = 0;
		for (auto &h : hist) idx += std::exchange(h, idx);
		for (auto &c : P2) {
			T[hist[(c.addr >> s) & 0xfff]++] = c;
		}
		P2.swap(T);
	}

	int64_t prev = 0;
	for (int i = P2.size() - 1; i >= 0; i--) {
		auto [ addr, value ] = P2[i];
		if (addr != prev) {
			part2 += value;
			prev = addr;
		}
	}

	return { part1, part2 };
}
