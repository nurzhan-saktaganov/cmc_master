#include <iostream>
#include "Complex.hpp"

int main(void){
	Complex c0, c1(1), c2(1.2,2);
	std::cout << c0 << " " << c1 << " " << c2 << std::endl;
	std::cout << ((c0 + 1) / Complex(0,1)) << " " << c1 * Complex(0,1) << " " << c2 / 2 << std::endl;
}