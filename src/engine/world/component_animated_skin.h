#ifndef __COMPONENT_ANIMATED_SKIN_H__
#define __COMPONENT_ANIMATED_SKIN_H__

#include "component.h"

BEGIN_COMPONENT_CLASS(AnimatedSkin)
	DECLARE_SCRIPT_CLASS(EntityComponentAnimatedSkin)
public:
	EntityComponentAnimatedSkin();
	~EntityComponentAnimatedSkin();

	virtual Bool IsUpdater() const	{ return TRUE; }
	virtual Bool IsDrawer()	const	{ return TRUE; }
	virtual Bool ReceiveEvent()	const { return FALSE; }

	virtual void	Update(Float _dTime);
	virtual void	Draw(Renderer*	_renderer);

	void	LoadFromFile(const Str& _path);
	void	AddAnimation(const Str& _name, const Str& _path);

protected:
	struct Animation
	{
		Name			m_Name;
		ResourceRef		m_Resource;
	};
	Animation*	GetAnimation(const Name& _name);

	ResourceRef			m_Skin;

	ArrayOf<Animation>	m_Animations;

END_COMPONENT_CLASS

#endif
