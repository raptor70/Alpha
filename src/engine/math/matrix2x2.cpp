#include "matrix2x2.h"
#include "math_constant.h"

Vec2f Mat22::operator*( const Vec2f& _point ) const
{
    return Vec2f(
        _point.x * xx + _point.y * xy,
        _point.x * yx + _point.y * yy
        );
}

//..

Mat22 Mat22::Identity(	1.f, 0.f,
						0.f, 1.f);