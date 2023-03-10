#include "quaternion.h"

BEGIN_SCRIPT_CLASS(Quat)
	SCRIPT_STATIC_CONSTRUCTOR2(Float,Vec3f)
	SCRIPT_RESULT_METHOD2(SetFromAxis,Quat,Float,Vec3f)
END_SCRIPT_CLASS

Quat	Quat::operator*(const Quat& _other) const
{
	return Quat(w * _other.x + x * _other.w + y * _other.z - z * _other.y,
				w * _other.y + y * _other.w + z * _other.x - x * _other.z,
				w * _other.z + z * _other.w + x * _other.y - y * _other.x,
				w * _other.w - x * _other.x - y * _other.y - z * _other.z );
}

Vec3f	Quat::operator*(const Vec3f& _p) const
{
	Quat qp(_p.x,_p.y,_p.z,0.f);
	Quat mul = ((*this)*qp*GetInverse());
	return Vec3f(mul.x,mul.y,mul.z);
}

//..

Quat	Quat::operator*(Float _value) const
{
	Float angle;
	Vec3f axis;
	GetAngleAxis(angle,axis);
	angle *= _value;
	Quat result;
	result.SetFromAxis(angle,axis);
	return result;
}

//..

Quat&	Quat::operator*=(const Quat& _other) 
{
	*this = *this * _other;
	return *this;
}

//..

const Quat & Quat::SetFromAxis( Float _angle, const Vec3f _axis )
{
    x = _axis.x;
    y = _axis.y;
    z = _axis.z;
    Float length = _axis.GetLength();

    if (length > MATH_Eps)
    {
        x /= length;
        y /= length;
        z /= length;

        Float omega = 0.5f * _angle;
        length = (Float)sin(omega);

        x = length*x;
        y = length*y;
        z = length*z;
        w = (Float)cos(omega);
    }
    else
    {
        x = y = z = 0.0f;
        w = 1.0f;
    }

    vec4.Normalize();

    return *this;
}

//..

const Quat &	Quat::SetFromVectors(const Vec3f& _a, const Vec3f& _b)
{
	Float anorm = _a.GetLength();
	Float bnorm = _b.GetLength();
	Float abnorm = anorm*bnorm;
	Float dotab = (_a * _b) / abnorm;
	if( dotab > 1.f - Float_Eps  )
	{ 
		*this = Quat::Identity;
	}
	else
	{
		Vec3f axis = _a ^ _b;
		axis.Normalize();
		Float angle = acos(Clamp(dotab,-1.f,1.f));
		SetFromAxis(angle, axis);
	}
	return *this;
}

//..

void	Quat::GetAngleAxis(Float& _angle, Vec3f& _axis) const
{
    Float square_length = x * x + y * y + z * z;
    if ( square_length > Float_Eps * Float_Eps )
    {
        Float scale = 1.0f / sqrt( square_length );
        _axis.x = x * scale;
        _axis.y = y * scale;
        _axis.z = z * scale;
        _angle = 2.0f * acos( w );
		if (_angle > MATH_PI)
			_angle -= MATH_2_PI;
    }
    else
    {
        _axis.x = 0.0f;
        _axis.y = 0.0f;
        _axis.z = 1.0f;
        _angle = 0.0f;
    }
}

//..

const Quat&		Quat::SetFromMatrix(const Mat44& _mat)
{
#if 0
	if( abs(_mat.xx) > abs(_mat.yy) && abs(_mat.xx) > abs(_mat.zz) )
	{
		Float s = 2.f * sqrt(1.f+_mat.xx-_mat.yy-_mat.zz);
		x = .25f * s;
		y = (_mat.yx + _mat.xy ) / s;
		z = (_mat.zx + _mat.xz ) / s;
		w = (_mat.zy - _mat.yz ) / s;
	}
	else if( abs(_mat.yy) > abs(_mat.zz) )
	{
		Float s = 2.f * sqrt(1.f-_mat.xx+_mat.yy-_mat.zz);
		x = (_mat.yx + _mat.xy ) / s;
		y = .25f * s;
		z = (_mat.zy + _mat.yz ) / s;
		w = (_mat.zx - _mat.xz ) / s;
	}
	else
	{
		Float s = 2.f * sqrt(1.f-_mat.xx-_mat.yy+_mat.zz);
		x = (_mat.zx + _mat.xz ) / s;
		y = (_mat.yz + _mat.zy ) / s;
		z = .25f * s;
		w = (_mat.yx - _mat.xy ) / s;
	}
#else
	Float t = 0.f;
	if (_mat.zz < 0)
	{
		if (_mat.xx > _mat.yy)
		{
			t = 1 + _mat.xx - _mat.yy - _mat.zz;
			x = t;
			y = _mat.xy + _mat.yx;
			z = _mat.zx + _mat.xz;
			w = _mat.yz - _mat.zy;
		}
		else
		{
			t = 1 - _mat.xx + _mat.yy - _mat.zz;
			x = _mat.xy + _mat.yx;
			y = t;
			z = _mat.yz + _mat.zy;
			w = _mat.zx - _mat.xz;
		}
	}
	else
	{
		if (_mat.xx < -_mat.yy)
		{
			t = 1 - _mat.xx - _mat.yy + _mat.zz;
			x = _mat.zx + _mat.xz;
			y = _mat.yz + _mat.zy;
			z = t;
			w = _mat.xy - _mat.yx;
		}
		else
		{
			t = 1 + _mat.xx + _mat.yy + _mat.zz;
			x = _mat.yz - _mat.zy;
			y = _mat.zx - _mat.xz;
			z = _mat.xy - _mat.yx;
			w = t;
		}
	}

	t = 0.5f / sqrt(t);
	x *= t;
	y *= t;
	z *= t;
	w *= t;
#endif
	return *this;
}

