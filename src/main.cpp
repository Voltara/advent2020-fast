#include <fstream>
#include <chrono>
#include "advent2020.h"

static constexpr int INPUT_PADDING = 64;

static const std::vector<advent_t> advent2020 = {
	{ day01 }, { day02 }, { day03 }, { day04 }, { day05 },
	{ day06 }, { day07 }, { day08 }, { day09 }, { day10 },
	{ day11 }, { day12 }, { day13 }, { day14 }, { day15 },
	{ day16 }, { day17 }, { day18 }, { day19 }, { day20 },
	{ day21 }, { day22 }, { day23 }, { day24 }, { day25 }
};

struct timing_t {
	std::string input;
	std::string part1;
	std::string part2;
	double t = 0;
	timing_t(const std::string &line) {
		std::stringstream ss(line);
		std::getline(ss, input, '\t');
		std::getline(ss, part1, '\t');
		std::getline(ss, part2);
	}
	bool operator < (const struct timing_t &o) const {
		return (t == o.t) ? (input < o.input) : (t < o.t);
	}
};
using timing_vec = std::vector<timing_t>;

static input_t load_input(const std::string &filename);
static void free_input(input_t &input);

int main() {
	double total_time = 0;

	printf("          Time         Part 1           Part 2\n");
	printf("========================================================\n");
	for (int day = 1; day <= advent2020.size(); day++) {
		auto &A = advent2020[day - 1];
		if (!A.fn) continue;

		char filename[64];
		sprintf(filename, "input/day%02d.txt", day);

		timing_vec V;

		auto input = load_input(filename);
		auto t0 = std::chrono::steady_clock::now();
		auto output = A.fn(input);
		auto elapsed = std::chrono::steady_clock::now() - t0;
		free_input(input);

		double t = 1e-3 * std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();
		total_time += t;

		printf("Day %02d: %7.f μs     %-16s %-16s\n",
				day,
				t,
				output.answer[0].c_str(),
				output.answer[1].c_str());
	}
	printf("========================================================\n");
	printf("Total:  %7.f μs\n", total_time);

	return 0;
}

input_t load_input(const std::string &filename) {
	FILE *fp = fopen(filename.c_str(), "r");
	if (!fp) {
		perror(filename.c_str());
		exit(EXIT_FAILURE);
	}

	if (fseek(fp, 0, SEEK_END) != 0) {
		perror("fseek");
		exit(EXIT_FAILURE);
	}

	input_t in;
	in.len = ftell(fp);
	in.s = new char[in.len + INPUT_PADDING];
	std::fill(in.s + in.len, in.s + in.len + INPUT_PADDING, 0);

	rewind(fp);
	if (fread(in.s, in.len, 1, fp) != 1) {
		perror("fread");
		exit(EXIT_FAILURE);
	}
	fclose(fp);

	return in;
}

void free_input(input_t &input) {
	delete[] input.s;
}
