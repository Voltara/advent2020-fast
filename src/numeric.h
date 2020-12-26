#ifndef _ADVENT2020_NUMERIC
#define _ADVENT2020_NUMERIC

template<typename T>
static T fastmod(T n, T mod) {
	n -= mod & -(n >= mod);
	return n;
}

template<typename T>
static T gcdx(T a, T b, T &x, T &y) {
	x = 1, y = 0;
	T u = 0, v = 1, q;
	while (b) {
		q = a / b;
		x = std::exchange(u, x - q * u);
		y = std::exchange(v, y - q * v);
		a = std::exchange(b, a - q * b);
	}
	return a;
}

template<typename T>
static T modinv(T b, T mod) {
	T x, y;
	gcdx(b, mod, x, y);
	x += mod & -(x < 0);
	return x;
}

#endif
