#include "advent2020.h"

output_t day21(input_t in) {
	// Hash the allergen name to the range [0..8]
	auto allergen_index = [&](uint64_t w) {
		w = ((w >> 33) ^ (w >> 56)) & 60;
		return (0x57800412036 >> w) & 15;
	};

	// Test 5-bit fields for nonzero
	auto nonzero5 = [](uint64_t x) {
		for (int i : { 1, 2, 1 }) x &= x >> i;
		return x & 0x10842108421;
	};

	/* Hashtable of words mapped to:
	 * - Number of times paired with each allergen
	 * - Total number of appearances
	 */
	std::array<uint64_t, 1100> Hw = { }, Ha = { };

	auto update_word = [&](uint64_t w, uint64_t allergens) {
		int i = w % 1021;
		while (Hw[i] && Hw[i] != w) i++;
		Hw[i] = w;
		Ha[i] += allergens + (1LL << 45);
	};

	int part1 = 0;

	// Parse the input
	uint64_t word = 0, allergens = 0, a_total = 0;
	std::vector<uint64_t> V;
	for (int shift = 64; in.len; ) {
		auto c = *in.s;
		if (c == ' ') {
			V.push_back(word);
			word = 0, shift = 64;
			parse::skip(in, 1);
		} else if (c == '(') {
			parse::skip(in, 10); // "(contains "
		} else if (c == ',' || c == ')') {
			allergens |= 1LL << (allergen_index(word) * 5);
			word = 0, shift = 64;
			parse::skip(in, 2);

			if (c == ')') {
				part1 += V.size();
				while (!V.empty()) {
					update_word(V.back(), allergens);
					V.pop_back();
				}
				a_total += allergens;
				allergens = 0;
			}
		} else {
			word |= uint64_t(c) << (shift -= 7);
			parse::skip(in, 1);
		}
	}

	// Mask of valid allergens
	allergens = ~nonzero5(~a_total);

	// Find all words that completely match at least one allergen
	int n_match = 0;
	for (int i = 0; i < Hw.size(); i++) {
		if (!Ha[i]) continue;
		uint64_t match = allergens & nonzero5(~a_total ^ Ha[i]);
		if (!match) continue;
		Hw[n_match] = Hw[i];
		Ha[n_match++] = match;
		part1 -= (Ha[i] >> 45);
	}

	// Find which specific ingredient matches which allergen
	std::array<uint64_t, 9> Ingredient = { };
	while (n_match) for (int i = 0; i < n_match; i++) {
		Ha[i] &= allergens;
		if (_mm_popcnt_u64(Ha[i]) != 1) continue;
		allergens ^= Ha[i];
		Ingredient[_mm_tzcnt_64(Ha[i]) / 5] = Hw[i];
		Hw[i] = Hw[--n_match];
		Ha[i] = Ha[n_match];
	}

	// Convert the words to a list of strings
	std::string part2;
	for (auto s : Ingredient) {
		if (!s) continue;
		for (; s; s <<= 7) part2.push_back(s >> 57);
		part2.push_back(',');
	}
	if (!part2.empty()) part2.pop_back();

	return { part1, part2 };
}
