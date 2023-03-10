#include "color.h"

BEGIN_SCRIPT_CLASS(Color)
	SCRIPT_STATIC_CONSTRUCTOR3(S32,S32,S32)
	SCRIPT_STATIC_CONSTRUCTOR4(S32,S32,S32,S32)
END_SCRIPT_CLASS

Color	Color::Red			( 1.0f, 0.0f, 0.0f, 1.0f );
Color	Color::LightGreen	( 0.5f, 1.0f, 0.5f, 1.0f );
Color	Color::Green		( 0.0f, 1.0f, 0.0f, 1.0f );
Color	Color::DarkGreen	( 0.0f, 0.5f, 0.0f, 1.0f );
Color	Color::Blue			( 0.0f, 0.0f, 1.0f, 1.0f );
Color	Color::LightCyan	(0.75f, 1.0f, 1.0f, 1.0f);
Color	Color::Cyan			( 0.0f, 1.0f, 1.0f, 1.0f );
Color	Color::Magenta		( 1.0f, 0.0f, 1.0f, 1.0f );
Color	Color::Yellow		( 1.0f, 1.0f, 0.0f, 1.0f );
Color	Color::Orange		( 1.0f, 0.5f, 0.0f, 1.0f );
Color	Color::White		( 1.0f, 1.0f, 1.0f, 1.0f );
Color	Color::Black		( 0.0f, 0.0f, 0.0f, 1.0f );
Color	Color::LightGrey	( 0.75f, 0.75f, 0.75f, 1.0f );
Color	Color::Grey			( 0.5f, 0.5f, 0.5f, 1.0f );
Color	Color::DarkGrey		( 0.25f, 0.25f, 0.25f, 1.0f );

Color Color::operator-( const Color& _other ) const
{
	return Color(r - _other.r, g - _other.g, b - _other.b, a - _other.a );
}

Color Color::operator+( const Color& _other ) const
{
	return Color(r + _other.r, g + _other.g, b + _other.b, a + _other.a );
}

Color Color::operator*( Float _ratio ) const
{
	return Color(r * _ratio, g * _ratio, b * _ratio, a * _ratio );
}

Color Color::operator*( const Color& _other ) const
{
	return Color(r * _other.r, g * _other.g, b * _other.b, a * _other.a);
}

Color& Color::operator*=(const Color& _other)
{
	r *= _other.r;
	g *= _other.g;
	b *= _other.b;
	a *= _other.a;
	return (*this);
}

Color& Color::operator*=( Float _ratio )
{
	r *= _ratio;
	g *= _ratio;
	b *= _ratio;
	a *= _ratio;
	return (*this);
}

Bool Color::operator==(const Color& _other) const
{
	return r == _other.r && g == _other.g && b == _other.b && a == _other.a;
}

U32 Color::ComputeId() const
{
	return (U32(r * 255.f))
		+ (U32(g*255.f) << 8)
		+ (U32(b*255.f) << 16)
		+ (U32(a*255.f) << 24);
}