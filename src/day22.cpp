#include "advent2020.h"

namespace {

// Random BUZ hash values, one for each (card, player) pair
static constexpr std::array<uint64_t, 100> Htable = {
0x2f6c30a572abb66a, 0x43181d2f6a7c6624, 0x394be292aa3ba2dc, 0x284cea25f597e52b,
0x14975a540dd4ff54, 0xe1aae11b42bc023a, 0xa6f4e6112a314500, 0x863e536e41c59c77,
0x4eaaa4a1a08da257, 0x7ae4b03953e3aff1, 0x237e795874c379bb, 0x849c2cf0efeb90ee,
0x36b06f4d45f84b53, 0x9489f8cce8b7ab81, 0xad1517dddd3d4584, 0xca933dd11c75ba65,
0x91da8cfe3faf2e0b, 0xb40d2f0f0accd202, 0x1b0d214e95c0dee5, 0x28bb75e43c271fed,
0xede31e378788cda8, 0xe698672d5e980f50, 0xeff9d41611cbb019, 0xa95ee9680f8fd6fb,
0x325b5edba311d0b5, 0x5ea0bcd55aab35b1, 0x675a1b19394f61a7, 0x5b2691819aee7c0e,
0x97cb5f117c3a3de6, 0xdb02bb8f04f413c9, 0xab3abaa6174ff768, 0x754e53e3c67a383b,
0xaeaa5c948b61f64c, 0xd81e7d25800ad3dc, 0x2a3e68f23e3c6858, 0x116c9f80f80d617a,
0xfb8601b39f1704c5, 0xf1c4e560c3a0a390, 0x8a37115a7f5f9ce1, 0xf16e2c62599d3810,
0xe63076b39751b857, 0xcfcf045428511e80, 0x21734139f1e43a6a, 0x30ee1abae2c4104b,
0xc3d13ac1233d2ba0, 0x63ded365ac705859, 0x462cc3e6ed7042bf, 0xc28450f7aba99c58,
0x1b31c4abb857259f, 0x8e19b11a548039bf, 0xf6839e65e9624a40, 0x6ed4e0c67a9cb686,
0x248368e255fc9f7e, 0xc5d28281339fd127, 0x5960fa04a0bae74e, 0x6728c82e704efc76,
0x485813ed286c1d70, 0x4e251815754ae5cd, 0xbdcfe14a7191864f, 0x11bc9e97bad6e7b0,
0x88772d6a11cfc4d6, 0x39e59294cd4cd1e4, 0x89549167158669af, 0x9ad1f18e00801d55,
0x663ad4f580b200a7, 0x4fcf4d42c3b96298, 0x91e755ff94d7dd1e, 0x1ef30d6e1c0be997,
0xcdb73a96ba06aa9b, 0xd555d85b16635d15, 0x54ea66890fa67731, 0xb9fc868f83a8fa1f,
0x7c8d49a5662764eb, 0x498be6569d9a09cb, 0x9533eed845458bfb, 0x8d41b72f2a265993,
0xfb07c8d2f3f01aec, 0xece949eae4b7aa5e, 0x80002f3965bc6e26, 0x6801407a4e192743,
0x7b29579ca72991ac, 0xd873bfa5fd567f15, 0x38420bd89e310693, 0x7037bf9f1647c7df,
0x7c978a5946b80880, 0x3665b20dff570e65, 0x74d72b0349c0c9ad, 0xf2c1866ae5ee8182,
0x5c608eef4a7771d1, 0x063ba7085f1fd12e, 0x6754e71dc1a13c3a, 0x13dd7510e260fe70,
0x34e06464cb58c2fb, 0xc7aa4debec66e22b, 0xa56a927afa78e6a4, 0x9bc1b2b8b55a4716,
0x0e352372fc1b176c, 0xd5bd86b8b002496c, 0x9215ed588250b5b8, 0x91bccdb69fe38880,
};

struct solver {
	std::array<std::array<uint8_t, 64>, 2> V; // circular array
	std::array<int, 2> n{}; // hand lengths
	int base{}; // current base index in array
	uint8_t hi;

