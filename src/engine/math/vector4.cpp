#include "vector4.h"

BEGIN_SCRIPT_CLASS(Vec4f)
	SCRIPT_STATIC_CONSTRUCTOR4(Float,Float,Float,Float)
	SCRIPT_ATTRIBUTE(x,Float)
	SCRIPT_ATTRIBUTE(y,Float)
	SCRIPT_ATTRIBUTE(z,Float)
	SCRIPT_OPERATOR_ADD
	SCRIPT_OPERATOR_SUB
	SCRIPT_RESULT_METHOD(GetLength,Float)
END_SCRIPT_CLASS

//..

Vec4f & Vec4f::operator=(const Vec4f & _other)
{
	x = _other.x;
	y = _other.y;
	z = _other.z;
	w = _other.w;

	return (*this);
}   

//..

Bool Vec4f::operator==(const Vec4f& _other) const
{
    return x == _other.x
        && y == _other.y
        && z == _other.z
        && w == _other.w;
}

//..

Vec4f Vec4f::operator+( const Vec4f & _other )
{
    return Vec4f( x + _other.x, y + _other.y, z + _other.z, w + _other.w );
}

//..

Vec4f & Vec4f::operator+=( const Vec4f & _other )
{
    x += _other.x;
    y += _other.y;
    z += _other.z;
    w += _other.w;
    return (*this);
}

//..

Vec4f Vec4f::operator-( const Vec4f & _other )
{
    return Vec4f( x - _other.x, y - _other.y, z - _other.z, w - _other.w );
}

//..

Vec4f & Vec4f::operator-=( const Vec4f & _other )
{
    x -= _other.x;
    y -= _other.y;
    z -= _other.z;
    w -= _other.w;
    return (*this);
}

//..

Float Vec4f::GetLength2() const
{
    return x*x + y*y + z*z + w*w;
}

//..

Float Vec4f::GetLength() const
{
	return ::sqrt( GetLength2() );
}

//..

const Vec4f & Vec4f::Normalize()
{
    Float length = GetLength();

    if (length == 0.0)
    {
        x = y = z = 0.0;
        w = 1.0;
    }
    else
    {
        x /= length;
        y /= length;
        z /= length;
        w /= length;
    }
    return *this;
}

//..

Vec4f Vec4f::Zero( 0.f, 0.f, 0.f, 0.f );