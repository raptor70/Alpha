#ifndef __VERTEX_BUFFER_H__
#define __VERTEX_BUFFER_H__

#define VB_ValueCountPerVertex 12  // X,Y,Z, U,V,  NX,NY,NZ, CR,CG,CB,CA

BEGIN_SUPER_CLASS(VertexBuffer, Resource)
public:
    VertexBuffer() {  m_iVertexCount = 0; }
    ~VertexBuffer( void ) { }

	static  const Char* GetResourcePrefix() { return "vb"; }

	void	Flush()		{ m_buffer.Flush(); m_iVertexCount = 0; }
    
    U16 GetVertexCount( void ) const { return m_iVertexCount; }
	void SetVertexCount(const U16 _count);
	Vec3f GetVertexAtIndex( const U32 vertex_index ) const;
	void SetVertexAtIndex( const U32 vertex_index, const Vec3f& _pos, const Vec2f & tex_coord, const Vec3f & normal, const Color& color = Color::White ) ;
	void SetVertexPosAtIndex( const U32 vertex_index, const Vec3f& _pos ) ;
	Vec2f GetTexCoordAtIndex( const U32 vertex_index ) const;
	Vec3f GetNormalAtIndex( const U32 vertex_index ) const;
	Color GetColorAtIndex(const U32 vertex_index ) const;

	const void * GetVertexArray(U32 _vertexOffset = 0) const { return m_buffer.GetPointer() + _vertexOffset* VB_ValueCountPerVertex; }
	const void * GetTexCoordArray(U32 _vertexOffset = 0) const { return m_buffer.GetPointer() + _vertexOffset * VB_ValueCountPerVertex + 3; }
	const void * GetNormalArray(U32 _vertexOffset = 0) const { return m_buffer.GetPointer() + _vertexOffset * VB_ValueCountPerVertex + 5; }
	const void * GetColorArray(U32 _vertexOffset = 0) const { return m_buffer.GetPointer() + _vertexOffset * VB_ValueCountPerVertex + 8; }
    const Float * GetArray( void ) const { return m_buffer.GetPointer(); }
	U32 GetArraySize() const { return m_buffer.GetItemSize(); }

	U16 AddLastVertex( const Vec3f & vertex, const Vec2f & tex_coord, const Vec3f & normal, const Color& color = Color::White );
	U16 AddUniqueVertex( const Vec3f & vertex, const Vec2f & texcoord, const Vec3f & normal, const Color& color = Color::White );
    void RemoveVertexAtIndex( const U32 index );

private :

	ArrayOf<Float>	m_buffer;
    U16				m_iVertexCount;
END_SUPER_CLASS

typedef RefTo<VertexBuffer> VertexBufferRef;

#endif
