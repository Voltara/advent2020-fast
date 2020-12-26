#include "advent2020.h"

namespace {

struct pt {
	int x, y;
	pt() : x(), y() { }
	pt(int x, int y) : x(x), y(y) { }
	pt operator* (int n) const { return { n*x, n*y }; }
	pt& operator+= (const pt &o) { x += o.x, y += o.y; return *this; }
	operator int () const { return std::abs(x) + std::abs(y); }
	void left()  { *this = { -y,  x }; }
	void right() { *this = {  y, -x }; }
	void back()  { *this = { -x, -y }; }
	void north(int n) { y += n; }
	void east(int n)  { x += n; }
};

}

output_t day12(input_t in) {
	pt p1, d1{1,0}, p2, d2{10,1};

	while (in.len) {
		char c = *in.s++; in.len--;
		auto n = parse::positive<int>(in);
		parse::skip(in, 1);
		switch (c) {
		    case 'S': n = -n;
		    case 'N': p1.north(n),  d2.north(n);  break;

		    case 'W': n = -n;
		    case 'E': p1.east(n),   d2.east(n);   break;

		    case 'F': p1 += d1 * n, p2 += d2 * n; break;

		    case 'R': n = 360 - n;
		    case 'L': switch (n) {
				  case  90: d1.left(),  d2.left();  break;
				  case 180: d1.back(),  d2.back();  break;
				  case 270: d1.right(), d2.right(); break;
			      }
		}
	}

	int part1 = p1, part2 = p2;

	return { part1, part2 };
}
