#include "advent2020.h"

namespace {

struct nbag {
	int n, bag;
	nbag(int n, int bag) : n(n), bag(bag) { }
};

struct solver {
	std::vector<std::vector<int>> Out;
	std::vector<std::vector<nbag>> In;
	std::vector<int> Memo;

	void grow() {
		Out.emplace_back();
		In.emplace_back();
	}

	void contains(int outer, int n, int inner) {
		Out[inner].emplace_back(outer);
		In[outer].emplace_back(n, inner);
	}

	int part1(int b) {
		Memo.resize(Out.size());
		Memo[b] = 0;
		return _part1(b);
	}

	int part2(int b) {
		Memo.resize(Out.size());
		Memo[b] = 0;
		return _part2(b);
	}

	int _part1(int b) {
		auto &M = Memo[b];
		if (M) return 0;
		for (auto bag : Out[b]) M += _part1(bag);
		return ++M;
	}

	int _part2(int b) {
		auto &M = Memo[b];
		if (!M) {
			M = 1;
			for (auto [ n, bag ] : In[b]) {
				M += _part2(bag) * n;
			}
		}
		return M;
	}
};

}

output_t day07(input_t in) {
	std::unordered_map<std::string, int> BagIdx;
	solver S;

	auto parse_word = [&in](std::string &s) {
		while (*in.s >= 'a' && *in.s <= 'z') {
			s.push_back(*in.s);
			in.s++, in.len--;
		}
	};

	auto parse_bag = [&]() {
		std::string s;
		parse_word(s);
		s.push_back(' ');
		parse::skip(in, 1);
		parse_word(s);
		auto [ it, ok ] = BagIdx.emplace(s, BagIdx.size());
		if (ok) S.grow();
		return it->second;
	};

	while (in.len) {
		auto outer = parse_bag();
		parse::skip(in, 13);
		if (in.s[1] == 'n') {
			parse::skip(in, 16);
			continue;
		}
		while (*in.s == ' ') {
			parse::skip(in, 1);
			int n = parse::positive<int>(in);
			parse::skip(in, 1);
			int inner = parse_bag();
			parse::skip(in, 5 + (n > 1));
			S.contains(outer, n, inner);
		}
		parse::skip(in, 1);
	}

	std::vector<int> Memo(BagIdx.size());
	auto gold = BagIdx["shiny gold"];
	int part1 = S.part1(gold) - 1;
	int part2 = S.part2(gold) - 1;

	return { part1, part2 };
}
