#ifndef __Mat22_H__
    #define __Mat22_H__

class Mat22
{
public :    
	Mat22():
		xx(0.f), xy(0.f),    
		yx(0.f), yy(0.f){ }
	Mat22(
		Float _xx, Float _xy,
		Float _yx, Float _yy):
		xx(_xx), xy(_xy),
		yx(_yx), yy(_yy){ }
	Mat22( const Mat22 & _other ):
		xx(_other.xx), xy(_other.xy),
		yx(_other.yx), yy(_other.yy){ }

	~Mat22() { }

	Vec2f operator*(const Vec2f& _point) const;

	union
	{
		struct
		{
			Float
				xx,xy,
				yx,yy;
		};
		Float mat22[2][2];
		Float mat[4];
	};
	
	static Mat22 Identity;
};

#endif
