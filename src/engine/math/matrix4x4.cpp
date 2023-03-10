#include "matrix4x4.h"
#include "math_constant.h"

Mat44 Mat44::operator*( const Float _scalar ) const
{
	Mat44 result;
	result.xx = _scalar * xx;
	result.xy = _scalar * xy;
	result.xz = _scalar * xz;
	result.xw = _scalar * xw;
				
	result.yx = _scalar * yx;
	result.yy = _scalar * yy;
	result.yz = _scalar * yz;
	result.yw = _scalar * yw;
				
	result.zx = _scalar * zx;
	result.zy = _scalar * zy;
	result.zz = _scalar * zz;
	result.zw = _scalar * zw;
				
	result.wx = _scalar * wx;
	result.wy = _scalar * wy;
	result.wz = _scalar * wz;
	result.ww = _scalar * ww;
	return result;
}

Vec4f Mat44::operator*( const Vec4f& _point ) const
{
    return Vec4f(
        _point.x * xx + _point.y * xy + _point.z * xz + _point.w * xw,
        _point.x * yx + _point.y * yy + _point.z * yz + _point.w * yw,
        _point.x * zx + _point.y * zy + _point.z * zz + _point.w * zw,
        _point.x * wx + _point.y * wy + _point.z * wz + _point.w * ww
        );
}

//..

Vec3f Mat44::operator*( const Vec3f& _point ) const
{
    Vec4f vector4 = Vec4f( GetXYZ( _point ), 1.0f );
    Vec4f result4 = operator*( vector4 );

    if ( Fabs( result4.w ) > MATH_Eps )
    {
        return Vec3f( GetXYZ( result4 ) ) / result4.w;
    }
    else
    {
		return Vec3f::Zero;
    }
}

//..

Mat44 Mat44::operator*( const Mat44& _other ) const
{
	Mat44 result;
	result.xx = xx * _other.xx + xy * _other.yx + xz * _other.zx + xw * _other.wx;
	result.xy = xx * _other.xy + xy * _other.yy + xz * _other.zy + xw * _other.wy;
	result.xz = xx * _other.xz + xy * _other.yz + xz * _other.zz + xw * _other.wz;
	result.xw = xx * _other.xw + xy * _other.yw + xz * _other.zw + xw * _other.ww;

	result.yx = yx * _other.xx + yy * _other.yx + yz * _other.zx + yw * _other.wx;
	result.yy = yx * _other.xy + yy * _other.yy + yz * _other.zy + yw * _other.wy;
	result.yz = yx * _other.xz + yy * _other.yz + yz * _other.zz + yw * _other.wz;
	result.yw = yx * _other.xw + yy * _other.yw + yz * _other.zw + yw * _other.ww;

	result.zx = zx * _other.xx + zy * _other.yx + zz * _other.zx + zw * _other.wx;
	result.zy = zx * _other.xy + zy * _other.yy + zz * _other.zy + zw * _other.wy;
	result.zz = zx * _other.xz + zy * _other.yz + zz * _other.zz + zw * _other.wz;
	result.zw = zx * _other.xw + zy * _other.yw + zz * _other.zw + zw * _other.ww;

	result.wx = wx * _other.xx + wy * _other.yx + wz * _other.zx + ww * _other.wx;
	result.wy = wx * _other.xy + wy * _other.yy + wz * _other.zy + ww * _other.wy;
	result.wz = wx * _other.xz + wy * _other.yz + wz * _other.zz + ww * _other.wz;
	result.ww = wx * _other.xw + wy * _other.yw + wz * _other.zw + ww * _other.ww;
	return result;
}

//..

Mat44 Mat44::operator+( const Mat44& _other ) const
{
	Mat44 result;
	result.xx = xx + _other.xx;
	result.xy = xy + _other.xy;
	result.xz = xz + _other.xz;
	result.xw = xw + _other.xw;
				   
	result.yx = yx + _other.yx;
	result.yy = yy + _other.yy;
	result.yz = yz + _other.yz;
	result.yw = yw + _other.yw;
				   
	result.zx = zx + _other.zx;
	result.zy = zy + _other.zy;
	result.zz = zz + _other.zz;
	result.zw = zw + _other.zw;
				   
	result.wx = wx + _other.wx;
	result.wy = wy + _other.wy;
	result.wz = wz + _other.wz;
	result.ww = ww + _other.ww;
	return result;
}

//..

void	Mat44::SetTRS(const Vec3f& _trans, const Quat& _rot,const Vec3f& _scale)
{
	SetRotation(_rot);
	AddScale(_scale);
	AddTranslation(_trans);
}

