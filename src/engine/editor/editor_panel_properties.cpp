#include "editor_panel_properties.h"

#ifdef USE_EDITOR_V2

void EditorPanelProperties::OnSeletedChanged()
{
	if (m_pSelected.IsValid())
	{
		const Name& name = m_pSelected->GetName();
		memcpy(m_RenameBuffer, name.GetStr().GetArray(), name.GetStr().GetLength() + 1);
	}
}

//..

void EditorPanelProperties::UpdatePanelContent(Float _dTime)
{
	if (m_pSelected.IsValid())
	{
		ImGui::SetNextItemWidth(-1.f);
		if (ImGui::InputText("##properties_rename", m_RenameBuffer, sizeof(m_RenameBuffer), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			m_pSelected->SetName(m_RenameBuffer);
		}

		Float ratios[] = { 0.3f, 0.7f };
		ImGuiEx::Columns(2, NULL, TRUE, ratios);
		//ImGui::Columns(2);

		//pos
		Vec3f pos = m_pSelected->GetWorldPos();
		ImGui::Text("Position");
		ImGui::NextColumn();
		ImGui::SetNextItemWidth(-1);
		if (ImGui::DragFloat3("##Pos", pos.xyz))
		{
			m_pSelected->SetWorldPos(pos);
		}
		ImGui::NextColumn();

		//rot
		Vec3f rot = m_pSelected->GetWorldRot().GetEulerAngles();
		rot.x = MATH_RAD_TO_DEG(rot.x);
		rot.y = MATH_RAD_TO_DEG(rot.y);
		rot.z = MATH_RAD_TO_DEG(rot.z);
		ImGui::Text("Rotation");
		ImGui::NextColumn();
		ImGui::SetNextItemWidth(-1);
		if (ImGui::DragFloat3("##Rot", rot.xyz))
		{
			rot.x = MATH_DEG_TO_RAD(rot.x);
			rot.y = MATH_DEG_TO_RAD(rot.y);
			rot.z = MATH_DEG_TO_RAD(rot.z);
			Quat qrot;
			qrot.SetFromEulerAngles(rot);
			m_pSelected->SetWorldRot(qrot);
		}
		ImGui::NextColumn();

		//scale
		Vec3f scale = m_pSelected->GetWorldScale();
		ImGui::Text("Scale");
		ImGui::NextColumn();
		ImGui::SetNextItemWidth(-1);
		if (ImGui::DragFloat3("##Scale", scale.xyz))
		{
			m_pSelected->SetWorldScale(scale);
		}
		ImGui::NextColumn();

		// color 
		Color color = m_pSelected->GetColor();
		ImGui::Text("Color");
		ImGui::NextColumn();
		ImGui::SetNextItemWidth(-1);
		if (ImGui::ColorEdit4("##Col", color.rgba))
		{
			m_pSelected->SetColor(color);
		}
		ImGui::Columns(1);


		//hide
		Bool hidden = m_pSelected->IsHidden();
		if (ImGui::Checkbox("Hidden", &hidden))
		{
			if(hidden)
				m_pSelected->Hide();
			else
				m_pSelected->Show();
		}
		ImGui::Separator();
		
		for (U32 c = 0; c < m_pSelected->GetNbComponents(); c++)
		{
			EntityComponent* pComp = m_pSelected->GetComponent(c);
			const EntityComponentFactory::Register* pRegister = EntityComponentFactory::GetInstance().GetComponentRegister(pComp->GetComponentName());

			if (ImGui::CollapsingHeader(pComp->GetComponentName().GetStr().GetArray(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Columns(2);
				ParamsTable& table = m_pSelected->GetParamsTable();
				NameArray orgComps;
				NameArray paramNames;
				pRegister->GetParamNames(paramNames, orgComps);

				for (U32 p = 0; p < paramNames.GetCount(); p++)
				{
					const Params* param = table.GetValue(paramNames[p]);
					U32 iCompNameSize = orgComps[p].GetStr().GetLength();
					const Str& orgName = param->GetName().GetStr();
					Str name = orgName.GetTextInsideRange(iCompNameSize, orgName.GetLength() - 1);
					ImGui::Text(name.GetArray());

					Str id;
					id.SetFrom("##%d%s", c, orgName.GetArray());

					ImGui::NextColumn();
					ImGui::SetNextItemWidth(-1);
					const EntityComponentFactory::Register::SEditorEnum* pEnum = NULL;
					const EntityComponentFactory::Register::SEditorFile* pFile = NULL;
					if (pRegister->FindEnumParam(param->GetName(), &pEnum)) // enum
					{
					}
					else if (pRegister->IsSoundParam(param->GetName())) // sound
					{
					}
					else if (pRegister->FindFileParam(param->GetName(), &pFile)) // file
					{
					}
					else // default
					{
						switch (param->GetType())
						{
							case Params::TYPE_Bool:
							{
								Bool bVal = param->GetBool();
								if (ImGui::Checkbox(id.GetArray(),&bVal))
								{
									m_pSelected->SetParamBool(param->GetName(), bVal); //m_pEntity->SetParamChangedByEditor(_name); ?
								}
								break;
							}
							case Params::TYPE_Float:
							{
								Float fVal = param->GetFloat();
								if (ImGui::DragFloat(id.GetArray(), &fVal))
								{
									m_pSelected->SetParamFloat(param->GetName(), fVal); //m_pEntity->SetParamChangedByEditor(_name); ?
								}
								break;
							}
							case Params::TYPE_S32:
							{
								S32 iVal = param->GetS32();
								if (ImGui::DragInt(id.GetArray(), (int*)&iVal))
								{
									m_pSelected->SetParamS32(param->GetName(), iVal); //m_pEntity->SetParamChangedByEditor(_name); ?
								}
								break;
							}
							case Params::TYPE_Color:
							{
								Color color = param->GetColor();
								if (ImGui::ColorEdit4(id.GetArray(), color.rgba))
								{
									m_pSelected->SetParamColor(param->GetName(), color); //m_pEntity->SetParamChangedByEditor(_name); ?
								}
								break;
							}
							case Params::TYPE_Str:
							{
								Char bBuffer[256] = "";
								memcpy(bBuffer, param->GetStr().GetArray(), param->GetStr().GetLength() + 1);
								if (ImGui::InputText(id.GetArray(), bBuffer, sizeof(bBuffer), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
								{
									m_pSelected->SetParamStr(param->GetName(), bBuffer); //m_pEntity->SetParamChangedByEditor(_name); ?
								}
								break;
							}
							default:
							{
								break;
							}
						}
					}
					
					ImGui::NextColumn();
				}

				ImGui::Columns(1);
			}
		}
	}
}


#endif // USE_EDITOR_V2