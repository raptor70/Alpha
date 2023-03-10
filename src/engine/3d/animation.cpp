#include "animation.h"

CAnimation::CAnimation()
{
	m_fCurTime = 0.f;
	m_fMaxTime = 0.f;
	m_fSpeed = 1.f;
	m_bPlay = FALSE;
	m_bLoop = TRUE;
}

//----------------------------------

CAnimation::~CAnimation()
{
}

//----------------------------------

void CAnimation::AddKey(const Name& _channel,Float _fTime,const Mat44& _transform)
{
	SChannel* pChannel = GetChannel(_channel);
	if( !pChannel )
	{
		m_Channels.AddLastItem(SChannel());
		pChannel = &(m_Channels[m_Channels.GetCount()-1]);
		pChannel->m_Name = _channel;
	}

	pChannel->m_Keys.AddLastItem(SKey(_fTime,_transform));
	m_fMaxTime = Max(_fTime,m_fMaxTime);
}

//----------------------------------

CAnimation::SChannel* CAnimation::GetChannel(const Name& _channel)
{
	for(U32 i=0; i<m_Channels.GetCount(); i++ )
	{
		if( m_Channels[i].m_Name == _channel )
			return &(m_Channels[i]);
	}

	return NULL;
}

//----------------------------------

void	CAnimation::GetValues(ArrayOf<CAnimation::SValue>& _outValues)
{
	for(U32 c=0; c<m_Channels.GetCount(); c++)
	{
		SChannel& channel = m_Channels[c];
		SValue value;
		value.m_Name = channel.m_Name;
		S32 first = -1, second = -1;
		for(U32 k=1;k<channel.m_Keys.GetCount();k++)
		{
			if( m_fCurTime > channel.m_Keys[k-1].m_fTime
				&& m_fCurTime <= channel.m_Keys[k].m_fTime )
			{
				first = k-1;
				second = k;
				break;
			}
		}

		if( first < 0 )
		{
			if( channel.m_Keys[channel.m_Keys.GetCount()-1].m_fTime < m_fCurTime)
			{
				value.m_Value = channel.m_Keys[channel.m_Keys.GetCount()-1].m_Value;
				_outValues.AddLastItem(value);
			}
			else if( channel.m_Keys[0].m_fTime >= m_fCurTime )
			{
				value.m_Value = channel.m_Keys[0].m_Value;
				_outValues.AddLastItem(value);
			}
			continue;
		}

		Float ratio = (m_fCurTime - channel.m_Keys[first].m_fTime)/(channel.m_Keys[second].m_fTime - channel.m_Keys[first].m_fTime);
		value.m_Value = LinearInterpolation(channel.m_Keys[first].m_Value,channel.m_Keys[second].m_Value,ratio);
		_outValues.AddLastItem(value);
	}
}

//----------------------------------

void	CAnimation::Update(Float _dTime)
{
	if( m_bPlay )
	{
		m_fCurTime += m_fSpeed * _dTime;
		if( m_fCurTime < 0.f || m_fCurTime > m_fMaxTime )
		{
			if( m_bLoop )
			{
				while(m_fCurTime > m_fMaxTime)
					m_fCurTime -= m_fMaxTime;
				while(m_fCurTime < 0.f)
					m_fCurTime += m_fMaxTime;
			}
			else
			{
				m_fCurTime = Clamp(m_fCurTime, 0.f, m_fMaxTime);
				Pause();
			}
		}
	}
}

//----------------------------------

void	CAnimation::Play()
{
	m_bPlay = TRUE;
}

//----------------------------------

void	CAnimation::Pause()
{
	m_bPlay = FALSE;
}

//----------------------------------

void	CAnimation::Stop()
{
	m_bPlay = FALSE;
	m_fCurTime = 0.f;
}

//----------------------------------
