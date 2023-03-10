#ifndef __COMPONENT_SPRITE_H__
#define __COMPONENT_SPRITE_H__

#include "world/component.h"
#include "3d/vertex_buffer.h"
#include "3d/index_buffer.h"
#include "2d/sprite.h"

BEGIN_COMPONENT_CLASS(Sprite)
	DECLARE_SCRIPT_CLASS(EntityComponentSprite)
public:
	EntityComponentSprite();
	~EntityComponentSprite();

	virtual Bool IsUpdater() const	{ return TRUE; }
	virtual Bool IsDrawer() const	{ return FALSE; }
	virtual Bool ReceiveEvent()	const { return TRUE; }

	virtual void	Initialize() OVERRIDE;
	virtual void	Finalize() OVERRIDE;
	virtual void	Update(Float _dTime) OVERRIDE;
	virtual void	Draw(Renderer*	_renderer) OVERRIDE;
	virtual void	OnEvent(const EntityEvent& _event) OVERRIDE;

	virtual void	Clone(const EntityComponent* _from) OVERRIDE;

	void	LoadSprite(const Str& _path);

	void	SetSize(Float _x, Float _y)		{ if( GetSprite()) GetSprite()->SetSize(_x,_y); }
const Vec2f& GetSize() const			{ return GetSprite()?GetSprite()->GetSize():Vec2f::Zero; }
	void	SetColor(const Color& _color);

	void	SetStayAtEnd(Bool _stay)	{ m_bStayAtEnd = _stay; }

	void	AddFrame(const Str& _anim,S32 _frame,
		Float _uvx,Float _uvy,
		Float _sizeuvx,Float _sizeuvy,
		Float _offsetuvx,Float _offsetuvy,
		Float _sizerefuvx,Float _sizerefuvy,
		S32 _sizerefx,S32 _sizerefy);
	void	AddAliasFrame(const Str& _targetAnim,S32 _targetFrame,const Str& _newAnim,S32 _newFrame);

	void		PlayAnim(const Name& _anim, Bool _loop);
	void		PlayAnim(S32 _id, Bool _loop);
	void		PlayAnimDelayed(const Name& _anim, Bool _loop, S32 _delay);
	Bool		IsPlayingAnim() const			{ 
		return (m_iCurrentAnim>=0 
			&& m_fProgress < GetSprite()->GetAnimDuration(m_iCurrentAnim)) || m_iDelayedAnim >=0; };
	void		SetAnimTime(Float _progress)	{ m_fProgress = _progress; }
	Float		GetAnimDuration(const Name& _anim);
	const Name&	GetCurrentAnim() const;
	void		RestartAnim();

	const Sprite*	GetSprite()	const	{ return (Sprite*)m_Sprite.GetPtr(); }
	const MaterialRef&	GetMaterial() 			{ if( GetSprite() ) return GetSprite()->GetMaterial(); return m_pDummyMaterial; }

	Bool		MustDraw() const { return (m_iCurrentAnim >= 0) && !GetEntity()->IsHidden(); }
	void		AddGeometry(const Vec3f& _refPos, VertexBufferRef& _vb, U16 _vbi, IndexBufferRef& _ib, S32 _ibi);
protected:
	Sprite*	GetSprite()				{ return (Sprite*)m_Sprite.GetPtr(); }

	ResourceRef		m_Sprite;

	S32			m_iCurrentAnim;
	Float		m_fProgress;
	Bool		m_bLoop;
	S32			m_iDelayedAnim;
	Float		m_fDelayedAnimDelay;
	Bool		m_bDelayedLoop;
	SoundRef	m_pSoundEvent;
	Bool		m_bStayAtEnd;
	MaterialRef	m_pDummyMaterial;

END_COMPONENT_CLASS

#endif
