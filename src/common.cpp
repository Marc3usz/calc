#include "common.hpp"

extern ld EPSILON_SQRT = std::sqrt(std::numeric_limits<ld>::epsilon()); // na stacku pisze ze ma najlepsza precyzje do pochodnej

void sortStrVecByFirstChar(strvecr vec) {
    auto partition = [](strvecr v, int low, int high) -> int {
        char pivot = v[high].empty() ? '\0' : v[high][0];
        int i = low - 1;
        for (int j = low; j < high; ++j) {
            char c = v[j].empty() ? '\0' : v[j][0];
            if (c < pivot) {
                ++i;
                std::swap(v[i], v[j]);
            }
        }
        std::swap(v[i + 1], v[high]);
        return i + 1;
        };

    std::function<void(strvecr, int, int)> quicksort = [&](strvecr v, int low, int high) {
        if (low < high) {
            int pi = partition(v, low, high);
            quicksort(v, low, pi - 1);
            quicksort(v, pi + 1, high);
        }
        };

    if (!vec.empty()) {
        quicksort(vec, 0, static_cast<int>(vec.size()) - 1);
    }
}

