#ifndef __Vec3f_H__
    #define __Vec3f_H__

#include "math_helper.h"
#include "math/vector2.h"

class Vec3f
{
	DECLARE_SCRIPT_CLASS_FINAL(Vec3f)

public :    
	Vec3f(): x(0.f), y(0.f), z(0.f) { }
	Vec3f( Float _xyz ): x(_xyz), y(_xyz), z(_xyz) {  }
	Vec3f( Float _x, Float _y, Float _z ): x(_x), y(_y), z(_z) { }
	~Vec3f() { } 

	void Set( Float _x, Float _y, Float _z ) { x = _x; y = _y; z = _z; }

	Vec3f operator+( const Vec3f & _other ) const ;
	Bool operator==( const Vec3f & _other ) const;
	Bool operator!=( const Vec3f & _other ) const { return !(*this == _other); }
	Vec3f & operator+=( const Vec3f & _other );
	Vec3f operator-( const Vec3f & _other ) const ;
	Vec3f & operator-=( const Vec3f & _other );
	Float operator*( const Vec3f & _other ) const ;
	Vec3f operator*( const Float _value ) const ;
	Vec3f & operator*=( const Float _value );
	Vec3f operator/( const Float _value ) const ;
	Vec3f & operator/=( const Float _value );
	Vec3f operator^( const Vec3f& _other ) const;

	Bool IsNull() const { return GetLength2() == 0.f; }

	Float GetLength2() const;
	Float GetLength() const;
	const Vec3f & Normalize();
	const Vec3f & Maximize(Float _maxLength);

	const Vec3f CrossProduct( const Vec3f& _other ) const;
	Float ScalarProduct( const Vec3f& _other ) const;
	Vec3f Product(const Vec3f& other ) const;
	Vec3f Divide(const Vec3f& other ) const;

	Vec3f & RotateX( const Float _angle );
	Vec3f & RotateY( const Float _angle );
	Vec3f & RotateZ( const Float _angle );

	void	AddToNorm(Float _normToAdd);

	union
	{
		struct 
		{
			Float x, y, z;
		};
		Float xyz[3];
		struct
		{
			Vec2f xy;
			Float z;
		};
		struct
		{
			Float x;
			Vec2f yz;
		};
	};

	static Vec3f Zero; 
	static Vec3f One; 
	static Vec3f XAxis;
	static Vec3f YAxis;
	static Vec3f ZAxis;
};

class Vec3fArray : public ArrayOf<Vec3f>
{
	DECLARE_SCRIPT_CLASS(Vec3fArray)
};

//..

inline Vec3f operator-( const Vec3f _v )
{
	return Vec3f( -_v.x, -_v.y, -_v.z );
}

inline Vec3f operator*( Float _value, const Vec3f& _v  ) 
{
	return _v * _value;
}

#endif