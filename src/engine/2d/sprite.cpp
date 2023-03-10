#include "sprite.h"

BEGIN_SCRIPT_CLASS(Sprite)
	SCRIPT_VOID_METHOD12(AddFrame,Str,U32,Float,Float,Float,Float,Float,Float,Float,Float,S32,S32)
	SCRIPT_VOID_METHOD4(AddAliasFrame,Str,U32,Str,U32)
	SCRIPT_VOID_METHOD2(AddSoundEvent,Str,Str)

END_SCRIPT_CLASS

Sprite::Sprite()
{
	m_Material = RendererDatas::GetInstance().CreateMaterial();
	m_Size = Vec2f(1.f,1.f);
}
	
//------------------------------

Sprite::~Sprite()
{
}

//------------------------------

Bool	Sprite::GetFrame(S32 _animId,S32 _frame, VertexBufferRef& _vb, IndexBufferRef& _ib) const
{
	_vb->SetVertexCount(4);
	_ib->m_Array.SetItemCount(6);
	return GetFrame(_animId,_frame,Vec3f::Zero,Quat::Identity,_vb,0,_ib,0);
}

//------------------------------

Bool	Sprite::GetFrame(S32 _animId,S32 _frame, const Vec3f& _refPos, const Quat& _refRot, VertexBufferRef& _vb, U16 _vbi, IndexBufferRef& _ib, S32 _ibi) const
{
	if( _animId < 0 )
		return FALSE;

	if( _frame < 0 )
		return FALSE;

	const SpriteAnim& anim = m_Anims[_animId];
	const SpriteFrame& frame = anim.m_Frames[_frame];
	Vec2f startUv = frame.m_UVPos;
	Vec2f endUv = startUv + frame.m_UVSize;

	Vec2f pos(0.f,0.f);
	pos -= Vec2f(frame.m_UVSizeRef.x,frame.m_UVSizeRef.y) * .5f;
	pos += Vec2f(frame.m_UVOffset.x,frame.m_UVOffset.y);
	pos += Vec2f(frame.m_UVSize.x,-frame.m_UVSize.y) * .5f;
		

	Vec2f size;
	size.x = ((Float)m_Size.x) * frame.m_UVSize.x * (Float)m_Material->GetTexture()->GetRealSizeX();
	size.y = ((Float)m_Size.y) * frame.m_UVSize.y * (Float)m_Material->GetTexture()->GetRealSizeY();

	pos.x *= ((Float)m_Size.x) * (Float)m_Material->GetTexture()->GetRealSizeX();
	pos.y *= ((Float)m_Size.y) * (Float)m_Material->GetTexture()->GetRealSizeY();

	//Vec2f size = frame.m_UVSize;
	//size.x *= ((Float)m_Size.x) * ratio.x;
	//size.y *= ((Float)m_Size.y) * ratio.y;

	Float halfScaleX = size.x *.5f;
	Float halfScaleY = -size.y *.5f;
	
	_vb->SetVertexAtIndex(_vbi,_refPos+_refRot*Vec3f(pos.x-halfScaleX,pos.y-halfScaleY,0.f),Vec2f(startUv.x,startUv.y),Vec3f::ZAxis);
	_vb->SetVertexAtIndex(_vbi+1,_refPos+_refRot*Vec3f(pos.x-halfScaleX,pos.y+halfScaleY,0.f),Vec2f(startUv.x,endUv.y),Vec3f::ZAxis);
	_vb->SetVertexAtIndex(_vbi+2,_refPos+_refRot*Vec3f(pos.x+halfScaleX,pos.y-halfScaleY,0.f),Vec2f(endUv.x,startUv.y),Vec3f::ZAxis);
	_vb->SetVertexAtIndex(_vbi+3,_refPos+_refRot*Vec3f(pos.x+halfScaleX,pos.y+halfScaleY,0.f),Vec2f(endUv.x,endUv.y),Vec3f::ZAxis);

	_ib->m_Array[_ibi] = _vbi;
	_ib->m_Array[_ibi+1] = _vbi+1;
	_ib->m_Array[_ibi+2] = _vbi+2;
	_ib->m_Array[_ibi+3] = _vbi+1;
	_ib->m_Array[_ibi+4] = _vbi+2;
	_ib->m_Array[_ibi+5] = _vbi+3;

	return TRUE;
}

//------------------------------

