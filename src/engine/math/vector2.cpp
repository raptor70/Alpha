#include "vector2.h"
#include "math_constant.h"

BEGIN_SCRIPT_CLASS(Vec2f)
	SCRIPT_STATIC_CONSTRUCTOR2(Float,Float)
	SCRIPT_ATTRIBUTE(x,Float)
	SCRIPT_ATTRIBUTE(y,Float)
END_SCRIPT_CLASS

//..

Vec2f Vec2f::operator+( const Vec2f & _other ) const 
{
    return Vec2f( x + _other.x, y + _other.y );
}

//..

Bool Vec2f::operator ==( const Vec2f & _other ) const
{
    return x == _other.x && y == _other.y;
}

//..

Bool Vec2f::operator!=(const Vec2f& _other) const
{
    return !(*this == _other);
}

//..

Vec2f & Vec2f::operator+=( const Vec2f & _other )
{
    x += _other.x;
    y += _other.y;
    return (*this);
}

//..

Vec2f Vec2f::operator-( const Vec2f & _other ) const 
{
    return Vec2f( x - _other.x, y - _other.y );
}

//..

Vec2f & Vec2f::operator-=( const Vec2f & _other )
{
    x -= _other.x;
    y -= _other.y;
    return (*this);
}

//..

Float Vec2f::operator*( const Vec2f & _other ) const 
{
    return x * _other.x + y * _other.y;
}

//..

Vec2f Vec2f::operator*( const Float _value ) const 
{
    return Vec2f( x * _value, y * _value );
}

//..

Vec2f & Vec2f::operator*=( const Float _value )
{
    x *= _value;
    y *= _value;
    return (*this);
}

//..

Vec2f Vec2f::operator/( const Float _value ) const 
{
    DEBUG_Require( _value != 0.f );

    return Vec2f( x / _value, y / _value );
}

//..

Vec2f & Vec2f::operator/=( const Float _value )
{
    DEBUG_Require( _value != 0.0f );

    x /= _value;
    y /= _value;
    return (*this);
}

//..

Float Vec2f::GetLength2() const
{
    return  x*x + y*y;
}

//..

Float Vec2f::GetLength() const
{
    return sqrt( GetLength2() );
}

//..

const Vec2f & Vec2f::Normalize()
{
    Float length = GetLength();
    if (length == 0.0)
    {
        x = y = 0.0;
    }
    else
    {
        x /= length;
        y /= length;
    }

    return *this;
}

//..

Vec2f Vec2f::Zero( 0.f,0.f );
Vec2f Vec2f::One( 1.f,1.f );
Vec2f Vec2f::XAxis( 1.f, 0.f );
Vec2f Vec2f::YAxis( 0.f, 1.f );
Vec2i Vec2i::Zero( 0,0 );
Vec2i Vec2i::One( 1,1 );
Vec2i Vec2i::XAxis( 1, 0 );
Vec2i Vec2i::YAxis( 0, 1 );

//..

Vec2i & Vec2i::operator=( const Vec2i & _other )
{
	x = _other.x;
	y = _other.y;

	return (*this);
}

//..

Vec2i Vec2i::operator+( const Vec2i & _other ) const 
{
    return Vec2i( x + _other.x, y + _other.y );
}

//..

Bool Vec2i::operator ==( const Vec2i & _other ) const
{
    return x == _other.x && y == _other.y;
}

//..

Bool Vec2i::operator !=( const Vec2i & _other ) const
{
    return x != _other.x || y != _other.y;
}

//..

Vec2i & Vec2i::operator+=( const Vec2i & _other )
{
    x += _other.x;
    y += _other.y;
    return (*this);
}

//..

Vec2i Vec2i::operator-( const Vec2i & _other ) const 
{
    return Vec2i( x - _other.x, y - _other.y );
}

//..

Vec2i & Vec2i::operator-=( const Vec2i & _other )
{
    x -= _other.x;
    y -= _other.y;
    return (*this);
}

Vec2i & Vec2i::operator*=(const Vec2i & _other)
{
	x *= _other.x;
	y *= _other.y;
	return (*this);
}