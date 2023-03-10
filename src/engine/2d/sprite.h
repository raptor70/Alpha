#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "resource/resource.h"
#include "3d/vertex_buffer.h"
#include "3d/index_buffer.h"

BEGIN_SUPER_CLASS(Sprite,Resource)
	DECLARE_SCRIPT_CLASS(Sprite)
public:
	Sprite();
	~Sprite();

	static  const Char* GetResourcePrefix() { return "sprite"; }

	void	SetSize(Float _x, Float _y)		{ m_Size.x = _x; m_Size.y = _y; }
	const Vec2f& GetSize() const			{ return m_Size; }

	void	AddFrame(const Str& _anim,U32 _frame,
		Float _uvx,Float _uvy,
		Float _sizeuvx,Float _sizeuvy,
		Float _offsetuvx,Float _offsetuvy,
		Float _sizerefuvx,Float _sizerefuvy,
		S32 _sizerefx,S32 _sizerefy);
	void	AddAliasFrame(const Str& _targetAnim,U32 _targetFrame,const Str& _newAnim,U32 _newFrame);
	void	AddSoundEvent(const Str& _targetAnim,const Str& _event);

	Bool	GetFrame(S32 _animId,S32 _frame, VertexBufferRef& _vb, IndexBufferRef& _ib) const;
	Bool	GetFrame(S32 _animId,S32 _frame, const Vec3f& _refPos, const Quat& _refRot, VertexBufferRef& _vb, U16 _vbi, IndexBufferRef& _ib, S32 _ibi) const;
	Bool	GetFrameUVs(S32 _animId,S32 _frame, Vec3f& _pos, Vec3f& _size, Vec2f& _uvmin, Vec2f& _uvmax) const;
	S32		GetAnimId(const Name& _anim) const;
	Float	GetAnimDuration(const Name& _anim) const	{ return m_Anims[GetAnimId(_anim)].m_fDuration; }
	Float	GetAnimDuration(S32 _animId) const			{ return m_Anims[_animId].m_fDuration; }
	const Name& GetAnimName(S32 _animId) const			{ return m_Anims[_animId].m_Name; }
	const Str& GetAnimSoundEvent(S32 _animId) const		{ return m_Anims[_animId].m_sSoundEvent; }
	S32		GetNbAnim() const							{ return m_Anims.GetCount(); }

	MaterialRef&	GetMaterial() 	{ return m_Material; }

protected:
	MaterialRef	m_Material;
	Vec2f	m_Size;

	struct SpriteFrame
	{
		Vec2f	m_UVPos;
		Vec2f	m_UVSize;
		Vec2f	m_UVOffset;
		Vec2f	m_UVSizeRef;
		Vec2i	m_SizeRef;
	};

	struct SpriteAnim
	{
		Name						m_Name;
		ArrayOf<SpriteFrame>		m_Frames;
		Float						m_fDuration;
		Str							m_sSoundEvent;
	};

	ArrayOf<SpriteAnim>		m_Anims;

END_SUPER_CLASS

#endif
