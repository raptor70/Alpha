#include "component_spline.h"

BEGIN_SCRIPT_CLASS_INHERITED(EntityComponentSpline,EntityComponent)
	SCRIPT_VOID_METHOD2(SetFromPoints,S32,Vec3fArray)
	SCRIPT_VOID_METHOD1(SetTexture,Str)
	SCRIPT_VOID_METHOD1(SetThickness,Float)
	SCRIPT_VOID_METHOD1(SetTextureLength,Float)
END_SCRIPT_CLASS

DEFINE_COMPONENT_CLASS(Spline)
END_DEFINE_COMPONENT

EntityComponentSpline::EntityComponentSpline()
{
	m_Material = RendererDatas::GetInstance().CreateMaterial();
	m_fThickness = 1.f;
	m_fTextureLength = 10.f;
}

//------------------------------

EntityComponentSpline::~EntityComponentSpline()
{
}

//------------------------------

void EntityComponentSpline::Draw(Renderer*	_renderer)
{
	_renderer->SetActiveMaterial(m_Material,GetEntity()->GetColor());
	_renderer->DrawGeometry(
		Geometry_TrianglesStrip,
		GetEntity()->GetWorldPos(),
		GetEntity()->GetWorldRot(),
		m_Primitive,
		GetEntity()->GetWorldScale());
	_renderer->SetActiveMaterial(NULL);
}

//------------------------------

void	EntityComponentSpline::SetFromPoints(S32 _type, const Vec3fArray& _points)
{
	m_Primitive = Primitive3D::Create(Str("spline_%s", GetEntity()->GetName().GetStr().GetArray()), FALSE);
	VertexBufferRef vb;
	IndexBufferRef ib;
	m_Primitive->Lock(vb,ib);

	m_Spline.SetType((CSpline::Type)_type);
	m_Spline.SetFromPoints(_points);

	S32 segCount = 100;
	Float deltaRatio = 1.f / segCount;
	for(S32 i=0; i<=segCount; i++)
	{
		Vec3f tangent = Vec3f::Zero;
		Float ratio = i*deltaRatio;
		Vec3f pos = m_Spline.GetPos(ratio,&tangent);
		Vec3f left = Vec3f::ZAxis ^ tangent;
		left.Normalize();

		Vec3f p0 = pos + left * m_fThickness;
		Vec3f p1 = pos - left * m_fThickness;
		
		Float uvx = ratio * m_Spline.GetLength() / m_fTextureLength;
		Float uvmaxx = m_Material->GetTexture()->GetUVMaxX();
		Float uvmaxy = m_Material->GetTexture()->GetUVMaxY();
		uvx *= uvmaxx;
		U16 i0 = vb->AddLastVertex(p0,Vec2f(uvx,0.f),-Vec3f::ZAxis);
		U16 i1 = vb->AddLastVertex(p1,Vec2f(uvx,uvmaxy),-Vec3f::ZAxis);

		ib->m_Array.AddLastItem(i0);
		ib->m_Array.AddLastItem(i1);
	}

	m_Primitive->Unlock();
}