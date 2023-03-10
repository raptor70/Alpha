#include "component_quad.h"

BEGIN_SCRIPT_CLASS_INHERITED(EntityComponentQuad,EntityComponent)
	SCRIPT_VOID_METHOD1(SetIsAdditive,Bool)
END_SCRIPT_CLASS

BEGIN_SCRIPT_CLASS_INHERITED(EntityComponentMaskedQuad,EntityComponentQuad)
END_SCRIPT_CLASS

DEFINE_COMPONENT_CLASS(Quad)
	INIT_ENTITY_PARAMS(Float, SizeX, 1.f);
	INIT_ENTITY_PARAMS(Float, SizeY, 1.f);
	INIT_ENTITY_PARAMS(Color, Color, Color::White);
	INIT_ENTITY_PARAMS(Str, Texture, DEFAULT_TEXTURE_PATH);
	EDITOR_CATEGORY(2D)
	EDITOR_FILE_VALUE(Texture, Texture);
END_DEFINE_COMPONENT

DEFINE_COMPONENT_CLASS(MaskedQuad)
	INIT_ENTITY_PARAMS(Float, MaskMinX, 0.f);
	INIT_ENTITY_PARAMS(Float, MaskMinY, 0.f);
	INIT_ENTITY_PARAMS(Float, MaskMaxX, 0.f);
	INIT_ENTITY_PARAMS(Float, MaskMaxY, 0.f);
END_DEFINE_COMPONENT

EntityComponentQuad::EntityComponentQuad()
{
	m_Material = RendererDatas::GetInstance().CreateMaterial();
}

//------------------------------

EntityComponentQuad::~EntityComponentQuad()
{
}

//------------------------------

void	EntityComponentQuad::SetColor(const Color& _color)
{
	m_Material->SetDiffuseColor(_color);
}

//------------------------------

void	EntityComponentQuad::SetIsAdditive(Bool _isAdditive)
{
	m_Material->SetIsAdditive(_isAdditive);
}

//------------------------------

void	EntityComponentQuad::Initialize()
{
	SUPER::Initialize();
}

//------------------------------

void	EntityComponentQuad::OnParamChanged()
{
	SUPER::OnParamChanged();

	m_Material->LoadTexture(GET_ENTITY_PARAMS(Str, QuadTexture));
	m_Material->SetDiffuseColor(GET_ENTITY_PARAMS(Color, QuadColor));
	CreateGeometry();
}

//------------------------------

void EntityComponentQuad::Draw(Renderer*	_renderer)
{
	_renderer->SetActiveMaterial(GetCurrentMaterial(),GetEntity()->GetColor());
	_renderer->DrawGeometry(Geometry_TrianglesList,GetEntity()->GetWorldPos(),GetEntity()->GetWorldRot(),m_Primitive,GetEntity()->GetWorldScale(),TRUE);
	_renderer->SetActiveMaterial(NULL);
}

//------------------------------

void EntityComponentQuad::CreateGeometry()
{
	Vec2f size = GetSize();
	Float halfScaleX = size.x*.5f;
	Float halfScaleY = size.y*.5f;
	Vec2f uvmin,uvmax;
	uvmin = Vec2f::Zero;
	uvmax = Vec2f::One;
	if( m_Material->GetTexture() )
	{
		uvmin = m_Material->GetUvMin();
		uvmax = m_Material->GetUvMax();
	}
	m_Primitive = Primitive3D::Create(Str("quad_%s",GetEntity()->GetName().GetStr().GetArray()),FALSE);
	VertexBufferRef vb;
	IndexBufferRef ib;
	m_Primitive->Lock(vb,ib);
	vb->AddLastVertex(Vec3f(-halfScaleX,-halfScaleY,0.f),Vec2f(uvmin.x,uvmax.y),-Vec3f::ZAxis);
	vb->AddLastVertex(Vec3f(-halfScaleX,halfScaleY,0.f),Vec2f(uvmin.x,uvmin.y),-Vec3f::ZAxis);
	vb->AddLastVertex(Vec3f(halfScaleX,-halfScaleY,0.f),Vec2f(uvmax.x,uvmax.y),-Vec3f::ZAxis);
	vb->AddLastVertex(Vec3f(halfScaleX,halfScaleY,0.f),Vec2f(uvmax.x,uvmin.y),-Vec3f::ZAxis);
	ib->m_Array.AddLastItem(0);
	ib->m_Array.AddLastItem(1);
	ib->m_Array.AddLastItem(2);
	ib->m_Array.AddLastItem(1);
	ib->m_Array.AddLastItem(3);
	ib->m_Array.AddLastItem(2);
	m_Primitive->Unlock();
}

