#include "derivative.hpp"

Function derivative(Function fn) {
    Function _fn = fn;
   return [_fn](ld x) {
       return (_fn(x + EPSILON_SQRT) - _fn(x - EPSILON_SQRT)) / (2 * EPSILON_SQRT); // wzor na pochodna obustronna, podobno dokladniejsza w komputerach
   };
}