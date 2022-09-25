#ifndef _UTILITIES_h
#define _UTILITIES_h

#include <random>

inline double random_double()
{
    static std::uniform_real_distribution<double> dist(0.0, 1.0);
    static std::mt19937 generator;
    return dist(generator);
}

inline double random_double(double start, double end)
{
    static std::uniform_real_distribution<double> dist(start, end);
    static std::mt19937 generator;
    return dist(generator);
}

inline int random_int(int start, int end)
{
    static std::uniform_int_distribution<int> dist(start, end);
    static std::mt19937 generator;
    return dist(generator);
}

#endif
