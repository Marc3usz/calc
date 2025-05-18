#include "getZeroes.hpp"
#include "derivative.hpp"
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>

std::vector<ld> getZeroes(Function fn) {

    std::vector<ld> roots;
    Function dfn = derivative(fn);

    const ld EPSILON = 1e-10;        
    const int MAX_ITERATIONS = 100;   
    const ld DOMAIN_MIN = -50.0;     
    const ld DOMAIN_MAX = 50.0;      
    const int NUM_STARTING_POINTS = 200000; 

    
    for (int i = 0; i < NUM_STARTING_POINTS; i++) {
        
        ld x = DOMAIN_MIN + (DOMAIN_MAX - DOMAIN_MIN) * i / (NUM_STARTING_POINTS - 1);

        bool converged = false;
        for (int iter = 0; iter < MAX_ITERATIONS; iter++) {
            ld fx = fn(x);
            ld dfx = dfn(x);
            
            if (std::abs(fx) < EPSILON) {
                converged = true;
                break;
            }

            if (std::abs(dfx) < EPSILON) { // nie dzielimy przez strasznie male liczby, bo by dawaly bardzo glupie wyniki (punkt przeciecia osi OX z styczna x^2 dla x~=0)
                break;
            }

            ld delta = fx / dfx;
            x = x - delta;
            
            if (x < DOMAIN_MIN || x > DOMAIN_MAX) {
                break;
            }
            
            if (std::abs(delta) < EPSILON) {
                converged = true;
                break;
            }
        }
        
        if (converged) {
            
            bool isDuplicate = false;
            for (ld root : roots) {
                if (std::abs(root - x) < EPSILON) {
                    isDuplicate = true;
                    break;
                }
            }

            if (!isDuplicate) {
                roots.push_back(x);
            }
        }
    }

    std::sort(roots.begin(), roots.end());

    return roots;
}