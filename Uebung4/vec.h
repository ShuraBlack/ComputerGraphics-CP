/////////////////////////////////////////////////////////////
// 
// Framework for computer graphics assignement 4
// 
// A simple, generic vector class
// 
// (c) Georg Umlauf, 2022
// 
/////////////////////////////////////////////////////////////

#pragma once

#include <math.h>

// Vector class for SIMPLE data types
template <class T, unsigned DIM> class CVector 
{
public:
	// standard constructor
	CVector<T,DIM> ()  
	{
		// initialize all elements with zero
		for (int i=0; i<DIM; i++) m_atData[i] = T(0);
	}

	// construction with data array
	CVector<T, DIM> (const T atData[DIM]) 
	{
		for (int i=0; i<DIM; i++)	m_atData[i] = atData[i];		
	}	
	
	// construction with values
	CVector<T, DIM> (T x, T y=0, T z=0) 
	{
		T tmp[3] = {x,y,z};
		for (int i=0; i< (DIM<3? DIM: 3); i++) m_atData[i] = tmp[i];
	}

	// copy constructor
	CVector<T, DIM> (const CVector<T, DIM> &vec) 
	{
		if (this==&vec) return;		// nothing to do, it's me
		for (int i=0; i<DIM; i++)	m_atData[i] = vec.m_atData[i];
	}

	// destructor
	~CVector () { /* nothing to do here ...*/	}

	void setData (const T atData[DIM]) 
	{
		for (int i=0; i<DIM; i++)	m_atData[i] = atData[i];		
	}

	void getData (T atData[DIM]) const 
	{
		for (int i=0; i<DIM; i++)	atData[i] = m_atData[i];		
	}

	unsigned getDimension () const 
	{
		return DIM;
	}

	CVector<T, DIM> &operator = (const CVector<T, DIM> &vec) 
	{
		if (this==&vec) return (*this);										// ok, it's me, so no L-value action
		for (int i=0; i<DIM; i++) m_atData[i] = vec.m_atData[i];		// not me, so L-Value action: copy data		
		return (*this); 
	}

	CVector<T, DIM> &operator = (const T atData[DIM]) 
	{
		for (int i=0; i<DIM; i++) m_atData[i] = atData[i];				// copy data
		return (*this); 
	}

	T &operator [] (unsigned i) 
	{	
		return m_atData[i%DIM]; 
	}

	T operator [] (unsigned i) const 
	{
		return m_atData[i%DIM];
	}

	void operator += (const CVector<T, DIM> &vec) 
	{
		for (int i=0; i<DIM; i++)	m_atData[i] += vec.m_atData[i];
	}

	CVector<T, DIM> operator + (const CVector<T, DIM> &vec) const 
	{
		CVector<T, DIM> buf (m_atData);
		for (int i=0; i<DIM; i++)	buf.m_atData[i] += vec.m_atData[i];
		return buf;
	}

	void operator -= (const CVector<T, DIM> &vec) 
	{
		for (int i=0; i<DIM; i++)	m_atData[i] -= vec.m_atData[i];
	}
	
	CVector<T, DIM> operator - (const CVector<T, DIM> &vec) const 
	{
		CVector<T, DIM> buf (m_atData);
		for (int i=0; i<DIM; i++)	buf.m_atData[i] -= vec.m_atData[i];
		return buf;
	}

	// Kreuzprodukt: nur für CVec3 definiert!
	CVector<T, 3> cross (const CVector<T, 3> &vec) const 
	{
		T atBuffer[3];
		atBuffer[0] = m_atData[1] * vec.m_atData[2];
		atBuffer[0]-= m_atData[2] * vec.m_atData[1];
		atBuffer[1] = m_atData[2] * vec.m_atData[0];
		atBuffer[1]-= m_atData[0] * vec.m_atData[2];
		atBuffer[2] = m_atData[0] * vec.m_atData[1];
		atBuffer[2]-= m_atData[1] * vec.m_atData[0];
		return CVector<T, 3> (atBuffer);
	}
	
	// Skalarprodukt zweier beliebiger Vektoren gleicher Dimension.
	double dot (const CVector<T, DIM> &vec) const 
	{
		return (*this)*vec;
	}

	// Normiert die Laenge eines beliebigen Vektors auf 1.
	const CVector<T, DIM> normalize() const 
	{
		T atBuffer[DIM];

		double d = length();
		
		for (int i=0; i<DIM; i++)
			atBuffer[i] = static_cast<T>(static_cast<double>(m_atData[i]) / d);

		return CVector<T, DIM> (atBuffer);
	}

	// liefert die laenge dieses vektors
	double length() const 
	{
		double d= 0;
		for (int i=0; i < DIM; i++) d += m_atData[i] * m_atData[i];
		return sqrt(d);
	}

	void print(char* name= "v") const 
	{
		std::cout << name << " = ";
		std::cout << "(" << m_atData[0] << ", " << m_atData[1] << ", " << m_atData[2] << ")" << std::endl;
	}
	
	CVector<T, DIM> operator - () const
	{
		T atBuffer[DIM];
		for (int i=0; i<DIM; i++) atBuffer[i] = -m_atData[i];
		return CVector<T, DIM> (atBuffer);
	}

	T operator * (const CVector<T, DIM> & vec) const 
	{
		T dp = T(0);
		for (int i=0; i<DIM; i++) dp += m_atData[i]*vec.m_atData[i];
		return dp;
	}

	void operator *= (T tScale) 
	{
		for (int i=0; i<DIM; i++) m_atData[i] *= tScale;
	}

	CVector<T, DIM> operator * (T tScale) const 
	{
		T atBuffer[DIM];
		for (int i=0; i<DIM; i++) atBuffer[i] = m_atData[i]*tScale;
		return CVector<T, DIM> (atBuffer);
	}

private:

	T m_atData[DIM];

}; // class CVector

template <class T, unsigned DIM> ostream& operator << (ostream &os, const CVector<T,DIM>& c)
{
	os << "(";
	for (int j = 0; j < DIM-1; j++) os << c[j] << ",";
	os << c[DIM-1] << ")";
	return os;
}

// some useful vector classes (abbr. names)
typedef CVector<float, 2> CVec2f;
typedef CVector<float, 3> CVec3f;
typedef CVector<float, 4> CVec4f;

typedef CVector<double, 2> CVec2d;
typedef CVector<double, 3> CVec3d;
typedef CVector<double, 4> CVec4d;

typedef CVector<int, 2> CVec2i;
typedef CVector<int, 3> CVec3i;
typedef CVector<int, 4> CVec4i;

typedef CVector<bool, 2> CVec2b;
typedef CVector<bool, 3> CVec3b;
typedef CVector<bool, 4> CVec4b;
