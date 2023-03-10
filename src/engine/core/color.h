#ifndef __COLOR_H__
#define __COLOR_H__

#include "math/vector4.h"
#include "math/vector3.h"

class Color
{ 
	DECLARE_SCRIPT_CLASS_FINAL(Color)

public :    
	Color() { r = g = b = a = 1.f; }
	Color(S32 _r, S32 _g, S32 _b)	{ r = ((Float)_r) / 255.f ; g = ((Float)_g) / 255.f; b = ((Float)_b) / 255.f; a = 1.f; }
	Color(S32 _r, S32 _g, S32 _b, S32 _a)	{ r = ((Float)_r) / 255.f ; g = ((Float)_g) / 255.f; b = ((Float)_b) / 255.f; a = ((Float)_a) / 255.f; }
	Color( Float _r, Float _g, Float _b, Float _a ){ r = _r ; g = _g; b = _b; a = _a; }
	Color(U32 _id) :Color(S32(_id & 0xFF), S32((_id >> 8) & 0xFF), S32((_id >> 16) & 0xFF) , S32((_id >> 24) & 0xFF)) {}
	Color(const Color& _other) { vec4 = _other.vec4; }
	~Color() {}

	Color operator-( const Color& _other ) const ;
	Color operator+( const Color& _other ) const ;
	Color operator*( Float _ratio ) const ;
	Color operator*( const Color& _other ) const ;
	Color& operator*=(const Color& _other);
	Color& operator*=(Float _ratio);
	Bool operator==(const Color& _other) const;
	Bool operator!=(const Color& _other) const { return !(*this == _other); }
	Color& operator=(const Color& _other) { vec4 = _other.vec4; return (*this); }
	U32 ComputeId() const;
	
	union
	{
		struct
		{
			union
			{
				struct
				{
					Float r, g, b;
				};
				struct
				{
					Vec3f vec3;
				};
				Float rgb[3];
			};
			Float a;
		};
		struct
		{
			Vec4f vec4;
		};
		Float rgba[4];
	};

	static Color	Red;
	static Color	LightGreen;
	static Color	Green;
	static Color	DarkGreen;
	static Color	Blue;
	static Color	Cyan;
	static Color	LightCyan;
	static Color	Magenta;
	static Color	Yellow;
	static Color	Orange;
	static Color	White;
	static Color	Black;
	static Color	LightGrey;
	static Color	Grey;
	static Color	DarkGrey;

};

#endif