#include "advent2020.h"

using int_t = int64_t;

namespace {

template<bool P1>
struct solver {
	std::vector<int_t> S = { -1 };

	void push(int_t t) {
		if (t == -7) t = result(), pop();
		if (t >= 0 && S.back() == -5) pop(), t += pop();
		else if (P1 && t >= 0 && S.back() >= 0) t *= pop();
		S.push_back(t);
	}

	auto result() {
		auto t = pop();
		while (!P1 && S.back() >= 0) t *= pop();
		return t;
	}

	auto pop() {
		auto t = S.back();
		S.pop_back();
		return t;
	}
};

}

output_t day18(input_t in) {
	solver<true> S1;
	solver<false> S2;

	int_t part1 = 0, part2 = 0;

	while (in.len) {
		int c = *in.s;
		if (c == '\n') {
			part1 += S1.result(), part2 += S2.result();
		} else if (1 & (0x3ff0b00 >> (c &= 31))) {
			c -= 16, S1.push(c), S2.push(c);
		}
		parse::skip(in, 1);
	}

	return { part1, part2 };
}
