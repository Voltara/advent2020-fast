#include "advent2020.h"

output_t day10(input_t in) {
	std::array<uint64_t, 4> B = { };

	while (in.len) {
		uint8_t n = parse::positive<uint8_t>(in);
		if (n) B[n >> 6] |= uint64_t(1) << (n & 63);
	}

	int prev = 0, jolt1 = 0, jolt3 = 1, h = 0;
	int64_t f0 = 1, f1 = 0, f2 = 0, tmp;
	for (auto mask : B) {
		for (auto l : bits(mask)) {
			int delta = h + l - prev;
			prev = h + l;
			switch (delta) {
			    case 1:
				jolt1++;
				tmp = f0 + f1 + f2;
				f2 = f1, f1 = f0, f0 = tmp;
				break;
			    case 2:
				f2 = std::exchange(f0, f0 + f1);
				f1 = 0;
				break;
			    case 3:
				jolt3++;
				f1 = f2 = 0;
				break;
			}
		}
		h += 64;
	}

	auto part1 = jolt1 * jolt3;
	auto part2 = f0;

	return { part1, part2 };
}
