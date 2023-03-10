#include "component_mesh.h"

#include "resource/resource_manager.h"

#define DEFAULT_MESH_PATH ""

DEFINE_COMPONENT_CLASS(Mesh)
	INIT_ENTITY_PARAMS(Str, Path, DEFAULT_MESH_PATH);
	INIT_ENTITY_PARAMS(Color, Outline, Color::Black);

	EDITOR_CATEGORY(3D)
	EDITOR_FILE_VALUE(Path, Mesh);
END_DEFINE_COMPONENT

EntityComponentMesh::EntityComponentMesh()
{
}

//------------------------------

EntityComponentMesh::~EntityComponentMesh()
{
}

//------------------------------

void	EntityComponentMesh::Initialize()
{
	SUPER::Initialize();
}

//------------------------------

void	EntityComponentMesh::OnParamChanged()
{
	Str path = GET_ENTITY_PARAMS(Str, MeshPath);
	if (!path.IsEmpty())
		m_Mesh = ResourceManager::GetInstance().GetResourceFromFile<Mesh>(path);
}

//------------------------------

void	EntityComponentMesh::Update(Float _dTime) 
{
	SUPER::Update(_dTime);

	// DEBUG DRAW NORMAL
	Mesh* pMesh = (Mesh*)m_Mesh.GetPtr();
	for(S32 i=0; i<pMesh->GetNbSubMesh(); i++)
	{
		Primitive3DRef prim = pMesh->GetPrimitive(i);
		const VertexBufferRef& vb = prim->GetVB();
		for(U32 v=0;v<vb->GetVertexCount(); v++)
		{
			Vec3f pos = GetEntity()->GetWorldPos() + GetEntity()->GetWorldRot() * vb->GetVertexAtIndex(v);
			Vec3f normal = vb->GetNormalAtIndex(v);
			Vec3f pos2 = pos + GetEntity()->GetWorldRot() * normal;
			DEBUG_DrawLine(pos,pos2,Color::Yellow);
		}
	}
}

//------------------------------

void EntityComponentMesh::Draw(Renderer*	_renderer)
{
	if( m_Mesh.IsValid() )
	{
		Mesh* pMesh = (Mesh*)m_Mesh.GetPtr();
		Vec3f worldPos = GetEntity()->GetWorldPos();
		Quat worldRot = GetEntity()->GetWorldRot();
		Vec3f worldScale = GetEntity()->GetWorldScale();
		Color color = GetEntity()->GetColor();
		_renderer->SetActiveOutlineValue(GET_ENTITY_PARAMS(Color, MeshOutline));
		for(S32 i=0; i<pMesh->GetNbSubMesh(); i++)
		{
			_renderer->SetActiveMaterial(pMesh->GetMaterial(i), color);
			_renderer->DrawGeometry(
				Geometry_TrianglesList,
				worldPos,
				worldRot,
				pMesh->GetPrimitive(i),
				worldScale);
		}

		_renderer->SetActiveOutlineValue(Color::Black);
		_renderer->SetActiveMaterial(NULL);
	}
}
