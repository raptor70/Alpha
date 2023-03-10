#ifndef __Quat_H__
    #define __Quat_H__

#include "vector4.h"
#include "vector3.h"
#include "matrix4x4.h"
#include "math_constant.h"

class Quat
{
	DECLARE_SCRIPT_CLASS_FINAL(Quat)

	friend Quat LinearInterpolation(const Quat&, const Quat&, Float );
public :    
	Quat( void ): vec4( 0.0f, 0.f, 0.f, 1.0f ) { }
	Quat( Float _x, Float _y, Float _z, Float _w ): vec4(_x,_y,_z,_w) { }
	Quat( Float _angle, const Vec3f& _axis)	{ SetFromAxis(_angle,_axis); }
	Quat(const Vec3f& _a, const Vec3f& _b) { SetFromVectors(_a, _b); }
	Quat( const Quat & _other ): vec4(_other.vec4) { }
	~Quat() {}

	Quat	operator*(const Quat& _other) const;
	Quat&	operator*=(const Quat& _other);
	Quat	operator-(const Quat& _other) const { return (*this)*(_other.GetInverse()); }
	Quat	operator+(const Quat& _other) const { return (*this)*_other; }
	Quat& operator=(const Quat& _other) { vec4 = _other.vec4; return (*this);  }
	Quat& operator=(const Vec4f& _other) { vec4 = _other; return (*this); }

	Vec3f	operator*(const Vec3f& _p) const;
	Quat	operator*(Float _value) const;

	const Quat &	SetFromVectors(const Vec3f& _a, const Vec3f& _b);
	const Quat &	SetFromAxis( Float _angle, const Vec3f _axis );
	void			GetAngleAxis(Float& _angle, Vec3f& _axis) const;

	const Quat&		SetFromMatrix(const Mat44& _mat);
	const Mat44 GetTranformMatrix() const;
	Quat GetInverse() const;

	Vec3f		GetEulerAngles() const;
	const Quat&		SetFromEulerAngles(const Vec3f& _eulers);

	union
	{
		struct
		{
			Float x,y,z,w;
		};
		struct
		{
			Vec4f vec4;
		};
		Float xyzw[4];
	};

	static Quat Identity; 
};

Quat LinearInterpolation(const Quat& _a, const Quat& _b, Float _ratio);

#endif