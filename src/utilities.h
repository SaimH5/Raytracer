#ifndef _UTILITIES_h
#define _UTILITIES_h

#include <cmath>
#include <memory>
#include <random>
#include "fastPRNG.h"

// Using's

using std::shared_ptr;
using std::make_shared;

// Constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions
inline double degrees_to_radians(double degrees) 
{
    return degrees * pi / 180.0;
}

// double random_double()
// {
//     static std::uniform_real_distribution<double> dist(0.0, 1.0);
//     static std::mt19937 generator;
//     return dist(generator);
// }

// double random_double(double start, double end)
// {
//     static std::uniform_real_distribution<double> dist(start, end);
//     static std::mt19937 generator;
//     return dist(generator);
// }

// Random Number Generators

double random_double()
{
    static fastPRNG::fastXS64 fastR(42);
    return fastR.xoshiro256p_UNI<double>();
}

double random_double(double start, double end)
{
    static fastPRNG::fastXS64 fastR(42);
    return fastR.xoshiro256p_Range<double>(start, end);
}

int random_int(int start, int end)
{
    static std::uniform_int_distribution<int> dist(start, end);
    static std::mt19937 generator;
    return dist(generator);
}


double clamp(double value, double min, double max)
{
    value = value < min ? min : value;
    value = value > max ? max : value;
    return value;
}


// Common include's

#include "ray.h"
#include "vec3.h"

#endif
