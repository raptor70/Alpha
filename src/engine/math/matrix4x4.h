#ifndef __Mat44_H__
    #define __Mat44_H__

class Quat;

class Mat44
{
public :    
	Mat44():
		xx(0.f), xy(0.f), xz(0.f), xw(0.f),    
		yx(0.f), yy(0.f), yz(0.f), yw(0.f),
		zx(0.f), zy(0.f), zz(0.f), zw(0.f),  
		wx(0.f), wy(0.f), wz(0.f), ww(0.f) { }
	Mat44( 
		Float _xx, Float _xy, Float _xz, Float _xw,
		Float _yx, Float _yy, Float _yz, Float _yw,
		Float _zx, Float _zy, Float _zz, Float _zw,
		Float _wx, Float _wy, Float _wz, Float _ww ):
		xx(_xx), xy(_xy), xz(_xz), xw(_xw),
		yx(_yx), yy(_yy), yz(_yz), yw(_yw),
		zx(_zx), zy(_zy), zz(_zz), zw(_zw), 
		wx(_wx), wy(_wy), wz(_wz), ww(_ww) { }
	Mat44( const Mat44 & _other ): 
		xx(_other.xx), xy(_other.xy), xz(_other.xz), xw(_other.xw),
		yx(_other.yx), yy(_other.yy), yz(_other.yz), yw(_other.yw),
		zx(_other.zx), zy(_other.zy), zz(_other.zz), zw(_other.zw), 
		wx(_other.wx), wy(_other.wy), wz(_other.wz), ww(_other.ww) { }
	Mat44(const Vec3f& _trans, const Quat& _rot,const Vec3f& _scale)
	{
		SetTRS(_trans,_rot,_scale);
	}
	~Mat44() { }

	Mat44 operator*( const Float _scalar ) const;
	Vec4f operator*( const Vec4f& _point ) const;
	Vec3f operator*( const Vec3f& _point ) const;
	Mat44 operator*( const Mat44& _other ) const;
	Mat44 operator+( const Mat44& _other ) const;

	void	SetTRS(const Vec3f& _trans, const Quat& _rot,const Vec3f& _scale);
	void	SetTranslation(const Vec3f& _trans);
	void	SetRotation(const Quat& _rot);
	void	SetScale(const Vec3f& _scale);
	void	AddTranslation(const Vec3f& _trans);
	void	AddRotation(const Quat& _rot);
	void	AddScale(const Vec3f& _scale);

	Vec3f	GetTranslation() const;
	Quat	GetRotation() const;
	Vec3f	GetScale() const;

	void SetOrtho(Float _left, Float _right, Float _bottom, Float _top, Float _near, Float _far);
	void SetPerspective(Float _fovy, Float _ratio, Float _near, Float _far);
	void SetLookAt(const Vec3f& _vCamPos, const Vec3f& _vTargetPos, const Vec3f& _vUpAxis);

	void Transpose();
	void Inverse();

	union
	{
		struct
		{
			Float
				xx,xy,xz,xw, //00 01 02 03
				yx,yy,yz,yw, //10 11 12 13
				zx,zy,zz,zw, //20 21 22 23
				wx,wy,wz,ww; //30 31 32 33	
		};
		Float mat44[4][4];
		Float mat[16];
	};
	
	static Mat44 Zero;
	static Mat44 Identity;
	static Mat44 ToClipSpaceRH01; // clip space from OpenGL LeftHand -1 1  -> Vulkan RightHand 0 1
};

Mat44	LinearInterpolation(const Mat44& _a, const Mat44& _b, Float _ratio);

typedef ArrayOf<Mat44> Mat44Array;

#endif
