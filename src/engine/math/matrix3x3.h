#ifndef __Mat33_H__
    #define __Mat33_H__

#include "matrix4x4.h"

class Mat33
{
public :    
	Mat33():
		xx(0.f), xy(0.f), xz(0.f),    
		yx(0.f), yy(0.f), yz(0.f),
		zx(0.f), zy(0.f), zz(0.f) { }
	Mat33( 
		Float _xx, Float _xy, Float _xz,
		Float _yx, Float _yy, Float _yz,
		Float _zx, Float _zy, Float _zz ):
		xx(_xx), xy(_xy), xz(_xz),
		yx(_yx), yy(_yy), yz(_yz),
		zx(_zx), zy(_zy), zz(_zz) { }
	Mat33( const Mat33 & _other ): 
		xx(_other.xx), xy(_other.xy), xz(_other.xz),
		yx(_other.yx), yy(_other.yy), yz(_other.yz),
		zx(_other.zx), zy(_other.zy), zz(_other.zz) { }
	Mat33( const Mat44 & _other ): 
		xx(_other.xx), xy(_other.xy), xz(_other.xz),
		yx(_other.yx), yy(_other.yy), yz(_other.yz),
		zx(_other.zx), zy(_other.zy), zz(_other.zz) { }

	~Mat33() { }

	Vec3f operator*( const Vec3f& _point ) const;

	void Transpose();
	void Inverse();

	union
	{
		struct
		{
			Float
				xx,xy,xz,
				yx,yy,yz,
				zx,zy,zz;
		};
		Float mat33[3][3];
		Float mat[9];
	};
	
	static Mat33 Identity;
};

#endif
