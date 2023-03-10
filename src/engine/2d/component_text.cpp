#include "component_text.h"

#include "2d/font.h"

BEGIN_SCRIPT_CLASS_INHERITED(EntityComponentText,EntityComponent)
END_SCRIPT_CLASS

DEFINE_COMPONENT_CLASS(Text)
	INIT_ENTITY_PARAMS(Str, Value, "");
	INIT_ENTITY_PARAMS(Float, Scale, 20.f);
	INIT_ENTITY_PARAMS(S32, HAlign, Font::HALIGN_CENTER);
	INIT_ENTITY_PARAMS(S32, VAlign, Font::VALIGN_CENTER);
	INIT_ENTITY_PARAMS(Str, Font, "debug");
	INIT_ENTITY_PARAMS(Color, Color, Color::White);
	INIT_ENTITY_PARAMS(Float, DeltaZ, 0.0f);
	INIT_ENTITY_PARAMS(Float, BlockSizeX, 0.f);
	INIT_ENTITY_PARAMS(Float, BlockSizeY, 0.f);

	EDITOR_CATEGORY(2D)
	EDITOR_START_ENUM_PARAM(HAlign)
		EDITOR_ENUM_VALUE(Left,Font::HALIGN_LEFT)
		EDITOR_ENUM_VALUE(Center,Font::HALIGN_CENTER)
		EDITOR_ENUM_VALUE(Right,Font::HALIGN_RIGHT)
	EDITOR_END_ENUM_PARAM
	EDITOR_START_ENUM_PARAM(VAlign)
		EDITOR_ENUM_VALUE(Top,Font::VALIGN_TOP)
		EDITOR_ENUM_VALUE(Center,Font::VALIGN_CENTER)
		EDITOR_ENUM_VALUE(Bottom,Font::VALIGN_BOTTOM)
	EDITOR_END_ENUM_PARAM
END_DEFINE_COMPONENT

EntityComponentText::EntityComponentText()
{
	m_bChanged = FALSE;
}

//------------------------------

EntityComponentText::~EntityComponentText()
{
}

//------------------------------

void	EntityComponentText::Initialize()
{
	SUPER::Initialize();
}

//------------------------------

void	EntityComponentText::OnParamChanged()
{
	SUPER::OnParamChanged();

	m_Text = GET_ENTITY_PARAMS(Str, TextValue);
	m_bChanged = TRUE;
}

//------------------------------

void	EntityComponentText::ComputeGeometry()
{
	const Font* pFont = FontManager::GetInstance().GetFont(GET_ENTITY_PARAMS(Str, TextFont));
	m_Mesh.Clean();
	Font::STextBlockParams params;
	params.m_HAlign = (Font::EHAlignment)GET_ENTITY_PARAMS(S32, TextHAlign);
	params.m_VAlign = (Font::EVAlignment)GET_ENTITY_PARAMS(S32, TextVAlign);
	params.m_Size = GET_ENTITY_PARAMS(Float, TextScale);
	params.m_BlockSize.x = GET_ENTITY_PARAMS(Float, TextBlockSizeX) / params.m_Size;
	params.m_BlockSize.y = GET_ENTITY_PARAMS(Float, TextBlockSizeY) / params.m_Size;
	params.m_outMesh = &m_Mesh;
	pFont->ComputeTextBlock(m_Text, params);
}

//------------------------------

void EntityComponentText::Draw(Renderer*	_renderer)
{
	if( m_bChanged )
	{
		ComputeGeometry();
		m_bChanged = FALSE;
	}

	Vec3f worldPos = GetEntity()->GetWorldPos();
	Quat worldRot = GetEntity()->GetWorldRot();
	worldPos.z += GET_ENTITY_PARAMS(Float, TextDeltaZ);
	Vec3f worldScale = GetEntity()->GetWorldScale()*GET_ENTITY_PARAMS(Float, TextScale);

	for (S32 i = 0; i<m_Mesh.GetNbSubMesh(); i++)
	{
		_renderer->SetActiveMaterial(m_Mesh.GetMaterial(i), GET_ENTITY_PARAMS(Color, TextColor));
		_renderer->DrawGeometry(Geometry_TrianglesList, worldPos, worldRot, m_Mesh.GetPrimitive(i), worldScale, TRUE);
	}

	_renderer->SetActiveMaterial(NULL);
}
