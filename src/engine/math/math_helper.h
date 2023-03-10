#ifndef __MATH_HELPER_H__
#define __MATH_HELPER_H__

#define GetXY( __VECTOR__ )		(__VECTOR__).x,(__VECTOR__).y
#define GetXYZ( __VECTOR__ )	(__VECTOR__).x,(__VECTOR__).y,(__VECTOR__).z
#define GetXYZW( __VECTOR__ )	(__VECTOR__).x,(__VECTOR__).y,(__VECTOR__).z,(__VECTOR__).w

template<class T>
inline const T& Min(const T& a, const T& b)
{
	if( a > b )
		return b;
	else
		return a;
}

template<class T>
inline const T& Max(const T& a, const T& b)
{
	if( a < b )
		return b;
	else
		return a;
}

template<typename T>
inline T Abs(T _Value)
{
	return (_Value < 0 ? -_Value : _Value);
}

template<typename T> 
inline T Sign( T _Value )
{
	return _Value == 0 ? 0 : ( _Value < 0 ? -1 : 1 );
}
template<> 
inline Float Sign( Float _Value )
{
	return _Value == 0.f ? 0.f : ( _Value < 0.f ? -1.f : 1.f );
}

template<typename T>
inline T Clamp( const T& _value, const T& _min, const T&_max)
{
	if( _value < _min )
		return _min;

	if(_value > _max )
		return _max;

	return _value;
}

template<typename T>
inline T Sqr(const T& _a)
{
	return _a*_a;
}

#define FLOOR(x) floor(x)
#define FLOORINT(x) (S32)(floor(x))
#define ROUND(x) floor((x)+.5f)
#define ROUNDINT(x) (S32)(floor((x)+.5f))

inline Float Random()
{
	return Float(rand()) / Float(RAND_MAX);
}

inline Float Random(Float _min, Float _max)
{
	return Random() * (_max - _min) + _min;
}

inline S32 Random(S32 _min, S32 _max)
{
	return (S32)(ROUND(Random() * (Float(_max - _min + 1) - Float_Eps) + Float(_min) - 0.5f));
}

#endif