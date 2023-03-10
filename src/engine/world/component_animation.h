#ifndef __COMPONENT_ANIMATION_H__
#define __COMPONENT_ANIMATION_H__

#include "component.h"

BEGIN_COMPONENT_CLASS(Animation)
	DECLARE_SCRIPT_CLASS(EntityComponentAnimation)
public:
	template<typename T>
	struct Key
	{
		T			m_Value;
		Float 		m_Ratio;

		Key()
		{
		}

		Key(const T& _val, Float _ratio)
		{
			m_Value = _val;
			m_Ratio = _ratio;
		}
	};

	EntityComponentAnimation();
	~EntityComponentAnimation();

	virtual Bool IsUpdater() const	{ return TRUE; }
	virtual Bool IsDrawer()	const	{ return FALSE; }
	virtual Bool ReceiveEvent()	const { return FALSE; }

	virtual void	Initialize();
	virtual void	Update(Float _dTime);
	
	void	SetDelayBeforeStart(Float _delay)	{ m_fDelay = _delay; }
	void	SetDuration(Float _duration)	{ m_fDuration = _duration; }
	void	SetLoop(Bool _loop)				{ m_bLoop = _loop; }
	void	AddPosKey(const Vec3f& _pos, Float _ratio)		{ m_Pos.AddLastItem(Key<Vec3f>(_pos,_ratio)); }
	void	AddRotKey(const Vec3f& _rot, Float _ratio)		{ m_Rots.AddLastItem(Key<Quat>(Quat(MATH_DEG_TO_RAD(_rot.x),Vec3f::XAxis)*Quat(MATH_DEG_TO_RAD(_rot.y), Vec3f::YAxis)*Quat(MATH_DEG_TO_RAD(_rot.z), Vec3f::ZAxis),_ratio)); }
	void	AddScaleKey(const Vec3f& _scale, Float _ratio)	{ m_Scales.AddLastItem(Key<Vec3f>(_scale,_ratio)); }
	void	AddColorKey(const Color& _color, Float _ratio)	{ m_Colors.AddLastItem(Key<Color>(_color,_ratio)); }
	
	void	Pause()			{ m_bPaused = TRUE; }
	void	Play()			{ m_bPaused = FALSE; }
	void	Restart()		{ Play(); m_fProgress = 0.f; }

protected:
	Vec3f	GetPos(Float _progress);
	Quat	GetRot(Float _progress);
	Vec3f	GetScale(Float _progress);
	Color	GetColor(Float _progress);

	Float m_fProgress;
	Float m_fDelay;
	Float m_fDuration;
	Float m_bLoop;
	ArrayOf< Key<Vec3f> >		m_Pos;
	ArrayOf< Key<Quat> >		m_Rots;
	ArrayOf< Key<Vec3f> >		m_Scales;
	ArrayOf< Key<Color> >		m_Colors;
	Vec3f	m_vOrigPos;
	Quat	m_qOrigRot;
	Vec3f	m_vOrigScale;
	Bool	m_bPaused;
END_COMPONENT_CLASS

template<typename T>
Bool	operator<(const EntityComponentAnimation::Key<T>& _a,const EntityComponentAnimation::Key<T>& _b) { return _a.m_Ratio < _b.m_Ratio; }

#endif
