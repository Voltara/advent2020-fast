#include "advent2020.h"

// SWAR, 4 bits per cell

namespace {

struct row {
	std::array<__m256i, 2> R;

	row() : R() {
	}

	row(const char *s, int dimx) : R() {
		auto S = (uint64_t *) &R[0];
		for (int x = 0; x < dimx; x++) {
			if (s[x] != 'L') continue;
			S[x >> 4] |= uint64_t(1) << ((x & 15) << 2);
		}
	}

	int count() const {
		int n = 0;
		auto S = (uint64_t *) &R[0];
		for (int i = 0; i < 8; i++) {
			n += _mm_popcnt_u64(S[i]);
		}
		return n;
	}

	// Set 4-bit fields to 1 if zero, or 0 otherwise
	row is_zero() const {
		row r = ~*this;
		for (auto &m : r.R) {
			m = _mm256_and_si256(m, _mm256_srli_epi64(m, 1));
			m = _mm256_and_si256(m, _mm256_srli_epi64(m, 2));
			m = _mm256_and_si256(m, _mm256_set1_epi8(0x11));
		}
		return r;
	}

	row is_small() const {
		row r = ~*this;
		for (auto &m : r.R) {
			m = _mm256_sub_epi64(m, _mm256_set1_epi8(0x33));
			m = _mm256_srli_epi64(m, 3);
			m = _mm256_and_si256(m, _mm256_set1_epi8(0x11));
		}
		return r;
	}

	row operator+ (const row &o) const { row r = *this; return r += o; }
	row operator& (const row &o) const { row r = *this; return r &= o; }
	row operator| (const row &o) const { row r = *this; return r |= o; }
	row operator<< (int s) const { row r = *this; return r <<= s; }
	row operator>> (int s) const { row r = *this; return r >>= s; }

	row operator ~ () const {
		row r;
		auto it = r.R.begin();
		for (auto &m : R) {
			*it++ = _mm256_xor_si256(m, _mm256_set1_epi8(0xff));
		}
		return r;
	}

	void shift_right_4() {
		auto l = _mm256_srli_epi64(R[0], 4), cl = _mm256_slli_epi64(R[0], 60);
		auto h = _mm256_srli_epi64(R[1], 4), ch = _mm256_slli_epi64(R[1], 60);
		auto ml = _mm256_permute2x128_si256(ch, cl, _MM_SHUFFLE(0,0,0,3));
		auto mh = _mm256_permute2x128_si256(ch, ch, _MM_SHUFFLE(3,0,0,3));
		R[0] = _mm256_or_si256(l, _mm256_alignr_epi8(ml, cl, 8));
		R[1] = _mm256_or_si256(h, _mm256_alignr_epi8(mh, ch, 8));
	}

	template<int N> void shift_left() {
		const auto s = 16 - (N + 4) / 8;
		auto m0 = _mm256_permute2x128_si256(R[0], R[0], _MM_SHUFFLE(0,0,3,0));
		auto m1 = _mm256_permute2x128_si256(R[0], R[1], _MM_SHUFFLE(0,2,0,1));
		R[0] = _mm256_alignr_epi8(R[0], m0, s);
		R[1] = _mm256_alignr_epi8(R[1], m1, s);
		if (N & 4) shift_right_4();
	}

	template<int N> void shift_right() {
		const auto s = N / 8;
		auto m0 = _mm256_permute2x128_si256(R[1], R[0], _MM_SHUFFLE(0,0,0,3));
		auto m1 = _mm256_permute2x128_si256(R[1], R[1], _MM_SHUFFLE(3,0,0,3));
		R[0] = _mm256_alignr_epi8(m0, R[0], s);
		R[1] = _mm256_alignr_epi8(m1, R[1], s);
		if (N & 4) shift_right_4();
	}

	row& operator<<= (int sl) {
		switch (sl) {
		    case  4: shift_left< 4>(); break;
		    case  8: shift_left< 8>(); break;
		    case 12: shift_left<12>(); break;
		    case 16: shift_left<16>(); break;
		    case 20: shift_left<20>(); break;
		    case 24: shift_left<24>(); break;
		    case 28: shift_left<28>(); break;
		    case 32: shift_left<32>(); break;
		    case 36: shift_left<36>(); break;
		    case 40: shift_left<40>(); break;
		    case 44: shift_left<44>(); break;
		    case 48: shift_left<48>(); break;
		    case 52: shift_left<52>(); break;
		    case 56: shift_left<56>(); break;
		    case 60: shift_left<60>(); break;
		    case 64: shift_left<64>(); break;
		    case 68: shift_left<68>(); break;
		    case 72: shift_left<72>(); break;
		    default: abort();
		}
		return *this;
	}

	row& operator>>= (int sr) {
		switch (sr) {
		    case  4: shift_right< 4>(); break;
		    case  8: shift_right< 8>(); break;
		    case 12: shift_right<12>(); break;
		    case 16: shift_right<16>(); break;
		    case 20: shift_right<20>(); break;
		    case 24: shift_right<24>(); break;
		    case 28: shift_right<28>(); break;
		    case 32: shift_right<32>(); break;
		    case 36: shift_right<36>(); break;
		    case 40: shift_right<40>(); break;
		    case 44: shift_right<44>(); break;
		    case 48: shift_right<48>(); break;
		    case 52: shift_right<52>(); break;
		    case 56: shift_right<56>(); break;
		    case 60: shift_right<60>(); break;
		    case 64: shift_right<64>(); break;
		    case 68: shift_right<68>(); break;
		    case 72: shift_right<72>(); break;
		    default: abort();
		}
		return *this;
	}

