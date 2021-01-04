#include "advent2020.h"

namespace {

struct bitset2048 {
	using T = uint64_t;

	std::array<T, 32> B = { };

	void set(int n) {
		B[n >> 6] |= T(1) << (n & 63);
	}

	bool test(int n) const {
		return (B[n >> 6] >> (n & 63)) & 1;
	}

	// Return sorted list of members
	auto vec(int reserve) const {
		std::vector<int> V;
		V.reserve(reserve);
		int h = 0;
		for (auto mask : B) {
			for (auto b : bits(mask)) {
				V.push_back(h | b);
			}
			h += 64;
		}
		return V;
	}
};

}

output_t day01(input_t in) {
	int part1 = 0, part2 = 0;

	bitset2048 B;

	int n, count = 0;
	while ((n = parse::positive<int>(in))) {
		if (n >= 2020) continue;
		B.set(n);
		count++;
	}

	// Get the list of numbers in sorted order
	auto V = B.vec(count);

	// Part 1: Meet in the middle
	for (auto m : V) {
		auto n = 2020 - m;
		if (B.test(n)) {
			part1 = m * n;
			break;
		}
	}

	/* Part 2: Brute force first two, lookup last
	 *
	 * This is MUCH faster than the apparent O(n^2) complexity.
	 * Because the input must have only one solution, the
	 * distribution is biased heavily toward larger numbers.
	 */
	for (auto m = V.begin(); m != V.end(); m++) {
		for (auto n = std::next(m); n != V.end(); n++) {
			int o = 2020 - *m - *n;
			// To avoid duplicates: m < n < o
			if (o < *n) break;
			if (B.test(o)) {
				part2 = *m * *n * o;
				goto done;
			}
		}
	}

done:
	return { part1, part2 };
}
