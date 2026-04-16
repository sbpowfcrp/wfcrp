#pragma once

#include <cstdlib>

// Return a random int in [a, b).
inline int Random_A_B(int a, int b) { return (rand() % (b - a)) + a; }

inline double Random_0_1() { return (double)rand() / RAND_MAX; }

template <typename T>
void Shuffle(T *begin, T *end)
{
    if (begin >= end) return;

    std::swap(*begin, begin[rand() % (end - begin)]);

    Shuffle(begin + 1, end);
}