Bool	Sprite::GetFrameUVs(S32 _animId,S32 _frame, Vec3f& _pos, Vec3f& _size, Vec2f& _uvmin, Vec2f& _uvmax) const
{
	if( _animId < 0 )
		return FALSE;

	if( _frame < 0 )
		return FALSE;

	const SpriteAnim& anim = m_Anims[_animId];
	const SpriteFrame& frame = anim.m_Frames[_frame];
	_uvmin.x = frame.m_UVPos.x;
	_uvmin.y = frame.m_UVPos.y;
	_uvmax.x = _uvmin.x + frame.m_UVSize.x;
	_uvmax.y = _uvmin.y + frame.m_UVSize.y;

	Vec2f pos(0.f,0.f);
	pos -= Vec2f(frame.m_UVSizeRef.x,frame.m_UVSizeRef.y) * .5f;
	pos += Vec2f(frame.m_UVOffset.x,frame.m_UVOffset.y);
	pos += Vec2f(frame.m_UVSize.x,-frame.m_UVSize.y) * .5f;
	pos.x /= frame.m_UVSizeRef.x;
	pos.y /= frame.m_UVSizeRef.y;

	Vec2f size;
	size.x = frame.m_UVSize.x;
	size.y = frame.m_UVSize.y;
	size.x /= frame.m_UVSizeRef.x;
	size.y /= frame.m_UVSizeRef.y;

	_pos.x = pos.x;
	_pos.y = pos.y;
	_size.x = size.x;
	_size.y = -size.y;

	return TRUE;
}

//------------------------------

void	Sprite::AddFrame(const Str& _anim,U32 _frame,
	Float _uvx,Float _uvy,
	Float _sizeuvx,Float _sizeuvy,
	Float _offsetuvx,Float _offsetuvy,
	Float _sizerefuvx,Float _sizerefuvy,
	S32 _sizerefx,S32 _sizerefy)
{
	Name animName = _anim;

	// find anim 
	SpriteAnim* pAnim = NULL;
	for(U32 a=0; a<m_Anims.GetCount(); a++ )
	{
		if( m_Anims[a].m_Name == animName )
		{
			pAnim = &(m_Anims[a]);
			break;
		}
	}

	// if not, create it !
	if( !pAnim )
	{
		pAnim = &(m_Anims.AddLastItem(SpriteAnim()));
		pAnim->m_Name = animName;
	}

	// resize frame array
	if( pAnim->m_Frames.GetCount() < _frame+1 )
	{
		pAnim->m_Frames.SetItemCount(_frame+1);
		pAnim->m_fDuration = ((Float)_frame+1)/30.f;
	}

	// set new frame
	SpriteFrame& frame = pAnim->m_Frames[_frame];
	Float uvfactorx = m_Material->GetTexture()->GetUVMaxX();
	Float uvfactory = m_Material->GetTexture()->GetUVMaxY();
	frame.m_UVPos = Vec2f(_uvx*uvfactorx,(1.f - _uvy)*uvfactory);
	frame.m_UVSize = Vec2f(_sizeuvx*uvfactorx,-_sizeuvy*uvfactory);
	frame.m_UVOffset = Vec2f(_offsetuvx*uvfactorx,_offsetuvy*uvfactory);
	frame.m_UVSizeRef = Vec2f(_sizerefuvx*uvfactorx,_sizerefuvy*uvfactory);
	frame.m_SizeRef = Vec2i(_sizerefx,_sizerefy);
}

//------------------------------

void	Sprite::AddAliasFrame(const Str& _targetAnim,U32 _targetFrame,const Str& _newAnim,U32 _newFrame)
{
	Name animName = _newAnim;
	Name targetAnimName = _targetAnim;

	// find anim 
	SpriteAnim* pAnim = NULL;
	SpriteAnim* pTargetAnim = NULL;
	for(U32 a=0; a<m_Anims.GetCount(); a++ )
	{
		if( m_Anims[a].m_Name == animName )
		{
			pAnim = &(m_Anims[a]);
		}
	}

	// if not, create it !
	if( !pAnim )
	{
		pAnim = &(m_Anims.AddLastItem(SpriteAnim()));
		pAnim->m_Name = animName;
	}

	for(U32 a=0; a<m_Anims.GetCount(); a++ )
	{
		if( m_Anims[a].m_Name == targetAnimName )
		{
			pTargetAnim = &(m_Anims[a]);
		}
	}

	DEBUG_RequireMessage(pTargetAnim && pTargetAnim->m_Frames.GetCount() > _targetFrame, "Bad sprite export !!");

	// resize frame array
	if( pAnim->m_Frames.GetCount() < _newFrame+1 )
	{
		pAnim->m_Frames.SetItemCount(_newFrame+1);
		pAnim->m_fDuration = ((Float)_newFrame+1)/30.f;
	}

	pAnim->m_Frames[_newFrame] = pTargetAnim->m_Frames[_targetFrame];
}

//------------------------------

void	Sprite::AddSoundEvent(const Str& _targetAnim,const Str& _event)
{
	S32 id = GetAnimId(_targetAnim);
	if( id < 0 )
	{
		LOGGER_LogError("AddSoundEvent : Unknown anim %s for sound event %s\n",_targetAnim.GetArray(),_event.GetArray());
		return;
	}

	m_Anims[id].m_sSoundEvent = _event;
}

//------------------------------

S32		Sprite::GetAnimId(const Name& _anim) const
{
	for(U32 a=0; a<m_Anims.GetCount(); a++ )
	{
		if( m_Anims[a].m_Name == _anim )
		{
			return a;
		}
	}

	return -1;
}
