#include "component_animation.h"

#include "core/sort.h"

BEGIN_SCRIPT_CLASS_INHERITED(EntityComponentAnimation,EntityComponent)
	SCRIPT_VOID_METHOD1(SetDuration,Float)
	SCRIPT_VOID_METHOD1(SetDelayBeforeStart,Float)
	SCRIPT_VOID_METHOD1(SetLoop,Bool)
	SCRIPT_VOID_METHOD(Pause)
	SCRIPT_VOID_METHOD(Play)
	SCRIPT_VOID_METHOD2(AddPosKey,Vec3f,Float)
	SCRIPT_VOID_METHOD2(AddRotKey, Vec3f, Float)
	SCRIPT_VOID_METHOD2(AddScaleKey,Vec3f,Float)
	SCRIPT_VOID_METHOD2(AddColorKey,Color,Float)
END_SCRIPT_CLASS

DEFINE_COMPONENT_CLASS(Animation)
END_DEFINE_COMPONENT

EntityComponentAnimation::EntityComponentAnimation()
{
	m_fProgress = 0.f;
	m_fDelay = 0.f;
	m_fDuration = 1.f;
	m_bLoop = TRUE;
	m_bPaused = FALSE;
}

//------------------------------

EntityComponentAnimation::~EntityComponentAnimation()
{
}

//------------------------------

void	EntityComponentAnimation::Initialize()
{
	if( m_Pos.GetCount() == 0 )
		m_Pos.AddLastItem(Key<Vec3f>(Vec3f::Zero,0.f));

	if( m_Rots.GetCount() == 0 )
		m_Rots.AddLastItem(Key<Quat>(Quat::Identity,0.f));

	if( m_Scales.GetCount() == 0 )
		m_Scales.AddLastItem(Key<Vec3f>(Vec3f::One,0.f));

	if( m_Colors.GetCount() == 0 )
		m_Colors.AddLastItem(Key<Color>(Color::White,0.f));
	
	SortAsc(m_Pos);
	SortAsc(m_Rots);
	SortAsc(m_Scales);
	SortAsc(m_Colors);

	m_fProgress = 0.f;
}

//------------------------------

void	EntityComponentAnimation::Update(Float _dTime)
{
	if( !m_bPaused )
	{
		if( m_fDelay > 0.f )
		{
			m_fDelay -= _dTime;
		}
		else
		{
			m_fProgress += _dTime;
			if( m_fProgress > m_fDuration )
			{
				if( m_bLoop )
					m_fProgress -= m_fDuration;
				else
				{
					Pause();
					m_fProgress = m_fDuration;
				}
			}
		}
	}

	Float ratio = m_fProgress / m_fDuration;
	GetEntity()->SetPos(GetPos(ratio));
	GetEntity()->SetRot(GetRot(ratio));
	GetEntity()->SetScale(GetScale(ratio));
	GetEntity()->SetColor(GetColor(ratio));
	
}

//------------------------------

Vec3f	EntityComponentAnimation::GetPos(Float _progress)
{
	DEBUG_Require(_progress>=0.f && _progress<= 1.f);

	if( _progress <= m_Pos[0].m_Ratio )
		return m_Pos[0].m_Value;
	else if( _progress >= m_Pos[m_Pos.GetCount()-1].m_Ratio )
		return m_Pos[m_Pos.GetCount()-1].m_Value;
	else
	{
		for(U32 i=0;i<m_Pos.GetCount()-1;i++)
		{
			if( _progress <= m_Pos[i+1].m_Ratio )
			{
				Vec3f v1 = m_Pos[i].m_Value;
				Float r1 = m_Pos[i].m_Ratio;
				Vec3f v2 = m_Pos[i+1].m_Value;
				Float r2 = m_Pos[i+1].m_Ratio;
				return LinearInterpolation(v1,v2,(_progress - r1 ) / ( r2 - r1 ) );
			}
		}
	}

	return m_Pos[0].m_Value;
}

//------------------------------

Quat EntityComponentAnimation::GetRot(Float _progress)
{
	DEBUG_Require(_progress>=0.f && _progress<= 1.f);

	if( _progress <= m_Rots[0].m_Ratio )
		return m_Rots[0].m_Value;
	else if( _progress >= m_Rots[m_Rots.GetCount()-1].m_Ratio )
		return m_Rots[m_Rots.GetCount()-1].m_Value;
	else
	{
		for(U32 i=0;i<m_Rots.GetCount()-1;i++)
		{
			if( _progress <= m_Rots[i+1].m_Ratio )
			{
				Quat v1 = m_Rots[i].m_Value;
				Float r1 = m_Rots[i].m_Ratio;
				Quat v2 = m_Rots[i+1].m_Value;
				Float r2 = m_Rots[i+1].m_Ratio;
				return LinearInterpolation(v1,v2,(_progress - r1 ) / ( r2 - r1 ) );
			}
		}
	}

	return m_Rots[0].m_Value;
}

//------------------------------

Vec3f	EntityComponentAnimation::GetScale(Float _progress)
{
	DEBUG_Require(_progress>=0.f && _progress<= 1.f);

	if( _progress <= m_Scales[0].m_Ratio )
		return m_Scales[0].m_Value;
	else if( _progress >= m_Scales[m_Scales.GetCount()-1].m_Ratio )
		return m_Scales[m_Scales.GetCount()-1].m_Value;
	else
	{
		for(U32 i=0;i<m_Scales.GetCount()-1;i++)
		{
			if( _progress <= m_Scales[i+1].m_Ratio )
			{
				Vec3f v1 = m_Scales[i].m_Value;
				Float r1 = m_Scales[i].m_Ratio;
				Vec3f v2 = m_Scales[i+1].m_Value;
				Float r2 = m_Scales[i+1].m_Ratio;
				return LinearInterpolation(v1,v2,(_progress - r1 ) / ( r2 - r1 ) );
			}
		}
	}

	return m_Scales[0].m_Value;
}

//------------------------------

Color	EntityComponentAnimation::GetColor(Float _progress)
{
	DEBUG_Require(_progress>=0.f && _progress<= 1.f);

	if( _progress <= m_Colors[0].m_Ratio )
		return m_Colors[0].m_Value;
	else if( _progress >= m_Colors[m_Colors.GetCount()-1].m_Ratio )
		return m_Colors[m_Colors.GetCount()-1].m_Value;
	else
	{
		for(U32 i=0;i<m_Colors.GetCount()-1;i++)
		{
			if( _progress <= m_Colors[i+1].m_Ratio )
			{
				Color v1 = m_Colors[i].m_Value;
				Float r1 = m_Colors[i].m_Ratio;
				Color v2 = m_Colors[i+1].m_Value;
				Float r2 = m_Colors[i+1].m_Ratio;
				return LinearInterpolation(v1,v2,(_progress - r1 ) / ( r2 - r1 ) );
			}
		}
	}

	return m_Colors[0].m_Value;
}