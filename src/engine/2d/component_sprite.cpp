#include "component_sprite.h"

#include "component_sprite_manager.h"
#include "core/name_static.h"

BEGIN_SCRIPT_CLASS_INHERITED(EntityComponentSprite,EntityComponent)
	SCRIPT_VOID_METHOD1(LoadSprite,Str)
	SCRIPT_VOID_METHOD2(SetSize,Float,Float)
	SCRIPT_VOID_METHOD1(SetStayAtEnd,Bool)
	SCRIPT_VOID_METHOD2(PlayAnim,Str,Bool)
	SCRIPT_VOID_METHOD3(PlayAnimDelayed,Str,Bool,S32)
END_SCRIPT_CLASS

DEFINE_COMPONENT_CLASS(Sprite)
END_DEFINE_COMPONENT

EntityComponentSprite::EntityComponentSprite()
{
	m_fProgress = 0.f;
	m_iCurrentAnim = -1;
	m_bLoop = FALSE;

	m_iDelayedAnim = -1;
	m_fDelayedAnimDelay = 0.f;
	m_bDelayedLoop = FALSE;
	m_bStayAtEnd = FALSE;
}

//------------------------------

EntityComponentSprite::~EntityComponentSprite()
{
	if( m_pSoundEvent.IsValid() )
	{
		if( m_bLoop )
			m_pSoundEvent->Stop();
		m_pSoundEvent.SetNull();
	}
}

//------------------------------

void	EntityComponentSprite::Initialize()
{
	SUPER::Initialize();
	World::GetInstance().GetComponent<EntityComponentSpriteManager>()->Register(this);
}

//------------------------------

void	EntityComponentSprite::Finalize()
{
	SUPER::Finalize();
	EntityComponentSpriteManager* manager = World::GetInstance().GetComponent<EntityComponentSpriteManager>();
	manager->Unregister(this);
}
	
//------------------------------

void	EntityComponentSprite::Update(Float _dTime)
{
	PROFILER_Begin("COMP_Sprite");
	SUPER::Update(_dTime);

	// update delayed
	if( m_iDelayedAnim >= 0 )
	{
		m_fDelayedAnimDelay -= _dTime;
		if( m_fDelayedAnimDelay <= 0.f )
		{
			PlayAnim(m_iDelayedAnim,m_bDelayedLoop);
			m_iDelayedAnim = -1;
		}
	}

	if( m_pSoundEvent.IsValid() && !m_pSoundEvent->IsPlaying() )
	{
		m_pSoundEvent.SetNull();
	}

	if( m_iCurrentAnim >= 0 )
	{
		Float curAnimDuration = GetSprite()->GetAnimDuration(m_iCurrentAnim);
		if( m_fProgress <= curAnimDuration )
		{
			m_fProgress += _dTime;
			if( m_fProgress >= curAnimDuration )
			{
				if( m_bLoop )
				{
					do
					{
						m_fProgress -= curAnimDuration;
					}
					while( m_fProgress >= curAnimDuration);
				}
				else
				{
					m_fProgress = curAnimDuration;
					if( !m_bStayAtEnd )	
						m_iCurrentAnim = -1;
				}
			}
		}
	}
	PROFILER_End();
}

//------------------------------

void	EntityComponentSprite::Draw(Renderer*	_renderer)
{
	SUPER::Draw(_renderer);
}

//------------------------------

void	EntityComponentSprite::OnEvent(const EntityEvent& _event)
{
	SUPER::OnEvent(_event);

	if( _event.m_nName == NAME(ON_PAUSE))
	{
		if( m_pSoundEvent.IsValid() )
			m_pSoundEvent->Pause();
	}
	else if( _event.m_nName == NAME(ON_RESUME))
	{
		if( m_pSoundEvent.IsValid() )
			m_pSoundEvent->Resume();
	}
}

//------------------------------

void	EntityComponentSprite::Clone(const EntityComponent* _from)
{
	SUPER::Clone(_from);

	EntityComponentSprite* from = (EntityComponentSprite*)_from;

	m_Sprite = from->m_Sprite;
	if( from->m_iCurrentAnim >= 0 )
	{
		PlayAnim(from->m_iCurrentAnim,from->m_bLoop);
		m_fProgress = from->m_fProgress;
	}

	m_bStayAtEnd = from->m_bStayAtEnd;
}

//------------------------------

void	EntityComponentSprite::LoadSprite(const Str& _path)
{
	Str luaPath = _path;
	luaPath += ".lua";
	m_Sprite = ResourceManager::GetInstance().GetResourceFromFile<Sprite>(luaPath);
}

//------------------------------

void	EntityComponentSprite::SetColor(const Color& _color)
{
	GetSprite()->GetMaterial()->SetDiffuseColor(_color);
}

//------------------------------

void	EntityComponentSprite::PlayAnim(const Name& _anim, Bool _loop)
{
    if( GetSprite() )
    {
        PlayAnimDelayed(_anim,_loop,0);
    }
}

//------------------------------

void	EntityComponentSprite::PlayAnim(S32 _id, Bool _loop)
{
	if( m_pSoundEvent.IsValid() )
	{
		if( m_bLoop )
			m_pSoundEvent->Stop();
		m_pSoundEvent.SetNull();
	}

	m_iCurrentAnim = _id;
	m_fProgress = 0.f;
	m_bLoop = _loop;

	const Str& event = GetSprite()->GetAnimSoundEvent(_id);;
	if( event.GetLength() > 0 && !GetEntity()->IsHidden() )
	{
		m_pSoundEvent = SoundManager::GetInstance().PlaySound(event);
	}
}

//------------------------------

void	EntityComponentSprite::PlayAnimDelayed(const Name& _anim, Bool _loop, S32 _frameDelay)
{
	S32 a = GetSprite()->GetAnimId(_anim);
	if( a >= 0 )
	{
		m_iDelayedAnim = a;
		m_fDelayedAnimDelay = (Float)_frameDelay / 30.f;
		m_bDelayedLoop = _loop;
	}
}

//------------------------------

void	EntityComponentSprite::RestartAnim()
{
	if( m_iCurrentAnim >= 0 )
		PlayAnim(m_iCurrentAnim,m_bLoop);
}

//------------------------------

Float	EntityComponentSprite::GetAnimDuration(const Name& _anim)
{
	return GetSprite()->GetAnimDuration(_anim);
}

//------------------------------

const Name&	EntityComponentSprite::GetCurrentAnim() const
{
	if( m_iCurrentAnim >= 0 )
		return GetSprite()->GetAnimName(m_iCurrentAnim);

	return Name::Null;
}

//------------------------------

void EntityComponentSprite::AddGeometry(const Vec3f& _refPos, VertexBufferRef& _vb, U16 _vbi, IndexBufferRef& _ib, S32 _ibi)
{
	S32 frameidx = (S32)(m_fProgress * 30.f - Float_Eps);
	Vec3f pos = GetEntity()->GetWorldPos() - _refPos;
	Quat rot = GetEntity()->GetWorldRot();
	GetSprite()->GetFrame(m_iCurrentAnim,frameidx,pos,rot,_vb,_vbi,_ib,_ibi);
}
