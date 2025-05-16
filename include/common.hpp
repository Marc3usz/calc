#pragma once

#include <functional>
#include <string>

typedef long double ld;
typedef std::function<ld(ld)> Function;
typedef std::function<ld(ld, ld)> Operator;

extern ld EPSILON_SQRT;
