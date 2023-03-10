#include "vertex_buffer.h"

void VertexBuffer::SetVertexCount(const U16 _count)
{
	m_iVertexCount = _count;
	m_buffer.SetItemCount(_count * VB_ValueCountPerVertex);
}

//-----------------------------------------

Vec3f VertexBuffer::GetVertexAtIndex( const U32 vertex_index ) const
{
	U32 index = VB_ValueCountPerVertex * vertex_index;
	return Vec3f( m_buffer[ index ], m_buffer[ index + 1 ], m_buffer[ index + 2 ] );
}

//-----------------------------------------

void VertexBuffer::SetVertexAtIndex( const U32 vertex_index, const Vec3f& _pos, const Vec2f & tex_coord, const Vec3f & normal, const Color& color /*= Color::White*/ ) 
{
	U32 index = VB_ValueCountPerVertex * vertex_index;
	*((Vec3f*)(m_buffer.GetPointer() + index)) = _pos;
	*((Vec2f*)(m_buffer.GetPointer() + index + 3)) = tex_coord;
	*((Vec3f*)(m_buffer.GetPointer() + index + 5)) = normal;
	*((Color*)(m_buffer.GetPointer() + index + 8)) = color;
}

//-----------------------------------------

void VertexBuffer::SetVertexPosAtIndex( const U32 vertex_index, const Vec3f& _pos ) 
{
	U32 index = VB_ValueCountPerVertex * vertex_index;
	m_buffer[index] = _pos.x;
	m_buffer[index+1] = _pos.y;
	m_buffer[index+2] = _pos.z;
}

//-----------------------------------------

Vec2f VertexBuffer::GetTexCoordAtIndex( const U32 vertex_index ) const
{
	U32 index = ( VB_ValueCountPerVertex * vertex_index ) + 3;
	return Vec2f( m_buffer[ index ], m_buffer[ index + 1 ] );
}

//-----------------------------------------

Vec3f VertexBuffer::GetNormalAtIndex(const U32 vertex_index) const
{
	U32 index = ( VB_ValueCountPerVertex * vertex_index ) + 5;
	return Vec3f( m_buffer[ index ], m_buffer[ index + 1 ], m_buffer[ index + 2 ] );
}

//-----------------------------------------

Color VertexBuffer::GetColorAtIndex(const U32 vertex_index ) const
{
	U32 index = ( VB_ValueCountPerVertex * vertex_index ) + 7;
	return Color( m_buffer[ index ], m_buffer[ index + 1 ], m_buffer[ index + 2 ], m_buffer[ index + 3 ] );
}

//-----------------------------------------

U16 VertexBuffer::AddLastVertex( const Vec3f & vertex, const Vec2f & tex_coord, const Vec3f & normal, const Color& color/* = Color::White*/  )
{
	struct
	{
		Vec3f v;
		Vec2f t;
		Vec3f n;
		Color c;
	} value = {vertex,tex_coord,normal,color};
	m_buffer.SetItemCount(m_buffer.GetCount()+VB_ValueCountPerVertex);
	memcpy(m_buffer.GetPointer() + m_iVertexCount * VB_ValueCountPerVertex,&value,VB_ValueCountPerVertex*sizeof(Float));
	m_iVertexCount++;
	return m_iVertexCount - 1;
}

//-----------------------------------------

U16 VertexBuffer::AddUniqueVertex( const Vec3f & vertex, const Vec2f & texcoord, const Vec3f & normal, const Color& color/* = Color::White*/ )
{
    for( U16 i=0;i<m_iVertexCount;i++ )
    {
        U32 first_index = VB_ValueCountPerVertex * i;
        if ( m_buffer[ first_index + 0 ] == vertex.x &&
            m_buffer[ first_index + 1 ] == vertex.y &&
            m_buffer[ first_index + 2 ] == vertex.z &&
			m_buffer[ first_index + 3 ] == texcoord.x &&
			m_buffer[ first_index + 4 ] == texcoord.y &&
			m_buffer[ first_index + 5 ] == normal.x &&
			m_buffer[ first_index + 6 ] == normal.y &&
			m_buffer[ first_index + 7 ] == normal.z &&
			m_buffer[ first_index + 8 ] == color.r &&
			m_buffer[ first_index + 9 ] == color.g &&
			m_buffer[ first_index + 10 ] == color.b &&
			m_buffer[ first_index + 11 ] == color.a )
        {
            return i;
        }
    }

    AddLastVertex( vertex, texcoord, normal );
    return m_iVertexCount - 1;
}

//-----------------------------------------

void VertexBuffer::RemoveVertexAtIndex( const U32 index )
{
    U32 first_index = VB_ValueCountPerVertex * index;
	m_buffer.RemoveItems( first_index, VB_ValueCountPerVertex );
    m_iVertexCount--;
}