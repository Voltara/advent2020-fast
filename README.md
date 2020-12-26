# advent2020-fast

[Advent of Code 2020](https://adventofcode.com/2020/) optimized C++ solutions.

Here are the timings from an example run on an i9-9980HK CPU laptop.

    Day 01        41 μs
    Day 02        13 μs
    Day 03         3 μs
    Day 04        41 μs
    Day 05         2 μs
    Day 06        21 μs
    Day 07       430 μs
    Day 08         9 μs
    Day 09        80 μs
    Day 10         2 μs
    Day 11       264 μs
    Day 12         9 μs
    Day 13         2 μs
    Day 14     1,051 μs
    Day 15   153,485 μs
    Day 16        45 μs
    Day 17        41 μs
    Day 18        70 μs
    Day 19        26 μs
    Day 20        15 μs
    Day 21        53 μs
    Day 22       104 μs
    Day 23   134,652 μs
    Day 24        75 μs
    Day 25         4 μs
    -------------------
    Total:   290,538 μs

Solutions should work with any puzzle input, provided it is byte-for-byte an exact copy of the file downloaded from Advent of Code.

This code makes use of SIMD instructions, and requires an x86 CPU that supports the AVX2 instruction set.

Two of the solutions depend on 2MiB huge pages to speed up random memory access.  To enable on Linux:

    sudo sysctl -w vm.nr_hugepages=64

# Summary of solutions

Here are a few brief notes about each solution.

## Day 1

Uses a custom implementation of a 2048-bit set, which is used both as a lookup table, and as a way to very quickly sort the list of values.  Sorting the list speeds up Part 2 significantly because the distribution of values is biased toward the upper end of the range.  Breaking out of the inner loop when the sum becomes too high is especially effective.

## Day 2

Uses SIMD instructions to find the matching characters and convert their positions to a bit mask.

## Day 3

Parses the input lines using SIMD instructions to match the '#' characters and convert to a bit mask.

## Day 4

Field names and eye colors are matched using minimal perfect hash functions.

As an example, packing the character codes for `cid` into a 24-bit integer gives the value `0x636964`.  Applying the function `x % 477 % 8` to that value results in the number `2`.  Hashing each of the eight valid field names in this manner results in a unique value in the range 0 through 7.

## Day 5

The character codes for 'L', 'R', 'B', 'F' in binary are:

    R 01010[0]10
    L 01001[1]00
    B 01000[0]10
    F 01000[1]10

Inverting the bit in the marked position gives exactly the binary digit needed for each input letter.  This bit can be extracted easily and in parallel using SIMD instructions.  The only catch is the bits are returned in reverse order from what we need; this is resolved using a vector shuffle.

The [cumulative XOR](https://oeis.org/A003815), which can be computed in `O(1)` time, is used to find the missing number in range.

## Day 6

Bitwise AND and OR for set intersection and union.

## Day 7

Memoized recursion using `unordered_map` to convert bag colors to natural numbers.

## Day 8

A graph problem dressed up in an assembly code costume, solved using memoized depth first search.

## Day 9

Uses a rolling-window lookup table to avoid `O(25^2)` operations per step.

## Day 10

Values are sorted using a 256-bit set (similar to Day 1).  Part 2 is solved using bottom-up dynamic programming.

## Day 11

Cells are represented as 4-bit fields within a 256-bit SIMD register.

In Part 2, distant vertical and diagonal neighbors are summed by making top-down and bottom-up passes over the grid, shifting left/right by 4 bits at a time.  Horziontal neighbors are more of a problem because AVX2 does not easily support variable-distance bitwise shifts, and because comparatively little useful work is parallelized in that direction.  To compensate, the shift distances needed for each row are determined ahead of time.

## Day 12

Straightforward manipulation of the coordinates.

## Day 13

Solves the system of congruences using modular multiplicative inverse.

## Day 14

The input is converted to a list of writes to individual memory addresses.  The list is then sorted using a stable radix sort by address.  The most recent write for each address is found by scanning the array in reverse.

## Day 15

Most of the time is spent with the CPU stalled waiting on memory access.  To improve performance, the array is allocated using [2MiB huge pages](https://en.wikipedia.org/wiki/Page_%28computer_memory%29#Multiple_page_sizes) to reduce TLB cache pressure.  A bitset is also checked before looking up values in the larger array to avoid unnecessary cache misses for numbers not yet encountered.

## Day 16

Creates a mapping from each number to the set of valid fields represented as a bit mask.  This is accomplished efficiently by marking the start and end of each subrange, then constructing a prefix sum array in `O(n)` time (n is the range of values, roughly 1000.)  The field mapping is solved by process of elimination.

## Day 17

Cells are represented as 4-bit fields in an array of 128-bit integers.  Neighbors are counted using [SWAR](https://en.wikipedia.org/wiki/SWAR) [saturating](https://en.wikipedia.org/wiki/Saturation_arithmetic) addition.  Ignoring the parallelism, a cell's neighbors can be counted using only `2 * dimensions` additions: `1+1+1=3, 3+3+3=9, 9+9+9=27, 27+27+27=81`.

Because the seed is symmetric in 1 or 2 dimensions, the automaton evolves symmetrically in those dimensions.  Some time is saved by taking advantage of this symmetry to avoid redundant computation.

## Day 18

Uses a stack of integers to evaluate the expressions, with operators represented using negative numbers.

## Day 19

Exploits properties of the language specified by the grammar.  Rules 31 and 42 each match a complementary set of 128 eight-letter words.  Makes use of SIMD instructions to map each eight-letter sequences to an index into a 256-bit set.

## Day 20

Each tile is represented as an array of four 16-bit integers (edges) and a single 64-bit integer (the 8x8 core).  The group of eight transformations are implemented by applying zero, one, or two primitive reflections (horizontal, vertical, diagonal, antidiagonal) to the core.  The primitive reflections are done by efficient bitwise operations.

The lookup table for matching up tile edges also keeps track of the relative orientation of the tiles.  This eliminates the need to guess and check; the correct orientation can be applied directly to each tile.

Sea monsters are identified by a minimal sequence of bitwise shift and AND operations that take advantage of periodicity within the sea monster's shape.

## Day 21

All input words are short enough to be packed into a 64-bit integer using 7 bits for each character.  The input has eight different allergens selected from a pool of only nine possibilities; these are mapped to numbers in the range 0-8 using a minimal perfect hash.

Each ingredient is mapped to a 64-bit integer sliced into 5-bit fields, each field containing the number of times that ingredient is paired with each allergen.  An additional 5-bit field counts the total number of times that ingredient occurs.  The mapping from ingredients to counts is done using a custom hash table implementation (open addressing, linear probing.)

## Day 22

This one was a challenge to optimize, and I'm still not completely satisfied with the results.  The amount of time varies widely depending on the starting shuffle.  Although my input is solved in around 100 μs, a more typical time is closer to 3,000 μs with outliers exceeding 15,000 μs.  The variance stems from how effectively it can prune subgames based on the rule that Player 1 always wins when in possession of the high card.

The two hands of cards are stored as circular arrays of size 64.  For cycle detection, the game state is represented by a rolling hash using the [BUZ Hash](https://en.wikipedia.org/wiki/Rolling_hash#Cyclic_polynomial) algorithm, which can be updated incrementally as cards are drawn and won.  Although unlikely, this can produce an incorrect answer in the event of a hash collision.  To mitigate the cost of accessing the `unordered_set` of hashes each round, repeated game state is tested only when the high card is about to be drawn.

## Day 23

The circular sequence of cups is stored as a successor list.  Similar to Day 15, this is bound by memory latency and benefits from huge page allocation.

## Day 24

With fewer neighbors to consider in this cellular automaton, the cells can be stored using only 3 bits each.

Input is parsed using SIMD instructions to find the positions of `n`, `s`, `w`, `e` on each line and convert them to bit masks which can be quickly converted into [axial coordinates](https://www.redblobgames.com/grids/hexagons/#coordinates-axial).

## Day 25

Two techniques are applied to speed up computing the discrete logarithm modulo `20201227`.  Because `φ(20201227) = 20201226 = 2 * 3 * 29 * 116099`, the task can be split into four smaller subproblems using the [Pohlig-Hellman algorithm](https://en.wikipedia.org/wiki/Pohlig%E2%80%93Hellman_algorithm).  The four answers are then combined using the Chinese Remainder Theorem.

Each of the four factors is solved by a different method.  `2` is a simple equality test.  `3` extracts two bits from `n^6733742 % 20201227` that just happen to align perfectly with the result.  `29` uses a perfect hash and a SIMD lookup table.  Finally, `116099` uses the [baby-step giant-step](https://en.wikipedia.org/wiki/Baby-step_giant-step) algorithm to meet-in-the-middle in `O(sqrt(116099))` time.
