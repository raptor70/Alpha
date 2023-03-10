#ifndef __PRIMITIVE_3D_H__
#define __PRIMITIVE_3D_H__

//--------------------------------------------------

class Primitive3D;
typedef RefTo<Primitive3D>			Primitive3DRef;
typedef PtrTo<Primitive3D>			Primitive3DPtr;
typedef ArrayOf<Primitive3DRef>		Primitive3DRefArray;

//--------------------------------------------------

BEGIN_SUPER_CLASS(Primitive3D,Resource)
public:
	Primitive3D()
	{
	}
	virtual ~Primitive3D();

	virtual	const	VertexBufferRef& GetVB() const = 0;
	virtual	const	IndexBufferRef& GetIB() const = 0;
	virtual void	Lock(VertexBufferRef& _vb, IndexBufferRef& _ib) = 0;
	virtual void	Unlock() = 0;
	virtual void	PrepareDraw(){}
	virtual Bool	IsEmpty() const { return GetVB()->GetVertexCount() == 0; }
	virtual	Bool	IsStatic() const { return TRUE; }

	static Primitive3DRef Create(const Char* _pName, Bool _bIsDynamic);
END_SUPER_CLASS

//--------------------------------------------------

template<U8 _nbBuffer>
BEGIN_SUPER_CLASS(Primitive3DOf,Primitive3D)
public:
	Primitive3DOf()
	{
		m_bLocked = FALSE;
	}

	virtual void Create() OVERRIDE
	{
		ResourceManager& mgr = ResourceManager::GetInstance();
		for (U8 i = 0; i < _nbBuffer; i++)
		{
			Str name("%s_%d", GetBaseName().GetStr().GetArray(), i);
			m_VB[i] = mgr.GetVirtualResource<VertexBuffer>(name.GetArray());
			m_IB[i] = mgr.GetVirtualResource<IndexBuffer>(name.GetArray());
		}
	}

	virtual void	Lock(VertexBufferRef& _vb, IndexBufferRef& _ib) OVERRIDE
	{
		DEBUG_Require(!m_bLocked);
		m_bLocked = TRUE;
	}

	virtual void	Unlock() OVERRIDE
	{
		DEBUG_Require(m_bLocked);
		m_bLocked = FALSE;
	}

protected:
	VertexBufferRef	m_VB[_nbBuffer];
	IndexBufferRef	m_IB[_nbBuffer];
	Bool	m_bLocked;
END_SUPER_CLASS

//--------------------------------------------------

BEGIN_SUPER_CLASS(CStatiPrimitive3D,Primitive3DOf<1>)	
public:
	static  const Char* GetResourcePrefix() { return "primitive_static"; }
	virtual	const	VertexBufferRef& GetVB() const OVERRIDE { return m_VB[0]; }
	virtual	const	IndexBufferRef& GetIB() const OVERRIDE { return m_IB[0]; }
	virtual void	Lock(VertexBufferRef& _vb, IndexBufferRef& _ib) OVERRIDE { SUPER::Lock(_vb,_ib); _vb = m_VB[0]; _ib = m_IB[0]; }
	virtual void	Unlock() OVERRIDE { SUPER::Unlock(); }
END_SUPER_CLASS

//--------------------------------------------------

BEGIN_SUPER_CLASS(CDynamiPrimitive3D,Primitive3DOf<2>)
public:
	CDynamiPrimitive3D() : SUPER()
	{
		m_iDrawIndex = 0;
		m_iWriteIndex = 1;
		m_bChanged = FALSE;
	}

	static  const Char* GetResourcePrefix() { return "primitive_dynamic"; }
	virtual	const	VertexBufferRef& GetVB() const { return m_VB[m_iDrawIndex]; }
	virtual	const	IndexBufferRef& GetIB() const { return m_IB[m_iDrawIndex]; }
	virtual void	Lock(VertexBufferRef& _vb, IndexBufferRef& _ib) OVERRIDE { SUPER::Lock(_vb,_ib); _vb = m_VB[m_iWriteIndex]; _ib = m_IB[m_iWriteIndex]; }
	virtual void	Unlock() OVERRIDE { SUPER::Unlock(); m_bChanged = TRUE; }
	virtual void	PrepareDraw(){ Swap(); } 
	virtual	Bool	IsStatic() const { return FALSE; }

protected:
	void	Swap() { if (m_bChanged) { m_iDrawIndex = m_iWriteIndex; m_iWriteIndex = 1 - m_iWriteIndex; m_bChanged = FALSE; } }

	U8	m_iDrawIndex;
	U8	m_iWriteIndex;
	Bool m_bChanged;
END_SUPER_CLASS


#endif
