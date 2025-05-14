#include "derivative.hpp"

Function derivative(Function fn) {
	return [&fn](ld x) {
		return (fn(x + EPSILON_SQRT) - fn(x - EPSILON_SQRT)) / 2 * EPSILON_SQRT; // wzor na pochodna obustronna, podobno dokladniejsza w komputerach
		};
}