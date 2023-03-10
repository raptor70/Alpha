#include "framebuffer.h"

#include "renderer/renderer.h"

FrameBuffer::FrameBuffer()
{
}

//----------------------------------

FrameBuffer::~FrameBuffer()
{
	RendererDatas::GetInstance().UnregisterFrameBuffer(this);
}

//----------------------------------

void	FrameBuffer::AddTexture(const TextureRef& _texture)
{
	DEBUG_Require(_texture.IsValid());
	m_Textures.AddLastItem(_texture);
}

//----------------------------------

TexturePtr FrameBuffer::GetTexture(U32 _idx)
{
	DEBUG_Require(_idx < m_Textures.GetCount() );
	return m_Textures[_idx];
}