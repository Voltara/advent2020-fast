#include "advent2020.h"

static constexpr int encode2(const char *s) {
	return (s[0] << 8) + s[1];
}

static constexpr int encode3(const char *s) {
	return (encode2(s) << 8) + s[2];
}

// Minimal perfect hash of field names
enum { PID, EYR, CID, IYR, HGT, BYR, HCL, ECL };
static int hash_field(const char *s) {
	return encode3(s) % 477 % 8;
}

// Check eye color
static bool check_ecl(input_t &in) {
	// Minimal perfect hash table of eye colors
	static constexpr int valid[7] = {
		encode3("grn"), encode3("hzl"), encode3("amb"), encode3("blu"),
		encode3("gry"), encode3("brn"), encode3("oth") };
	if (in.len < 3) return false;
	int ecl = encode3(in.s);
	in.s += 3, in.len -= 3;
	return (ecl == valid[ecl % 122 % 7]);
}

// Hex string: #xxxxxx
static bool check_hex(input_t &in, int count) {
	if (in.len < count + 1) return false;
	if (*in.s != '#') return false;
	for (in.s++, in.len--; count--; in.s++, in.len--) {
		if (*in.s < '0' || *in.s > 'f') return false;
		if (*in.s > '9' && *in.s < 'a') return false;
	}
	return true;
}

// Fixed-width positive integer in range
static bool check_range(input_t &in, int digits, int min, int max) {
	int n = parse::positive<int>(in, digits, true);
	return n >= min && n <= max;
}

static bool validate(input_t &in, int h) {
	int n;
	switch (h) {
	    case BYR: return check_range(in, 4, 1920, 2002);
	    case IYR: return check_range(in, 4, 2010, 2020);
	    case EYR: return check_range(in, 4, 2020, 2030);
	    case PID: return check_range(in, 9, 1, INT32_MAX);
	    case ECL: return check_ecl(in);
	    case HCL: return check_hex(in, 6);
	    case HGT:
		n = parse::positive<int>(in);
		if (in.len < 2) return false;
		in.s += 2, in.len -= 2;
		switch (encode2(in.s - 2)) {
		    case encode2("cm"):
			return (n >= 150) && (n <= 193);
		    case encode2("in"):
			return (n >= 59) && (n <= 76);
		}
		return false;
	}
	return true;
}

output_t day04(input_t in) {
	if (in.len < 3) abort();

	int part1 = 0, part2 = 0, have = 0, bad = 0;

	auto check = [&]() {
		int complete = (have | (1 << CID)) == 0xff;
		part1 += complete;
		part2 += complete & !bad;
	};

	in.s += 3, in.len -= 3;
	for (int nl = 0; in.len; ) {
		char c = *in.s++;
		in.len--;
		if (c == '\n') {
			nl ^= 1;
			if (!nl) {
				check();
				have = bad = 0;
			}
		} else if (c == ':') {
			nl = 0;
			int h = hash_field(in.s - 4);
			have |= 1 << h;
			bad = bad || !validate(in, h);
		}
	}

	// Final passport
	check();

	return { part1, part2 };
}
