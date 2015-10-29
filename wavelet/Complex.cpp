#include "Complex.hpp"

Complex Complex::operator+ (const Complex &c){
	return Complex(re + c.re, im + c.im);
}

Complex Complex::operator- (const Complex &c){
	return Complex(re - c.re, im - c.im);
}

Complex Complex::operator* (const Complex &c){
	return Complex(re * c.re - im * c.im, re * c.im + im * c.re);
}

Complex Complex::operator/ (const Complex &c){
	double tmp = c.re * c.re + c.im * c.im;
	return Complex((re * c.re - im * c.im) / tmp, (im * c.re - re * c.im) / tmp);
}

std::ostream& operator<<(std::ostream& out, const Complex par){
	out << par.re << "+" << par.im << "i";
	return out;
}