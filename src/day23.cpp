#include "advent2020.h"

template<int PART>
static int64_t solve(const std::vector<int> &V, int *N, int len, int steps) {
	for (int i = 1; i < len; i++) N[i - 1] = i;
	for (int i = 1; i < V.size(); i++) N[V[i - 1]] = V[i];

	if (V.size() == len) {
		N[V.back()] = V[0];
	} else {
		N[V.back()] = V.size();
		N[len - 1] = V[0];
	}

	for (int s = 1, i = V[0]; s <= steps; s++) {
		auto j = i, a = N[i], b = N[a], c = N[b];
		do {
			j = (j ? j : len) - 1;
		} while (j == a || j == b || j == c);
		i = N[i] = std::exchange(N[c], std::exchange(N[j], a));
	}

	if (PART == 1) {
		int answer = 0, i = 0;
		while (--len) answer = 10 * answer + (i = N[i]) + 1;
		return answer;
	} else {
		int64_t a = N[0] + 1, b = N[N[0]] + 1;
		return a * b;
	}
}

output_t day23(input_t in) {
	std::vector<int> V;

	for (; in.len; parse::skip(in, 1)) {
		uint8_t d = *in.s - '1';
		if (d > '9') break;
		V.push_back(d);
	}

	constexpr int SIZE2 = 1000000;

	// Try to get 2MB pages if possible
	hugemem alloc(SIZE2 * sizeof(int));
	auto N = (int *) alloc.ptr;

	auto part1 = solve<1>(V, N, V.size(), 100);
	auto part2 = solve<2>(V, N, SIZE2, 10000000);

	return { part1, part2 };
}
