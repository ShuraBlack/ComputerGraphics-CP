/////////////////////////////////////////////////////////////
// 
// Framework for computer graphics assignement 4
// 
// Some useful mathematical functions
// 
// (c) Georg Umlauf, 2022
// 
/////////////////////////////////////////////////////////////

#pragma once

// Conversion of indices of (x,y) coordinates to a linear array
#define TO_LINEAR(x, y) (((x)) + TEX_RES_X*((y)))

#define PI 3.1415926535

#define max(a ,b) (((a) > (b)) ? (a) : (b))
#define min(a ,b) (((a) < (b)) ? (a) : (b))

// crop x to interval [l,u]
float crop(float l, float u, float x)
{
	return min(u,max(l,x));
}
// crop all componentens of x to interval [l,u]
CVec3f crop(float l, float u, const CVec3f& x)
{
	return CVec3f(crop(l, u, x[0]), crop(l, u, x[1]), crop(l, u, x[2]));
}

// spherical to affine coordinates
// (x) PI <= XZ <= 2Pi (X)
// (y) 0 <= YZ <= PI (Y)
CVec3f SphericalToAffine(const CVec3f& sphericalCoordinates) 
{
	CVec3f affineCoordinates;
	affineCoordinates[0] = sphericalCoordinates[0] * sin(sphericalCoordinates[2]) * cos(sphericalCoordinates[1]);
	affineCoordinates[2] = sphericalCoordinates[0] * sin(sphericalCoordinates[2]) * sin(sphericalCoordinates[1]);
	affineCoordinates[1] = sphericalCoordinates[0] * cos(sphericalCoordinates[2]);
	cout << "L= " << affineCoordinates << endl;
	return affineCoordinates;
}