//..

const Mat44 Quat::GetTranformMatrix() const
{
	Float xx      = x * x;
	Float xy      = x * y;
	Float xz      = x * z;
	Float xw      = x * w;
	Float yy      = y * y;
	Float yz      = y * z;
	Float yw      = y * w;
	Float zz      = z * z;
	Float zw      = z * w;
	return Mat44( 
        1.f-2.f*(yy+zz),	2.f*(xy-zw),		2.f*(xz+yw),		0.f,
		2.f*(xy+zw),		1.f-2.f*(xx+zz),	2.f*(yz-xw),		0.f,
		2.f*(xz-yw),		2.f*(yz+xw),		1.f-2.f*(xx+yy),	0.f,
		0.f,				0.f,				0.f,				1.f
        );
}

//..

Quat Quat::GetInverse() const
{
	return Quat(-x,-y,-z,w);
}

//..

Vec3f		Quat::GetEulerAngles() const
{
	Vec3f eulers = Vec3f::Zero;

	Float ysqr = y*y;

	// x-axis rotation
	Float t0 = 2.f * (w * x + y * z);
	Float t1 = 1.f - 2.f * (x * x + ysqr);
	eulers.x = atan2(t0, t1);

	// y-axis rotation
	Float t2 = +2.f * (w * y - z * x);
	t2 = t2 > 1.f ? 1.f : t2;
	t2 = t2 < -1.f ? -1.f : t2;
	eulers.y = asin(t2);

	// z-axis rotation
	Float t3 = +2.f * (w * z + x * y);
	Float t4 = +1.f - 2.f * (ysqr + z * z);
	eulers.z = atan2(t3, t4);

	return eulers;
}

//..

const Quat&		Quat::SetFromEulerAngles(const Vec3f& _eulers)
{
	Float t0 = cos(_eulers.z * 0.5f);
	Float t1 = sin(_eulers.z * 0.5f);
	Float t2 = cos(_eulers.x * 0.5f);
	Float t3 = sin(_eulers.x * 0.5f);
	Float t4 = cos(_eulers.y * 0.5f);
	Float t5 = sin(_eulers.y * 0.5f);

	w = t0 * t2 * t4 + t1 * t3 * t5;
	x = t0 * t3 * t4 - t1 * t2 * t5;
	y = t0 * t2 * t5 + t1 * t3 * t4;
	z = t1 * t2 * t4 - t0 * t3 * t5;

	return *this;
}

//..

Quat Quat::Identity( 0.f, 0.f, 0.f, 1.f );

Quat LinearInterpolation(const Quat& _a, const Quat& _b, Float _ratio)
{
	// Calculate angle between them.
	Float cosHalfTheta = _a.w * _b.w + _a.x * _b.x + _a.y * _b.y + _a.z * _b.z;
	// if qa=qb or qa=-qb then theta = 0 and we can return qa
	if (abs(cosHalfTheta) >= 1.0)
	{
		return _a;
	}

	// Calculate temporary values.
	Float halfTheta = acos(cosHalfTheta);
	Float sinHalfTheta = sqrt(1.f - cosHalfTheta*cosHalfTheta);
	
	// if theta = 180 degrees then result is not fully defined
	// we could rotate around any axis normal to qa or qb
	Float ratioA,ratioB;
	ratioA = ratioB = .5f;
	if (abs(sinHalfTheta) >= 0.001f)
	{
		ratioA = sin((1 - _ratio) * halfTheta) / sinHalfTheta;
		ratioB = sin(_ratio * halfTheta) / sinHalfTheta; 
	}

	//calculate Quaternion.
	return Quat(
			(_a.x * ratioA + _b.x * ratioB),
			(_a.y * ratioA + _b.y * ratioB),
			(_a.z * ratioA + _b.z * ratioB),
			(_a.w * ratioA + _b.w * ratioB));
}
