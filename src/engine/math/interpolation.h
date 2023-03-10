#ifndef __INTERPOLATION_H__
#define __INTERPOLATION_H__

template<typename T>
T	LinearInterpolation(const T& _a, const T& _b, Float _ratio)
{
	return (T)(_a + ( _b - _a) * _ratio);
}

inline void	LinearSmooth(Float _origin,Float _dest, Float _speed, Float _dTime, Float& _result)
{	
	DEBUG_RequireMessage(_speed > 0.f,"Bad Smooth speed in LinearSmooth");
    if (abs(_dest-_origin)<Float_Eps)
    {
        _result=_dest;
        return;
    }

    if (_origin>_dest)
    {
        _result = _origin - _speed*_dTime;
        if (_result<_dest)
            _result=_dest;
    }
    else
    {
        _result = _origin + _speed*_dTime;
        if (_result>_dest)
            _result=_dest;
    }
}

inline void	LinearSmooth(const Vec3f& _origin,const Vec3f& _dest, Float _speed, Float _dTime, Vec3f& _result)
{	
	DEBUG_RequireMessage(_speed > 0.f,"Bad Smooth speed in LinearSmooth");
	if ((_origin-_dest).GetLength()<Float_Eps)
    {
        _result = _dest;
        return;
    }

	Vec3f delta = _dest-_origin;
	Float deltaNorm = delta.GetLength();
	Float newDelta = _speed * _dTime;
	
	if( newDelta > deltaNorm || deltaNorm < Float_Eps)
		_result = _dest;
	else
		_result = _origin +  delta * (newDelta / deltaNorm);
}


inline void	Smooth(Float _origin, Float _dest, Float _factor, Float _dTime, Float& _result)
{
	DEBUG_RequireMessage(_factor > 0.f, "Bad Smooth speed in LinearSmooth");
	if (abs(_origin - _dest)<Float_Eps)
	{
		_result = _dest;
		return;
	}

	Float delta = _dest - _origin;
	Float newDelta = delta * _factor * _dTime;

	if (abs(newDelta) > abs(delta) || abs(delta) < Float_Eps)
		_result = _dest;
	else
		_result = _origin + newDelta;
}

inline void	Smooth(const Vec3f& _origin, const Vec3f& _dest, Float _factor, Float _dTime, Vec3f& _result)
{
	DEBUG_RequireMessage(_factor > 0.f, "Bad Smooth speed in LinearSmooth");
	if ((_origin - _dest).GetLength()<Float_Eps)
	{
		_result = _dest;
		return;
	}

	Vec3f delta = _dest - _origin;
	Float deltaNorm = delta.GetLength();
	Float newDelta = deltaNorm * _factor * _dTime;

	if (newDelta > deltaNorm || deltaNorm < Float_Eps)
		_result = _dest;
	else
		_result = _origin + delta * (newDelta / deltaNorm);
}

#endif