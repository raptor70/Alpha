#include "renderer_pass.h"

RendererSubpassRef	RendererPass::AddSubpass(const Char* _pName)
{
	RendererSubpassRef pSubpass = ResourceManager::GetInstance().GetVirtualResource<RendererSubpass>(Str("%s_%s",GetBaseName().GetStr().GetArray(),_pName), TRUE);
	m_Subpasses.AddLastItem(pSubpass);
	return pSubpass;
}


RendererPass::~RendererPass()
{
	RendererDatas::GetInstance().UnregisterPass(this);
}

RendererSubpass::~RendererSubpass()
{
	RendererDatas::GetInstance().UnregisterSubpass(this);
}