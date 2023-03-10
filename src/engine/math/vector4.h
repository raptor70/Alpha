#ifndef __Vec4f_H__
    #define __Vec4f_H__

#include "math_helper.h"
#include "vector3.h"

class Vec4f
{
	DECLARE_SCRIPT_CLASS_FINAL(Vec4f)

public :
	Vec4f(): x(0.f), y(0.f), z(0.f), w(0.f) { }
	Vec4f( Float _x, Float _y, Float _z, Float _w ): x(_x), y(_y), z(_z), w(_w) { }
	Vec4f( const Vec4f & _other ): x(_other.x), y(_other.y), z(_other.z), w(_other.w) { }
	Vec4f(const Vec3f& _vec3, Float _w) : x(_vec3.x), y(_vec3.y), z(_vec3.z), w(_w) { }
	~Vec4f() { }
	Vec4f & operator=( const Vec4f & _other );

	Bool operator==(const Vec4f& _other) const;
	Bool operator!=(const Vec4f& _other) const { return !(*this == _other); }

	Vec4f operator+( const Vec4f & _other );
	Vec4f & operator+=( const Vec4f & _other );
	Vec4f operator-( const Vec4f & _other );
	Vec4f & operator-=( const Vec4f & _other );

	Float GetLength2() const;
	Float GetLength() const;

	const Vec4f & Normalize();

	union
	{
		struct 
		{
			Float x, y, z, w;
		};
		struct
		{
			Vec2f xy;
			Float z;
			Float w;
		};
		struct
		{
			Vec3f xyz;
			Float w;
		};
		Float xyzw[4];
	};

	static Vec4f Zero;
};

#endif