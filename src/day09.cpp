#include "advent2020.h"

output_t day09(input_t in) {
	std::vector<int64_t> V;

	V.reserve(1000);

	int64_t n;
	while ((n = parse::positive<int64_t>(in))) {
		V.push_back(n);
	}

	int64_t part1 = 0, part2 = 0;

	std::unordered_set<int64_t> H;
	for (int i = 0; i < 25; i++) {
		H.insert(V[i]);
	}
	for (int i = 25; i < V.size(); i++) {
		bool found = false;
		for (int j = i - 25; j < i; j++) {
			if (H.count(V[i] - V[j])) {
				found = true;
				break;
			}
		}
		if (!found) {
			// Increment to hide from Part 2
			part1 = V[i]++;
			break;
		}
		H.erase(V[i - 25]);
		H.insert(V[i]);
	}

	V.push_back(part1); // Memory safety

	int64_t lo = 0, hi = 0, sum = V[0];
	while (sum != part1) {
		if (sum < part1) {
			sum += V[++hi];
		} else {
			sum -= V[lo++];
		}
	}

	int64_t min = INT64_MAX, max = INT64_MIN;
	for (int i = lo; i <= hi; i++) {
		min = std::min(min, V[i]);
		max = std::max(max, V[i]);
	}
	part2 = min + max;

	return { part1, part2 };
}
