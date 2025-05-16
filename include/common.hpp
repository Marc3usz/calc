#pragma once

#include <functional>

typedef long double ld;
typedef std::function<ld(ld)> Function;
typedef std::function<ld(ld, ld)> Operator;

extern ld EPSILON_SQRT;