//------------------------------

EntityComponentMaskedQuad::EntityComponentMaskedQuad()
{
}

//------------------------------

void	EntityComponentMaskedQuad::Initialize()
{
	SUPER::Initialize();
}

//------------------------------

void EntityComponentMaskedQuad::CreateGeometry()
{
	Vec2f vMinMask;
	vMinMask.x = GET_ENTITY_PARAMS(Float,MaskedQuadMaskMinX);
	vMinMask.y = GET_ENTITY_PARAMS(Float,MaskedQuadMaskMinY);
	Vec2f vMaxMask;
	vMaxMask.x = GET_ENTITY_PARAMS(Float,MaskedQuadMaskMaxX);
	vMaxMask.y = GET_ENTITY_PARAMS(Float,MaskedQuadMaskMaxY);

	Vec2f size = GetSize();
	Float halfScaleX = size.x*.5f;
	Float halfScaleY = size.y*.5f;
	Float uvmaxx  = 1.f;
	Float uvmaxy  = 1.f;
	if( m_Material->GetTexture() )
	{
		uvmaxx = m_Material->GetTexture()->GetUVMaxX();
		uvmaxy = m_Material->GetTexture()->GetUVMaxY();
	}

	m_Primitive = Primitive3D::Create(Str("maskedquad_%s", GetEntity()->GetName().GetStr().GetArray()), FALSE);
	VertexBufferRef vb;
	IndexBufferRef ib;
	m_Primitive->Lock(vb,ib);
	if( -halfScaleX < vMinMask.x )
	{
		Float uvmaxMask = ( vMinMask.x + halfScaleX ) * uvmaxx/ size.x;
		U16 i0 = vb->AddLastVertex(Vec3f(-halfScaleX,-halfScaleY,0.f),Vec2f(0.f,uvmaxy),Vec3f::ZAxis);
		U16 i1 = vb->AddLastVertex(Vec3f(-halfScaleX,halfScaleY,0.f),Vec2f(0.f,0.f),Vec3f::ZAxis);
		U16 i2 = vb->AddLastVertex(Vec3f(vMinMask.x,-halfScaleY,0.f),Vec2f(uvmaxMask,uvmaxy),Vec3f::ZAxis);
		U16 i3 = vb->AddLastVertex(Vec3f(vMinMask.x,halfScaleY,0.f),Vec2f(uvmaxMask,0.f),Vec3f::ZAxis);
		ib->m_Array.AddLastItem(i0);
		ib->m_Array.AddLastItem(i1);
		ib->m_Array.AddLastItem(i2);
		ib->m_Array.AddLastItem(i1);
		ib->m_Array.AddLastItem(i2);
		ib->m_Array.AddLastItem(i3);
	}

	if( halfScaleX > vMaxMask.x )
	{
		Float uvminMask = ( vMaxMask.x + halfScaleX ) * uvmaxx/ size.x;
		U16 i0 = vb->AddLastVertex(Vec3f(vMaxMask.x,-halfScaleY,0.f),Vec2f(uvminMask,uvmaxy),Vec3f::ZAxis);
		U16 i1 = vb->AddLastVertex(Vec3f(vMaxMask.x,halfScaleY,0.f),Vec2f(uvminMask,0.f),Vec3f::ZAxis);
		U16 i2 = vb->AddLastVertex(Vec3f(halfScaleX,-halfScaleY,0.f),Vec2f(uvmaxx,uvmaxy),Vec3f::ZAxis);
		U16 i3 = vb->AddLastVertex(Vec3f(halfScaleX,halfScaleY,0.f),Vec2f(uvmaxx,0.f),Vec3f::ZAxis);
		ib->m_Array.AddLastItem(i0);
		ib->m_Array.AddLastItem(i1);
		ib->m_Array.AddLastItem(i2);
		ib->m_Array.AddLastItem(i1);
		ib->m_Array.AddLastItem(i2);
		ib->m_Array.AddLastItem(i3);
	}

	if( -halfScaleY < vMinMask.y )
	{
		Float uvminMaskX = ( vMinMask.x + halfScaleX ) * uvmaxx / size.x;
		Float uvmaxMaskX = ( vMaxMask.x + halfScaleX ) * uvmaxx / size.x;
		Float uvmaxMaskY = ( vMinMask.y - halfScaleY ) * uvmaxy / -size.y;
		U16 i0 = vb->AddLastVertex(Vec3f(vMinMask.x,-halfScaleY,0.f),Vec2f(uvminMaskX,uvmaxy),Vec3f::ZAxis);
		U16 i1 = vb->AddLastVertex(Vec3f(vMinMask.x,vMinMask.y,0.f),Vec2f(uvminMaskX,uvmaxMaskY),Vec3f::ZAxis);
		U16 i2 = vb->AddLastVertex(Vec3f(vMaxMask.x,-halfScaleY,0.f),Vec2f(uvmaxMaskX,uvmaxy),Vec3f::ZAxis);
		U16 i3 = vb->AddLastVertex(Vec3f(vMaxMask.x,vMinMask.y,0.f),Vec2f(uvmaxMaskX,uvmaxMaskY),Vec3f::ZAxis);
		ib->m_Array.AddLastItem(i0);
		ib->m_Array.AddLastItem(i1);
		ib->m_Array.AddLastItem(i2);
		ib->m_Array.AddLastItem(i1);
		ib->m_Array.AddLastItem(i2);
		ib->m_Array.AddLastItem(i3);
	}

	if( halfScaleY > vMaxMask.y )
	{
		Float uvminMaskX = ( vMinMask.x + halfScaleX ) * uvmaxx/ size.x;
		Float uvmaxMaskX = ( vMaxMask.x + halfScaleX ) * uvmaxx/ size.x;
		Float uvminMaskY = ( vMaxMask.y - halfScaleY ) * uvmaxy/ -size.y;
		U16 i0 = vb->AddLastVertex(Vec3f(vMinMask.x,vMaxMask.y,0.f),Vec2f(uvminMaskX,uvminMaskY),Vec3f::ZAxis);
		U16 i1 = vb->AddLastVertex(Vec3f(vMinMask.x,halfScaleY,0.f),Vec2f(uvminMaskX,0.f),Vec3f::ZAxis);
		U16 i2 = vb->AddLastVertex(Vec3f(vMaxMask.x,vMaxMask.y,0.f),Vec2f(uvmaxMaskX,uvminMaskY),Vec3f::ZAxis);
		U16 i3 = vb->AddLastVertex(Vec3f(vMaxMask.x,halfScaleY,0.f),Vec2f(uvmaxMaskX,0.f),Vec3f::ZAxis);
		ib->m_Array.AddLastItem(i0);
		ib->m_Array.AddLastItem(i1);
		ib->m_Array.AddLastItem(i2);
		ib->m_Array.AddLastItem(i1);
		ib->m_Array.AddLastItem(i2);
		ib->m_Array.AddLastItem(i3);
	}

	m_Primitive->Unlock();
}

