#ifndef __VEC2F_H__
#define __VEC2F_H__

#include "math_helper.h"

class Vec2f
{
	DECLARE_SCRIPT_CLASS_FINAL(Vec2f)
public :    
	Vec2f(): x(0.f), y(0.f) { }
	Vec2f( Float _x, Float _y ): x(_x), y(_y) { }
	~Vec2f() { } 

	Vec2f operator+( const Vec2f & _other ) const ;
	Bool operator==( const Vec2f & _other ) const;
	Bool operator!=( const Vec2f & _other ) const;
	Vec2f & operator+=( const Vec2f & _other );
	Vec2f operator-( const Vec2f & _other ) const ;
	Vec2f & operator-=( const Vec2f & _other );
	Float operator*( const Vec2f & _other ) const ;
	Vec2f operator*( const Float _value ) const ;
	Vec2f & operator*=( const Float _value );
	Vec2f operator/( const Float _value ) const ;
	Vec2f & operator/=( const Float _value );

	Bool IsNull() const { return GetLength2() == 0.f; }

	Float GetLength2() const;
	Float GetLength() const;
	const Vec2f & Normalize();

	union
	{
		struct
		{
			Float x, y;
		};
		Float xy[2];
	};

	static Vec2f Zero;
	static Vec2f One;
	static Vec2f XAxis;
	static Vec2f YAxis;
};

typedef ArrayOf<Vec2f> Vec2fArray;

//..

inline Vec2f operator-( const Vec2f& _v )
{
	return Vec2f( -_v.x, -_v.y );
}

//..

class Vec2i
{
public :    
	Vec2i(): x(0), y(0) { }
	Vec2i( S32 _v ): x(_v), y(_v) { }
	Vec2i( S32 _x, S32 _y ): x(_x), y(_y) { }
	Vec2i( const Vec2i & _other ): x(_other.x), y(_other.y) { }
	virtual ~Vec2i() { } 

	Vec2i & operator=( const Vec2i & _other );
	Vec2i operator+( const Vec2i & _other ) const ;
	Bool operator==( const Vec2i & _other ) const;
	Bool operator!=( const Vec2i & _other ) const;
	Vec2i & operator+=( const Vec2i & _other );
	Vec2i operator-( const Vec2i & _other ) const ;
	Vec2i & operator-=( const Vec2i & _other );
	Vec2i & operator*=(const Vec2i & _other);

	Float GetLength2() const { return  Float(x*x + y*y); }
	Float GetLength() const	{ return sqrt(GetLength2());}

	S32 x, y;

	static Vec2i Zero;
	static Vec2i One;
	static Vec2i XAxis;
	static Vec2i YAxis;
};

//..

inline Vec2i operator-( const Vec2i& _v )
{
	return Vec2i( -_v.x, -_v.y );
}

typedef ArrayOf<Vec2i> Vec2iArray;

#endif