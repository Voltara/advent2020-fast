#ifndef _ADVENT2020_BITS
#define _ADVENT2020_BITS

// Iterate over bits using range-for syntax:
//   for (int i : bits(mask))
template<typename T>
struct bits {
        T mask;
        bits(T mask) : mask(mask)             { }
        bits& operator++ ()                   { mask &= mask - 1; return *this; }
        bool operator!= (const bits &o) const { return mask != o.mask; }
        inline int operator* () const         { return __builtin_ctzll(mask); }
        bits begin() const                    { return mask; }
        bits end() const                      { return 0; }
};

#endif
