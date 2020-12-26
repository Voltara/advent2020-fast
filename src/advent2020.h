#ifndef _ADVENT2020_H
#define _ADVENT2020_H

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <array>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <bitset>
#include <sstream>
#include <x86intrin.h>

struct input_t {
	char *s;
	ssize_t len;
};

struct output_t {
	std::array<std::string, 2> answer;

	template<typename T1, typename T2>
	output_t(T1 p1, T2 p2) {
		set(0, p1);
		set(1, p2);
	}

	template<typename T>
	void set(int part, T value) {
		std::stringstream ss;
		ss << value;
		answer[part] = ss.str();
	}
};

struct advent_t {
	output_t (*fn)(input_t);
};

output_t day01(input_t);
output_t day02(input_t);
output_t day03(input_t);
output_t day04(input_t);
output_t day05(input_t);
output_t day06(input_t);
output_t day07(input_t);
output_t day08(input_t);
output_t day09(input_t);
output_t day10(input_t);
output_t day11(input_t);
output_t day12(input_t);
output_t day13(input_t);
output_t day14(input_t);
output_t day15(input_t);
output_t day16(input_t);
output_t day17(input_t);
output_t day18(input_t);
output_t day19(input_t);
output_t day20(input_t);
output_t day21(input_t);
output_t day22(input_t);
output_t day23(input_t);
output_t day24(input_t);
output_t day25(input_t);

#include "parse.h"
#include "bits.h"
#include "numeric.h"
#include "memory.h"

#endif
