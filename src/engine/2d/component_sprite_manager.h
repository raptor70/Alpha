#ifndef __COMPONENT_SPRITE_MANAGER_H__
#define __COMPONENT_SPRITE_MANAGER_H__

#include "world/component.h"
#include "3d/vertex_buffer.h"
#include "3d/index_buffer.h"
#include "2d/sprite.h"

class EntityComponentSprite;

BEGIN_COMPONENT_CLASS(SpriteManager)
public:
	EntityComponentSpriteManager();
	~EntityComponentSpriteManager();

	virtual Bool IsUpdater() const	{ return FALSE; }
	virtual Bool IsDrawer() const	{ return TRUE; }
	virtual Bool ReceiveEvent()	const { return FALSE; }

	virtual void	Draw(Renderer*	_renderer);

	void	Register(EntityComponentSprite* _sprite);
	void	Unregister(EntityComponentSprite* _sprite);

protected:
	struct SSpritePerMaterial
	{
		MaterialRef	m_pMaterial;
		ArrayOf<EntityComponentSprite*>	m_Components;
		Primitive3DRef	m_Primitive;
	};

	ArrayOf<SSpritePerMaterial>	m_Sprites;
END_COMPONENT_CLASS

#endif
