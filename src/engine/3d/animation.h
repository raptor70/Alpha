#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "resource/resource.h"

BEGIN_SUPER_CLASS(CAnimation,Resource)
protected:
	struct SKey
	{
		Float	m_fTime;
		Mat44	m_Value;

		SKey()
		{
			m_fTime = -1.f;
			m_Value = Mat44::Identity;
		}
		SKey(Float _fTime, const Mat44& _transform)
		{
			m_fTime = _fTime;
			m_Value = _transform;
		}
	};
	struct SChannel
	{
		Name			m_Name;
		ArrayOf<SKey>	m_Keys;

		SChannel()
		{
			m_Name = Name::Null;
		}
	};

public:
	CAnimation();
	virtual ~CAnimation();

	static  const Char* GetResourcePrefix() { return "animation"; }

	void		AddKey(const Name& _channel,Float _fTime,const Mat44& _transform);
	SChannel*	GetChannel(const Name& _channel);

	struct SValue
	{
		Name	m_Name;
		Mat44	m_Value;
	};
	void	GetValues(ArrayOf<SValue>& _outValues);
	Float	GetCurTime() const { return m_fCurTime; }

	void	Update(Float _dTime);
	void	Play();
	void	Pause();
	void	Stop();

protected:
	ArrayOf<SChannel>	m_Channels;
	Float		m_fCurTime;
	Float		m_fMaxTime;
	Float		m_fSpeed;
	Bool		m_bPlay;
	Bool		m_bLoop;
END_SUPER_CLASS

#endif