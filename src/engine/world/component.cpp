#include "component.h"

BEGIN_SCRIPT_CLASS(EntityComponent)
END_SCRIPT_CLASS

EntityComponent::EntityComponent()
{
	m_pEntity = NULL;
}

//------------------------------

EntityComponent::~EntityComponent()
{
}

//------------------------------

void	EntityComponent::Clone(const EntityComponent* _from)
{
	DEBUG_Require(GetComponentName() == _from->GetComponentName() );
}
