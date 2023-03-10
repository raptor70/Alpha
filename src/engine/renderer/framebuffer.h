#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

#include "3d\texture.h"

BEGIN_SUPER_CLASS(FrameBuffer, Resource)
public:
	FrameBuffer();
	virtual ~FrameBuffer();

	static  const Char* GetResourcePrefix() { return "frameBuffer"; }

	void	AddTexture(const TextureRef& _texture);
	TexturePtr GetTexture(U32 _idx = 0);
	U32		GetTextureCount() const { return m_Textures.GetCount(); }

protected:
	TextureRefArray		m_Textures;
END_SUPER_CLASS

typedef RefTo<FrameBuffer> FrameBufferRef;
typedef ArrayOf<FrameBufferRef> FrameBufferRefArray;

#endif