	row& operator += (const row &o) {
		R[0] = _mm256_add_epi64(R[0], o.R[0]);
		R[1] = _mm256_add_epi64(R[1], o.R[1]);
		return *this;
	}

	row& operator &= (const row &o) {
		R[0] = _mm256_and_si256(R[0], o.R[0]);
		R[1] = _mm256_and_si256(R[1], o.R[1]);
		return *this;
	}

	row& operator |= (const row &o) {
		R[0] = _mm256_or_si256(R[0], o.R[0]);
		R[1] = _mm256_or_si256(R[1], o.R[1]);
		return *this;
	}

	bool operator != (const row &o) const {
		return ~_mm256_movemask_epi8(_mm256_cmpeq_epi64(R[0], o.R[0])) ||
			~_mm256_movemask_epi8(_mm256_cmpeq_epi64(R[1], o.R[1]));
	}

	bool operator ! () const {
		uint32_t nz = 0;
		nz |= _mm256_movemask_epi8(_mm256_cmpgt_epi64(R[0], _mm256_setzero_si256()));
		nz |= _mm256_movemask_epi8(_mm256_cmpgt_epi64(R[1], _mm256_setzero_si256()));
		return !nz;
	}
};

}

output_t day11(input_t in) {
	char *nl = strchr(in.s, '\n');
	if (!nl || nl == in.s) abort();

	int dimx = nl - in.s;
	if (dimx > 124) abort();

	std::vector<row> G;
	for (int y = 0; in.len >= dimx + 1; y++) {
		if (in.s[dimx] != '\n') abort();
		G.emplace_back(in.s, dimx);
		parse::skip(in, dimx + 1);
	}

	// Neighbor counts
	std::vector<row> N(G.size());

	// Seat locations & inverses
	std::vector<row> Seat = G, SeatI;
	for (auto &r : G) {
		SeatI.push_back(~r);
	}

	// Find horizontal shifts for each row, precompute masks
	std::vector<int> Shift;
	std::vector<row> ShiftMaskL, ShiftMaskR;
	for (int i = 0; i < G.size(); i++) {
		auto gl = G[i], sl = gl, gr = gl, sr = gl;

		int shift = 0;
		while (!!sl) {
			shift += 4;
			sl >>= 4, sr <<= 4;

			auto ml = gl & sl;
			if (!ml) continue;
			auto mr = gr & sr;

			sl &= ~ml, gl &= ~ml;
			sr &= ~mr, gr &= ~mr;

			Shift.push_back(shift);
			ShiftMaskL.push_back(ml);
			ShiftMaskR.push_back(mr);

			shift = 0;
		}
		Shift.push_back(0);
		ShiftMaskL.emplace_back();
		ShiftMaskR.emplace_back();
	}

	// Update grid based on neighbor counts
	auto update = [&]() {
		bool diff = false;
		for (int i = 0; i < G.size(); i++) {
			auto t = (N[i].is_small() & G[i]) |
				(N[i].is_zero() & Seat[i]);
			diff |= (t != G[i]);
			G[i] = t;
		}
		return diff;
	};

	// Count all occupied seats
	auto count = [&]() {
		int n = 0;
		for (auto &r : G) n += r.count();
		return n;
	};

	// Part 1
	for (bool diff = true; diff; diff = update()) {
		N[0] = G[0] + (G[0] << 4) + (G[0] >> 4);
		row t = N[0];
		for (int i = 1; i < G.size(); i++) {
			N[i] = t;
			t = G[i] + (G[i] << 4) + (G[i] >> 4);
			N[i] += t, N[i-1] += t;
		}
	}
	int part1 = count();

	// Part 2
	G = Seat;
	for (bool diff = true; diff; diff = update()) {
		std::fill(N.begin(), N.end(), row{});

		// Propagate distant neighbors vertically/diagonally
		auto vertical = [&](int start, int end, int incr) {
			row UL, U, UR;
			for (int i = start; i != end; i += incr) {
				UL = UL >>= 4, UR <<= 4;
				N[i] += UL + U + UR;
				UL = G[i] | (UL & SeatI[i]);
				U  = G[i] | (U  & SeatI[i]);
				UR = G[i] | (UR & SeatI[i]);
			}
		};

		vertical(0, G.size(), 1);       // Top-down
		vertical(G.size() - 1, -1, -1); // Bottom-up

		// Propagate horizontally
		for (int i = 0, sidx = 0; i < G.size(); i++, sidx++) {
			row L = G[i], R = G[i];
			for (auto s = Shift[sidx]; s; s = Shift[++sidx]) {
				L >>= s, R <<= s;
				N[i] += (L & ShiftMaskL[sidx]);
				N[i] += (R & ShiftMaskR[sidx]);
			}
		}
	}
	int part2 = count();

	return { part1, part2 };
}
