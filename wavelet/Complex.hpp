#pragma once
#include <iostream>

class Complex{
public:
	Complex(double real = 0.0, double imagine = 0.0): re(real), im(imagine) {}
	Complex operator+ (const Complex& c);
	Complex operator- (const Complex& c);
	Complex operator* (const Complex& c);
	Complex operator/ (const Complex& c);
	double real() {return re;}
	double imag() {return im;}
	friend std::ostream& operator<<(std::ostream& out, const Complex par);
private:
	double re;
	double im;
};