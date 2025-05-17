#pragma once

#include <functional>
#include <string>
#include <vector>

typedef std::vector<std::string>& strvecr;
typedef long double ld;
typedef std::function<ld(ld)> Function;
typedef std::function<ld(ld, ld)> Operator;

void sortStrVecByFirstChar(strvecr);

extern ld EPSILON_SQRT;