//..

void	Mat44::SetTranslation(const Vec3f& _trans)
{
	*this = Mat44::Identity;
	AddTranslation(_trans);
}

//..

void	Mat44::SetRotation(const Quat& _rot)
{
	*this = _rot.GetTranformMatrix();
}

//..

void	Mat44::SetScale(const Vec3f& _scale)
{
	*this = Mat44::Identity;
	AddScale(_scale);
}

void	Mat44::AddTranslation(const Vec3f& _trans)
{
	xw += _trans.x;
	yw += _trans.y;
	zw += _trans.z;
}

//..

void	Mat44::AddRotation(const Quat& _rot)
{
	*this = (*this) * _rot.GetTranformMatrix();
}

//..

void	Mat44::AddScale(const Vec3f& _scale)
{
	xx *= _scale.x; xy *= _scale.y; xz *= _scale.z;
	yx *= _scale.x; yy *= _scale.y; yz *= _scale.z;
	zx *= _scale.x; zy *= _scale.y; zz *= _scale.z;
}

//..

Vec3f	Mat44::GetTranslation() const
{
	return Vec3f(xw,yw,zw);
}

//..

Quat	Mat44::GetRotation() const
{
	Quat res;
	res.SetFromMatrix(*this);
	return res;
}

//..

Vec3f	Mat44::GetScale() const
{
	Vec3f scale;
	scale.x = Vec3f(xx,yx,zx).GetLength();
	scale.y = Vec3f(xy,yy,zy).GetLength();
	scale.z = Vec3f(xz,yz,zz).GetLength();
	return scale;
}

//..

void Mat44::SetOrtho(Float _left, Float _right, Float _bottom, Float _top, Float _near, Float _far) // clip space OpenGL (Left Hand - -1 to 1)
{
	// http://www.songho.ca/opengl/gl_projectionmatrix.html
	*this = Mat44::Identity;
    xx = 2.f / (_right - _left);
    xw = (_right + _left) / (_left - _right);
    yy  = 2.f / (_top - _bottom);
    yw  = (_top + _bottom) / (_bottom - _top);
    zz = 2.f / (_near - _far);
    zw = (_far + _near) / (_near - _far);
}

//..

void Mat44::SetPerspective(Float _fovy, Float _ratio, Float _near, Float _far) // clip space OpenGL (Left Hand - -1 to 1)
{
	// http://www.songho.ca/opengl/gl_projectionmatrix.html
	*this = Mat44::Zero;
	Float f = 1.f / tan(_fovy / 2.f);
	Float d = (_near - _far);
	xx = f / _ratio;
	yy = f;
	zz = (_far + _near) / d;
	zw = (2.f * _far * _near) / d;
	wz = -1.f;
}

//..

void Mat44::SetLookAt(const Vec3f& _vCamPos, const Vec3f& _vTargetPos, const Vec3f& _vUpAxis)
{
	Vec3f zaxis = (_vCamPos - _vTargetPos); // camera directed to -Z
	zaxis.Normalize();
	Vec3f xaxis = _vUpAxis ^ zaxis;
	xaxis.Normalize();
	Vec3f yaxis = zaxis ^ xaxis;

	xx = xaxis.x;	xy = yaxis.x;	xz = zaxis.x;	xw = 0.f;
	yx = xaxis.y;	yy = yaxis.y;	yz = zaxis.y;	yw = 0.f;
	zx = xaxis.z;	zy = yaxis.z;	zz = zaxis.z;	zw = 0.f;
	wx = -(xaxis * _vCamPos);  
	wy = -(yaxis * _vCamPos);  
	wz = -(zaxis * _vCamPos);
	ww = 1.f;

	Transpose();
}

//..

void Mat44::Transpose()
{
	Swap(xy,yx);
	Swap(xz,zx);
	Swap(xw,wx);
	Swap(yz,zy);
	Swap(yw,wy);
	Swap(zw,wz);
}

//..

