#include "advent2020.h"

namespace {

struct instr {
	int next, alt, acc;
	bool mark;
	instr(int next, int alt, int acc) :
		next(next), alt(alt), acc(acc), mark()
	{
	}
};

struct solver {
	std::vector<instr> V;
	std::vector<std::pair<int,int>> Alt;

	int part1() {
		int idx = 0, acc = 0;
		while (idx >= 0 && idx <= V.size()) {
			auto &I = V[idx];
			if (I.mark) break;
			acc += I.acc;
			I.mark = true;
			idx = I.next;

			// Remember alternate execution path
			if (I.alt != I.next) {
				Alt.emplace_back(I.alt, acc);
			}
		}

		return acc;
	}

	int part2() {
		for (auto [ idx, acc ] : Alt) {
			acc = part2_tail(idx, acc);
			if (acc) return acc;
		}
		return 0;
	}

	int part2_tail(int idx, int acc) {
		while (idx >= 0 && idx < V.size()) {
			auto &I = V[idx];
			if (I.mark) return 0;
			I.mark = true;
			acc += I.acc;
			idx = I.next;
		}
		return acc;
	}
};

}

output_t day08(input_t in) {
	solver S;

	while (in.len >= 6) {
		char op = in.s[0];
		bool neg = (in.s[4] == '-');
		parse::skip(in, 5);
		int n = parse::positive<int>(in);
		parse::skip(in, 1);
		if (neg) n = -n;

		int next = S.V.size() + 1, alt = next, acc = 0;

		if (op == 'a') {
			acc = n;
		} else {
			alt = next + n - 1;
			if (op == 'j') std::swap(next, alt);
		}

		S.V.emplace_back(next, alt, acc);
	}

	int part1 = S.part1(), part2 = S.part2();

	return { part1, part2 };
}
