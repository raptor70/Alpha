#include "matrix3x3.h"
#include "math_constant.h"

Vec3f Mat33::operator*( const Vec3f& _point ) const
{
    return Vec3f(
        _point.x * xx + _point.y * xy + _point.z * xz,
        _point.x * yx + _point.y * yy + _point.z * yz,
        _point.x * zx + _point.y * zy + _point.z * zz
        );
}

//..

void Mat33::Transpose()
{
	Swap(xy,yx);
	Swap(xz,zx);
	Swap(yz,zy);
}

//..

void Mat33::Inverse()
{
	// http://www.cg.info.hiroshima-cu.ac.jp/~miyazaki/knowledge/teche23.html
	
	Float det = xx*yy*zz + yx*zy*xz + zx*xy*yz
			- xx*zy*yz - zx*yy*xz - yx*xy*zz;
	if( det != 0.f )
	{
		det = 1.f / det;
		Mat44 result;
		result.xx = det * ( yy*zz - yz*zy ); 
		result.xy = det * ( xz*zy - xy*zz );
		result.xz = det * ( xy*yz - xz*yy );
		result.yx = det * ( yz*zx - yx*zz );
		result.yy = det * ( xx*zz - xz*zx );
		result.yz = det * ( xz*yx - xx*yz );
		result.zx = det * ( yx*zy - yy*zx );
		result.zy = det * ( xy*zx - xx*zy );
		result.zz = det * ( xx*yy - xy*yx );
		*this = result;
	}
}

//..

Mat33 Mat33::Identity(	1.f, 0.f, 0.f,
						0.f, 1.f, 0.f,
						0.f, 0.f, 1.f );