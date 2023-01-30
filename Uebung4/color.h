/////////////////////////////////////////////////////////////
// 
// Framework for computer graphics assignement 4
// 
// A simple color class
// 
// (c) Georg Umlauf, 2022
// 
/////////////////////////////////////////////////////////////

#pragma once

#include "vec.h"

#include <iostream>
using namespace std;

// Eine überaus primitive Farbklasse
class Color: public CVec3f {
public:

	Color (float r=1.0f, float g=1.0f, float b=1.0f): CVec3f(r,g,b) {	}

	float r() const { return (*this)[0]; }
	float g() const { return (*this)[1]; }
	float b() const { return (*this)[2]; }

};

ostream& operator << (ostream &os, const Color& c)
{
	os << "(" << c[0] << ", " << c[1] << ", " << c[2] << ")" << endl;
	return os;
}

