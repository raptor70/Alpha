#ifndef __INDEX_BUFFER_H__
#define __INDEX_BUFFER_H__

BEGIN_SUPER_CLASS(IndexBuffer, Resource)
public:
	static  const Char* GetResourcePrefix() { return "ib"; }

	ArrayOf<unsigned short>	m_Array;
END_SUPER_CLASS

typedef RefTo<IndexBuffer> IndexBufferRef;

#endif
