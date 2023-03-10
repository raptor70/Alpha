#include "vector3.h"
#include "debug/debug.h"
#include "math_constant.h"

BEGIN_SCRIPT_CLASS(Vec3f)
	SCRIPT_STATIC_CONSTRUCTOR3(Float,Float,Float)
	SCRIPT_ATTRIBUTE(x,Float)
	SCRIPT_ATTRIBUTE(y,Float)
	SCRIPT_ATTRIBUTE(z,Float)
	SCRIPT_OPERATOR_ADD
	SCRIPT_OPERATOR_SUB
	SCRIPT_OPERATOR_MUL1(Float)
	SCRIPT_OPERATOR_DIV1(Float)
	SCRIPT_VOID_METHOD3(Set,Float,Float,Float)
	SCRIPT_RESULT_METHOD(GetLength,Float)
	SCRIPT_RESULT_METHOD1(CrossProduct,Vec3f,Vec3f)
	SCRIPT_RESULT_METHOD1(ScalarProduct,Float,Vec3f)
END_SCRIPT_CLASS

BEGIN_SCRIPT_CLASS(Vec3fArray)
	SCRIPT_STATIC_CONSTRUCTOR
	SCRIPT_VOID_METHOD1(AddLastItem,Vec3f)
END_SCRIPT_CLASS

//..

Vec3f Vec3f::operator+( const Vec3f & _other ) const 
{
    return Vec3f( x + _other.x, y + _other.y, z + _other.z );
}

//..

Bool Vec3f::operator ==( const Vec3f & _other ) const
{
    return x == _other.x && y == _other.y && z == _other.z;
}

//..

Vec3f & Vec3f::operator+=( const Vec3f & _other )
{
    x += _other.x;
    y += _other.y;
    z += _other.z;
    return (*this);
}

//..

Vec3f Vec3f::operator-( const Vec3f & _other ) const 
{
    return Vec3f( x - _other.x, y - _other.y, z - _other.z );
}

//..

Vec3f & Vec3f::operator-=( const Vec3f & _other )
{
    x -= _other.x;
    y -= _other.y;
    z -= _other.z;
    return (*this);
}

//..

Float Vec3f::operator*( const Vec3f & _other ) const 
{
    return x * _other.x + y * _other.y + z * _other.z;
}

//..

Vec3f Vec3f::operator*( const Float _value ) const 
{
    return Vec3f( x * _value, y * _value, z * _value );
}

//..

Vec3f & Vec3f::operator*=( const Float _value )
{
    x *= _value;
    y *= _value;
    z *= _value;
    return (*this);
}

//..

Vec3f Vec3f::operator/( const Float _value ) const 
{
    DEBUG_Require( _value != 0.0f )

    return Vec3f( x / _value, y / _value, z / _value );
}

//..

Vec3f & Vec3f::operator/=( const Float _value )
{
    DEBUG_Require( _value != 0.0f )

    x /= _value;
    y /= _value;
    z /= _value;
    return (*this);
}

//..

Vec3f Vec3f::operator^( const Vec3f& _other ) const
{
	return Vec3f(
        y * _other.z - z * _other.y,
        z * _other.x - x * _other.z,
        x * _other.y - y * _other.x
        );
}

//..

Float Vec3f::GetLength2() const
{
    return  x*x + y*y + z*z;
}

//..

Float Vec3f::GetLength() const
{
    return sqrt( GetLength2() );
}

//..

const Vec3f & Vec3f::Normalize()
{
    Float length2 = GetLength2();
    if (length2 < Float_Eps)
    {
        x = y = z = 0.f;
    }
    else
    {
		Float factor = 1.f / sqrt(length2);
        (*this) *= factor;
    }

    return *this;
}

//..

const Vec3f & Vec3f::Maximize(Float _maxLength)
{
	Float length2 = GetLength2();
	if (length2 > _maxLength * _maxLength)
    {
		Float factor = _maxLength / sqrt(length2);
        (*this) *= factor;
    }

	return *this;
}

//..

const Vec3f Vec3f::CrossProduct( const Vec3f& _other ) const
{
    return Vec3f(
        y * _other.z - z * _other.y,
        z * _other.x - x * _other.z,
        x * _other.y - y * _other.x
        );
}

//..

Float Vec3f::ScalarProduct( const Vec3f& _other ) const
{
    return x * _other.x + y * _other.y + z * _other.z;
}

//..

Vec3f Vec3f::Product(const Vec3f& _other ) const
{
	return Vec3f(
        x * _other.x,
        y * _other.y,
        z * _other.z
        );
}

//..
	
Vec3f Vec3f::Divide(const Vec3f& _other ) const
{
	DEBUG_Require(_other.x > Float_Eps);
	DEBUG_Require(_other.y > Float_Eps);
	DEBUG_Require(_other.z > Float_Eps);
	return Vec3f(
        x / _other.x,
        y / _other.y,
        z / _other.z
        );
}

//..

Vec3f & Vec3f::RotateX( const Float _angle )
{
    Float sinus( sin( _angle ) );
	Float cosinus( cos( _angle ) );
    Float local_y(y);
	Float local_z(z);

    y = local_y * cosinus - local_z * sinus;
    z = local_y * sinus + local_z * cosinus;
    return (*this);
}

//..

Vec3f & Vec3f::RotateY( const Float _angle )
{
    Float sinus( sin( _angle ) );
    Float cosinus( cos( _angle ) );
    Float local_x(x);
	Float local_z(z);

    z = local_z * cosinus - local_x * sinus;
    x = local_z * sinus + local_x * cosinus;
    return (*this);
}

//..

Vec3f & Vec3f::RotateZ( const Float _angle )
{
    Float sinus( sin( _angle ) );
    Float cosinus( cos( _angle ) );
    Float local_y(y);
	Float local_x(x);

    x = local_x * cosinus - local_y * sinus;
    y = local_x * sinus + local_y * cosinus;
    return (*this);
}

//..

void	Vec3f::AddToNorm(Float _normToAdd)
{
	Float fLength = GetLength();
	if (fLength > Float_Eps)
	{
		Float fFactor = (fLength + _normToAdd) / fLength;
		x *= fFactor;
		y *= fFactor;
		z *= fFactor;
	}
}

//..

Vec3f Vec3f::Zero( 0.f, 0.f, 0.f );
Vec3f Vec3f::One( 1.f, 1.f, 1.f );
Vec3f Vec3f::XAxis( 1.f, 0.f, 0.f );
Vec3f Vec3f::YAxis( 0.f, 1.f, 0.f );
Vec3f Vec3f::ZAxis( 0.f, 0.f, 1.f );
