#pragma once

#include <functional>
#include <limits>
#include <cmath>

typedef long double ld;
typedef std::function<ld(ld)> Function;
typedef std::function<ld(ld, ld)> Operator;

ld EPSILON_SQRT = std::sqrt(std::numeric_limits<ld>::epsilon()); // na stacku pisze ze ma najlepsza precyzje do pochodnej