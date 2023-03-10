#include "component_sprite_manager.h"

#include "resource/resource_manager.h"
#include "component_sprite.h"

DEFINE_COMPONENT_CLASS(SpriteManager)
END_DEFINE_COMPONENT

EntityComponentSpriteManager::EntityComponentSpriteManager()
{
}

//------------------------------

EntityComponentSpriteManager::~EntityComponentSpriteManager()
{
}

//------------------------------

void	EntityComponentSpriteManager::Draw(Renderer*	_renderer)
{
	for(U32 i=0; i<m_Sprites.GetCount();i++)
	{
		SSpritePerMaterial* pSprite = &(m_Sprites[i]);
		U16 count = (U16)pSprite->m_Components.GetCount();

		// get component to draw
		ArrayOf<EntityComponentSprite*>	spritesToDraw;
		Vec3f refPos = Vec3f::Zero;
		for(U32 c=0;c<count; c++)
			if( pSprite->m_Components[c]->MustDraw() )
			{
				refPos *= (Float)spritesToDraw.GetCount();
				refPos += pSprite->m_Components[c]->GetEntity()->GetWorldPos();
				spritesToDraw.AddLastItem(pSprite->m_Components[c]);
				refPos /= (Float)spritesToDraw.GetCount();
			}

		U16 curVidx = 0;
		S32 curIidx = 0;
		count = (U16)spritesToDraw.GetCount();
		if( count > 0 )
		{	
			VertexBufferRef vb;
			IndexBufferRef ib;
			pSprite->m_Primitive->Lock(vb,ib);
			vb->SetVertexCount(count * 4);
			ib->m_Array.SetItemCount(count * 6 );
			for(U32 c=0; c<count; c++)
			{
				spritesToDraw[c]->AddGeometry(refPos,vb,curVidx,ib,curIidx);
				curVidx += 4;
				curIidx += 6;
			}
			pSprite->m_Primitive->Unlock();
		
			_renderer->SetActiveMaterial(pSprite->m_pMaterial,GetEntity()->GetColor());
			_renderer->DrawGeometry(Geometry_TrianglesList,refPos,Quat::Identity,pSprite->m_Primitive);
		}
	}
	_renderer->SetActiveMaterial(NULL);
}

//------------------------------

void	EntityComponentSpriteManager::Register(EntityComponentSprite* _sprite)
{
	//find existing material
	SSpritePerMaterial* pSprite = NULL;
	const MaterialRef& pMat = _sprite->GetMaterial();
	for(U32 i=0; i<m_Sprites.GetCount();i++)
	{
		if( m_Sprites[i].m_pMaterial == pMat )
		{
			pSprite = &(m_Sprites[i]);
			break;
		}
	}

	// if not, create it
	if( !pSprite)
	{
		pSprite = &(m_Sprites.AddLastItem(SSpritePerMaterial()));
		pSprite->m_pMaterial = pMat;
		pSprite->m_Primitive = Primitive3D::Create(Str("sprite_%s", _sprite->GetEntity()->GetName().GetStr().GetArray()), FALSE);
	}

	// add it
	DEBUG_Require(pSprite && !pSprite->m_Components.Find(_sprite));
	pSprite->m_Components.AddLastItem(_sprite);
}

//------------------------------

void	EntityComponentSpriteManager::Unregister(EntityComponentSprite* _sprite)
{
	const MaterialRef& pMat = _sprite->GetMaterial();
	for(U32 i=0; i<m_Sprites.GetCount();i++)
	{
		if( m_Sprites[i].m_pMaterial == pMat )
		{
			U32 id = 0;
			if( m_Sprites[i].m_Components.Find(_sprite,id) )
			{
				m_Sprites[i].m_Components.RemoveItems(id,1);
				if( m_Sprites[i].m_Components.GetCount() == 0 )
				{
					m_Sprites.RemoveItems(i,1);
				}
				return;
			}
		}
	}
}