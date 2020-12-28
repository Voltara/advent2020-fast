#include "advent2020.h"

namespace {

struct range {
	int start, end;
	range() : start(), end() { }
	range(input_t &in) {
		start = parse::positive<int>(in) & 1023;
		end  = (parse::positive<int>(in) + 1) & 1023;
	}
};

struct range_pair {
	std::array<range, 2> R;
	range_pair(input_t &in) {
		R[0] = in;
		R[1] = in;
	}
};

static auto parse_list(input_t &in) {
	std::vector<int> V;
	do {
		V.push_back(parse::positive<int>(in) & 1023);
	} while (*in.s == ',');
	return V;
}

}

output_t day16(input_t in) {
	std::vector<range_pair> R;
	while (*in.s >= 'a') {
		R.emplace_back(in);
		parse::skip(in, 1);
	}

	int n_fields = R.size();

	// Construct mask of valid fields for each value
	std::vector<uint32_t> valid(1024);
	for (int i = 0; i < n_fields; i++) {
		uint32_t m = 1 << i;
		for (auto &r : R[i].R) {
			valid[r.start] += m;
			valid[r.end] -= m;
		}
	}
	std::partial_sum(valid.begin(), valid.end(), valid.begin());

	// Your ticket
	auto Your = parse_list(in);

	std::vector<uint64_t> M(n_fields, -1);

	int part1 = 0;

	// Nearby tickets
	while (in.len > 40) {
		auto N = parse_list(in);
		N.resize(n_fields);

		// Check ticket validity
		bool bad = false;
		for (auto k : N) {
			bad |= !valid[k];
			part1 += k & -!valid[k];
		}

		if (bad) continue;

		// Update field validity masks
		auto it = M.begin();
		for (auto k : N) *it++ &= valid[k];
	}

	// Tag each field mask with position and population count
	for (int i = 0; i < n_fields; i++) {
		M[i] = (M[i] << 16) | (i << 8) | _mm_popcnt_u64(M[i]);
	}

	// Solve position-to-field mapping
	std::vector<int> F(n_fields);
	for (int f = 0; f < n_fields; f++) {
		// Find a singleton
		int s;
		for (s = 0; s < M.size() && uint8_t(M[s]) != 1; s++) { }
		if (s == M.size()) abort();

		// Record the field number, then remove from the list
		auto m = M[s];
		F[_mm_tzcnt_64(m >> 16)] = uint8_t(m >> 8);
		M[s] = M.back();
		M.pop_back();

		// Remove the field from the remaining masks
		m = ~m | 0xffff;
		for (auto &k : M) {
			auto old = std::exchange(k, k & m);
			k -= (k != old); // decrement popcount
		}
	}

	// Departure fields are always the first six
	int64_t part2 = 1;
	for (int i = 0; i < 6; i++) {
		part2 *= Your[F[i]];
	}

	return { part1, part2 };
}