void Mat44::Inverse()
{
	// http://www.cg.info.hiroshima-cu.ac.jp/~miyazaki/knowledge/teche23.html
	
	Float det = xx*yy*zz*ww + xx*yz*zw*wy + xx*yw*zy*wz
			+ xy*yx*zw*wz + xy*yz*zx*ww + xy*yw*zz*wx
			+ xz*yx*zy*ww + xz*yy*zw*wx + xz*yw*zx*wy
			+ xw*yx*zz*wy + xw*yy*zx*wz + xw*yz*zy*wx
			- xx*yy*zw*wz - xx*yz*zy*ww - xx*yw*zz*wy
			- xy*yx*zz*ww - xy*yz*zw*wx - xy*yw*zx*wz
			- xz*yx*zw*wy - xz*yy*zx*ww - xz*yw*zy*wx
			- xw*yx*zy*wz - xw*yy*zz*wx - xw*yz*zx*wy;
	if( det != 0.f )
	{
		det = 1.f / det;
		Mat44 result;
		result.xx = det * ( yy*zz*ww + yz*zw*wy + yw*zy*wz - yy*zw*wz - yz*zy*ww - yw*zz*wy ); 
		result.xy = det * ( xy*zw*wz + xz*zy*ww + xw*zz*wy - xy*zz*ww - xz*zw*wy - xw*zy*wz );
		result.xz = det * ( xy*yz*ww + xz*yw*wy + xw*yy*wz - xy*yw*wz - xz*yy*ww - xw*yz*wy );
		result.xw = det * ( xy*yw*zz + xz*yy*zw + xw*yz*zy - xy*yz*zw - xz*yw*zy - xw*yy*zz );
		result.yx = det * ( yx*zw*wz + yz*zx*ww + yw*zz*wx - yx*zz*ww - yz*zw*wx - yw*zx*wz );
		result.yy = det * ( xx*zz*ww + xz*zw*wx + xw*zx*wz - xx*zw*wz - xz*zx*ww - xw*zz*wx );
		result.yz = det * ( xx*yw*wz + xz*yx*ww + xw*yz*wx - xx*yz*ww - xz*yw*wx - xw*yx*wz );
		result.yw = det * ( xx*yz*zw + xz*yw*zx + xw*yx*zz - xx*yw*zz - xz*yx*zw - xw*yz*zx );
		result.zx = det * ( yx*zy*ww + yy*zw*wx + yw*zx*wy - yx*zw*wy - yy*zx*ww - yw*zy*wx );
		result.zy = det * ( xx*zw*wy + xy*zx*ww + xw*zy*wx - xx*zy*ww - xy*zw*wx - xw*zx*wy );
		result.zz = det * ( xx*yy*ww + xy*yw*wx + xw*yx*wy - xx*yw*wy - xy*yx*ww - xw*yy*wx );
		result.zw = det * ( xx*yw*zy + xy*yx*zw + xw*yy*zx - xx*yy*zw - xy*yw*zx - xw*yx*zy );
		result.wx = det * ( yx*zz*wy + yy*zx*wz + yz*zy*wx - yx*zy*wz - yy*zz*wx - yz*zx*wy );
		result.wy = det * ( xx*zy*wz + xy*zz*wx + xz*zx*wy - xx*zz*wy - xy*zx*wz - xz*zy*wx );
		result.wz = det * ( xx*yz*wy + xy*yx*wz + xz*yy*wx - xx*yy*wz - xy*yz*wx - xz*yx*wy );
		result.ww = det * ( xx*yy*zz + xy*yz*zx + xz*yx*zy - xx*yz*zy - xy*yx*zz - xz*yy*zx );
		*this = result;
	}
}

//..

Mat44 Mat44::Zero(	0.f, 0.f, 0.f, 0.f,
					0.f, 0.f, 0.f, 0.f,
					0.f, 0.f, 0.f, 0.f,
					0.f, 0.f, 0.f, 0.f);

//..

Mat44 Mat44::Identity(	1.f, 0.f, 0.f ,0.f,
						0.f, 1.f, 0.f, 0.f,
						0.f, 0.f, 1.f, 0.f,
						0.f, 0.f, 0.f, 1.f );

Mat44 Mat44::ToClipSpaceRH01(	1.f, 0.f, 0.f, 0.f,
								0.f, -1.f, 0.f, 0.f,
								0.f, 0.f, 0.5f, 0.5f,
								0.f, 0.f, 0.f, 1.f);
Mat44	LinearInterpolation(const Mat44& _a, const Mat44& _b, Float _ratio)
{
/*	Vec3f transA = _a.GetTranslation();
	Vec3f transB = _b.GetTranslation();
	Vec3f trans = LinearInterpolation<Vec3f>(transA,transB,_ratio);

	Vec3f scaleA = _a.GetScale();
	Vec3f scaleB = _b.GetScale();
	Vec3f scale = LinearInterpolation(scaleA,scaleB,_ratio);

	Quat rotA = _a.GetRotation();
	Quat rotB = _b.GetRotation();
	Quat rot = LinearInterpolation(rotA,rotB,_ratio);

	return Mat44(trans,rot,scale);*/
	return _a * (1.f - _ratio) + _b * _ratio;
}