	// bitwise rotate left/right
	static uint64_t rol(uint64_t n, int r) { return _lrotl(n, r); }
	static uint64_t ror(uint64_t n, int r) { return _lrotr(n, r); }

	// Hash function
	static uint64_t H(int player, int card) {
		return Htable[(--card << 1) | player];
	}

	// Get player's card at offset
	auto & D(int player, int offset) {
		return V[player][(base + offset) & 63];
	}

	// Used for initializing player hands
	void push(int player, int card) {
		D(player, n[player]++) = card;
	}

	solver() : hi() {
		// high card doesn't matter for root level game
	}

	solver(const solver &s, int n0, int n1, uint8_t hi)
		: V(s.V), n({n0,n1}), base(s.base), hi(hi)
	{
	}

	// Compute score
	int score() {
		int s = 0, player = !n[0];
		for (int i = 0, w = 50; i < n[player]; i++) {
			s += D(player, i) * w--;
		}
		return s;
	}

	// Part 1
	int solve1() {
		while (n[0] && n[1]) {
			int c0 = D(0,0), c1 = D(1,0);
			base++, n[0]--, n[1]--;
			if (c0 > c1) {
				D(0,n[0]++) = c0;
				D(0,n[0]++) = c1;
			} else {
				D(1,n[1]++) = c1;
				D(1,n[1]++) = c0;
			}
		}
		return !n[0];
	}

	// Value of high card, but only if player 2 has it
	uint8_t high_card(int c0, int c1) {
		uint8_t hi = 0;
		for (int i = 0; i < c1; i++) hi = std::max(hi, D(1,i));
		for (int i = 0; i < c0; i++) if (hi < D(0,i)) return 0;
		return hi;
	}

	int solve2() {
		uint64_t hash{};
		for (int player = 0; player < 2; player++) {
			for (int i = 0; i < n[player]; i++) {
				hash ^= rol(H(player, D(player,i)), i);
			}
		}

		std::unordered_set<uint32_t> seen;

		while (n[0] && n[1]) {
			// Check for cycle whenever player 2 is about
			// to play high card (this reduces expensive
			// lookups, and lowers chance of false positive)
			if (D(1,0) == hi && !seen.emplace(hash).second) {
				return 0;
			}

			// Draw cards, update game state hash
			int c0 = D(0,0), c1 = D(1,0);
			base++, n[0]--, n[1]--;
			hash = ror(hash ^ H(0, c0) ^ H(1, c1), 1);

			uint8_t new_hi;

			int winner = 0;

			if (c0 > n[0] || c1 > n[1]) {
				winner = c0 < c1;
			} else if ((new_hi = high_card(c0, c1)) == 0) {
				// Player 1 always wins with high card
				winner = 0;
			} else {
				solver s(*this, c0, c1, new_hi);
				winner = s.solve2();
			}

			// Award cards to winner, update game state hash
			if (winner == 0) {
				hash ^= rol(H(0, c0), n[0]) ^ rol(H(0, c1), n[0] + 1);
				D(0,n[0]++) = c0;
				D(0,n[0]++) = c1;
			} else {
				hash ^= rol(H(1, c1), n[1]) ^ rol(H(1, c0), n[1] + 1);
				D(1,n[1]++) = c1;
				D(1,n[1]++) = c0;
			}
		}

		return !n[0];
	}
};

}

output_t day22(input_t in) {
	solver S1;

	parse::skip(in, 10);
	for (int n = 0, player = 0; in.len; parse::skip(in, 1)) {
		if (*in.s != '\n') n = n * 10 + *in.s - '0';
		else if (n) S1.push(player, n), n = 0;
		else player ^= 1, parse::skip(in, 10);
	}

	solver S2{S1};

	S1.solve1();
	auto part1 = S1.score();

	S2.solve2();
	auto part2 = S2.score();

	return { part1, part2 };
}
