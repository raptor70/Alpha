#include "loader_DAE.h"

#include "3d/mesh.h"
#include "3d/skin.h"
#include "3d/animation.h"
#include "resource_manager.h"
#include "core/name_static.h"

DEFINE_LOADER_CLASS(DAE);

#define DAE_UNSUPPORTED(toto) LOGGER_LogError("DAE : %s not yet support !\n",#toto);
#define DAE_UNKNOWN(toto) LOGGER_LogError("DAE : %s unknown !\n",toto);
#define DAE_CHECK(a) {if(!(a)) { LOGGER_LogError("DAE : check invalid (%s) !\n",#a); return FALSE; } }

Loader_DAE::Loader_DAE()
{
}

//-----------------------------------------------

Loader_DAE::~Loader_DAE()
{
}

//-----------------------------------------------

Bool Loader_DAE::LoadToResources(const Str& _path, ResourceRefArray& _outResources)
{
	m_Path = _path;
	return SUPER::LoadToResources(_path,_outResources);
}

//-----------------------------------------------

Bool Loader_DAE::LoadToResourcesFromXml(const rapidxml::xml_document<>& _xmlDoc, ResourceRefArray& _outResources)
{
	rapidxml::xml_node<> *node = _xmlDoc.first_node();
	DAE_CHECK( Name(node->name()) == NAME(COLLADA) );

	const rapidxml::xml_node<> *child = node->first_node();
	while(child)
	{
		Name childName = child->name();

		if( childName == NAME(asset) )
		{
			DAE_CHECK(LoadAsset(child));
		}
		else if( childName == NAME(library_animations) )
		{
			DAE_CHECK(LoadLibAnimations(child));
		}
		else if( childName == NAME(library_animation_clips) )
		{
			DAE_UNSUPPORTED(library_animation_clips);
		}
		else if( childName == NAME(library_cameras) )
		{
			DAE_UNSUPPORTED(library_cameras);
		}
		else if( childName == NAME(library_controllers) )
		{
			DAE_CHECK(LoadLibControllers(child));
		}
		else if( childName == NAME(library_effects) )
		{
			DAE_CHECK(LoadLibEffects(child));
		}
		else if( childName == NAME(library_force_fields) )
		{
			DAE_UNSUPPORTED(library_force_fields);
		}
		else if( childName == NAME(library_geometries) )
		{
			DAE_CHECK(LoadLibGeometries(child));
		}
		else if( childName == NAME(library_images) )
		{
			DAE_CHECK(LoadLibImages(child));
		}
		else if( childName == NAME(library_lights) )
		{
			DAE_CHECK(LoadLibLights(child));
		}
		else if( childName == NAME(library_materials) )
		{
			DAE_CHECK(LoadLibMaterials(child));
		}
		else if( childName == NAME(library_nodes) )
		{
			DAE_UNSUPPORTED(library_nodes);
		}
		else if( childName == NAME(library_physics_materials) )
		{
			DAE_UNSUPPORTED(library_physics_materials);
		}
		else if( childName == NAME(library_physics_models) )
		{
			DAE_UNSUPPORTED(library_physics_models);
		}
		else if( childName == NAME(library_physics_scenes) )
		{
			DAE_UNSUPPORTED(library_physics_scenes);
		}
		else if( childName == NAME(library_visual_scenes) )
		{
			DAE_CHECK(LoadLibVisualScenes(child));
		}
		else if( childName == NAME(scene) )
		{
			DAE_CHECK(LoadScene(child,_outResources));
		}
		else 
		{
			DAE_UNKNOWN(childName.GetStr().GetArray());
		}

		child = child->next_sibling();
	}

	// todo 
	return TRUE;
}

//------------------------------

Bool	Loader_DAE::LoadAsset(const rapidxml::xml_node<>* _xmlNode)
{
	// todo 
	return TRUE;
}

//------------------------------

Bool	Loader_DAE::LoadExtra(const rapidxml::xml_node<>* _xmlNode)
{
	// todo 
	return TRUE;
}

//------------------------------

Bool	Loader_DAE::LoadScene(const rapidxml::xml_node<>* _xmlNode, ResourceRefArray& _outResources)
{
	const rapidxml::xml_node<> *child = _xmlNode->first_node();
	while(child)
	{
		Name childName = child->name();

		if( childName == NAME(instance_physics_scene) )
		{
			DAE_UNSUPPORTED(instance_physics_scene);
		}
		else if( childName == NAME(instance_visual_scene) )
		{
			Str visual_scene_name;
			DAE_CHECK(ReadParameterStr(child,"url",visual_scene_name));
			VisualSceneDAE* visualScene = NULL;
			if( visual_scene_name.GetLength() == 1 ) // #
			{
				DEBUG_Require(m_allVisualScene.GetCount()==1);
				visualScene = &m_allVisualScene[0];
			}
			else
			{
				visual_scene_name = visual_scene_name.GetTextInsideRange(1,visual_scene_name.GetLength()-1); // remove #
				
				// find visual scene
				for(U32 s = 0; s < m_allVisualScene.GetCount(); s++)
				{
					if( m_allVisualScene[s].m_Name == visual_scene_name )
					{
						visualScene = &m_allVisualScene[s];
						break;
					}
				}
			}

			// parse it
			if( visualScene )
			{
				// generate nodes from visual scene
				for(U32 n=0; n<visualScene->m_Nodes.GetCount(); n++)
				{
					DAE_CHECK(GenerateResourceFromNode(*visualScene,visualScene->m_Nodes[n],_outResources));
				}
			}
		}
		else if( childName == NAME(extra) )
		{
			DAE_CHECK(LoadExtra(child));
		}
		else 
		{
			DAE_UNKNOWN(childName.GetStr().GetArray());
		}

		child = child->next_sibling();

	}

	return TRUE;
}

//------------------------------

Bool	Loader_DAE::LoadLibImages(const rapidxml::xml_node<>* _xmlNode)
{
	// todo attrib 

	const rapidxml::xml_node<> *child = _xmlNode->first_node();
	while(child)
	{
		Name childName = child->name();

		if( childName == NAME(asset) )
		{
			DAE_CHECK(LoadAsset(child));
		}
		else if( childName == NAME(image) )
		{
			m_allImages.AddLastItem(ImageDAE());
			ImageDAE& image = m_allImages[m_allImages.GetCount()-1];
			DAE_CHECK(LoadImage(child,image));
		}
		else if( childName == NAME(extra) )
		{
			DAE_CHECK(LoadExtra(child));
		}
		else 
		{
			DAE_UNKNOWN(childName.GetStr().GetArray());
		}

		child = child->next_sibling();

	}

	return TRUE;
}

//------------------------------

Bool	Loader_DAE::LoadImage(const rapidxml::xml_node<>* _xmlNode, ImageDAE& _outImage)
{
	Str id;
	DAE_CHECK(ReadParameterStr(_xmlNode,"id",id));
	_outImage.m_Name = id;

	const rapidxml::xml_node<> *child = _xmlNode->first_node();
	Bool bUnique = FALSE;
	while(child)
	{
		Name childName = child->name();

		if( childName == NAME(asset) )
		{
			DAE_CHECK(LoadAsset(child));
		}
		else if( childName == NAME(data) )
		{
			DEBUG_Require(!bUnique);
			bUnique = TRUE;
			DAE_UNSUPPORTED(data);
		}
		else if( childName == NAME(init_from) )
		{
			DEBUG_Require(!bUnique);
			bUnique = TRUE;
			_outImage.m_Path = child->value();
			_outImage.m_Type = ImageDAE::TYPE_FROMPATH;
		}
		else if( childName == NAME(extra) )
		{
			DAE_CHECK(LoadExtra(child));
		}
		else 
		{
			DAE_UNKNOWN(childName.GetStr().GetArray());
		}

		child = child->next_sibling();

	}
	return TRUE;
}

//------------------------------

Bool	Loader_DAE::LoadLibMaterials(const rapidxml::xml_node<>* _xmlNode)
{
	// todo attrib 

	const rapidxml::xml_node<> *child = _xmlNode->first_node();
	while(child)
	{
		Name childName = child->name();

		if( childName == NAME(asset) )
		{
			DAE_CHECK(LoadAsset(child));
		}
		else if( childName == NAME(material) )
		{
			m_allMaterials.AddLastItem(MaterialDAE());
			MaterialDAE& mat = m_allMaterials[m_allMaterials.GetCount()-1];
			DAE_CHECK(LoadMaterial(child,mat));
		}
		else if( childName == NAME(extra) )
		{
			DAE_CHECK(LoadExtra(child));
		}
		else 
		{
			DAE_UNKNOWN(childName.GetStr().GetArray());
		}

		child = child->next_sibling();

	}

	return TRUE;
}

//------------------------------

Bool	Loader_DAE::LoadMaterial(const rapidxml::xml_node<>* _xmlNode, MaterialDAE& _outMaterial)
{
	Str id;
	DAE_CHECK(ReadParameterStr(_xmlNode,"id",id));
	_outMaterial.m_Name = id;

	const rapidxml::xml_node<> *child = _xmlNode->first_node();
	while(child)
	{
		Name childName = child->name();

		if( childName == NAME(asset) )
		{
			DAE_CHECK(LoadAsset(child));
		}
		else if( childName == NAME(instance_effect) )
		{
			DAE_CHECK(ReadParameterStr(child,"url",_outMaterial.m_Effect));
			_outMaterial.m_Effect = _outMaterial.m_Effect.GetTextInsideRange(1,_outMaterial.m_Effect.GetLength()-1); // remove #
		}
		else if( childName == NAME(extra) )
		{
			DAE_CHECK(LoadExtra(child));
		}
		else 
		{
			DAE_UNKNOWN(childName.GetStr().GetArray());
		}

		child = child->next_sibling();

	}

	return TRUE;
}

//------------------------------

Bool	Loader_DAE::LoadLibEffects(const rapidxml::xml_node<>* _xmlNode)
{
	// todo attrib 

	const rapidxml::xml_node<> *child = _xmlNode->first_node();
	while(child)
	{
		Name childName = child->name();

		if( childName == NAME(asset) )
		{
			DAE_CHECK(LoadAsset(child));
		}
		else if( childName == NAME(effect) )
		{
			m_allEffects.AddLastItem(EffectDAE());
			EffectDAE& effect = m_allEffects[m_allEffects.GetCount()-1];
			DAE_CHECK(LoadEffect(child,effect));
		}
		else if( childName == NAME(extra) )
		{
			DAE_CHECK(LoadExtra(child));
		}
		else 
		{
			DAE_UNKNOWN(childName.GetStr().GetArray());
		}

		child = child->next_sibling();
	}

	return TRUE;
}

//------------------------------

Bool	Loader_DAE::LoadEffect(const rapidxml::xml_node<>* _xmlNode, EffectDAE& _outEffect)
{
	Str id;
	DAE_CHECK(ReadParameterStr(_xmlNode,"id",id));
	_outEffect.m_Name = id;

	const rapidxml::xml_node<> *child = _xmlNode->first_node();
	while(child)
	{
		Name childName = child->name();

		if( childName == NAME(asset) )
		{
			DAE_CHECK(LoadAsset(child));
		}
		else if( childName == NAME(annotate) )
		{
			DAE_UNSUPPORTED(effect);
		}
		else if( childName == NAME(image) )
		{
			ImageDAE image;
			DAE_CHECK(LoadImage(child,image));
		}
		else if( childName == NAME(newparam) )
		{
			DAE_UNSUPPORTED(newparam);
		}
		else if( childName == NAME(profile_COMMON) )
		{
			rapidxml::xml_node<>* technique = child->first_node("technique");
			if( technique )
			{
				rapidxml::xml_node<>* phong = technique->first_node("phong");
				if( phong )
				{
					_outEffect.m_Type = EffectDAE::TYPE_PHONG;
					DAE_CHECK(LoadColor(phong->first_node("emission"),_outEffect.m_Emissive));
					DAE_CHECK(LoadColor(phong->first_node("ambient"),_outEffect.m_Ambient));
					DAE_CHECK(LoadColor(phong->first_node("diffuse"),_outEffect.m_Diffuse));
					DAE_CHECK(LoadColor(phong->first_node("specular"),_outEffect.m_Specular));
					DAE_CHECK(LoadFloat(phong->first_node("shininess"),_outEffect.m_Shininess));
					DAE_CHECK(LoadColor(phong->first_node("reflective"),_outEffect.m_Reflective));
					DAE_CHECK(LoadFloat(phong->first_node("reflectivity"),_outEffect.m_Reflectivity));
					DAE_CHECK(LoadColor(phong->first_node("transparent"),_outEffect.m_Transparent));
					DAE_CHECK(LoadFloat(phong->first_node("transparency"),_outEffect.m_Transparency));

					rapidxml::xml_node<>* diffuse = phong->first_node("diffuse");
					if( diffuse )
					{
						rapidxml::xml_node<>* texture = diffuse->first_node("texture");
						if( texture )
						{
							Str name;
							DAE_CHECK(ReadParameterStr(texture,"texture",name));
							_outEffect.m_DiffuseTexture = name;
						}
					}

					rapidxml::xml_node<>* emissive = phong->first_node("emission");
					if (emissive)
					{
						rapidxml::xml_node<>* texture = emissive->first_node("texture");
						if (texture)
						{
							Str name;
							DAE_CHECK(ReadParameterStr(texture, "texture", name));
							_outEffect.m_EmissiveTexture = name;
						}
					}
				}
			}
		}
		else if( childName == NAME(fx_profile_abstract) )
		{
			DAE_UNSUPPORTED(fx_profile_abstract);
		}
		else if( childName == NAME(extra) )
		{
			DAE_CHECK(LoadExtra(child));
		}
		else 
		{
			DAE_UNKNOWN(childName.GetStr().GetArray());
		}

		child = child->next_sibling();
	}

	return TRUE;
}

//------------------------------

Bool	Loader_DAE::LoadColor(const rapidxml::xml_node<>* _xmlNode, Color& _outColor)
{
	DAE_CHECK(_xmlNode );

	rapidxml::xml_node<>* color = _xmlNode->first_node("color");
	if( color )
	{
		Str data = color->value();
		m_Values.ReserveCount(4);
		data.Split(" ", m_Values);
		DAE_CHECK( (m_Values.GetCount() == 4));
		_outColor.r = m_Values[0].ToFloat();
		_outColor.g = m_Values[1].ToFloat();
		_outColor.b = m_Values[2].ToFloat();
		_outColor.a = m_Values[3].ToFloat();
		m_Values.SetEmpty();
	}

	return TRUE;
}

//------------------------------

Bool	Loader_DAE::LoadFloat(const rapidxml::xml_node<>* _xmlNode, Float& _outValue)
{
	DAE_CHECK(_xmlNode );

	rapidxml::xml_node<>* value = _xmlNode->first_node("float");
	DAE_CHECK(value );

	Str data = value->value();
	_outValue = data.ToFloat();

	return TRUE;
}

//------------------------------

Bool	Loader_DAE::LoadLibGeometries(const rapidxml::xml_node<>* _xmlNode)
{
	// todo attrib 

	const rapidxml::xml_node<> *child = _xmlNode->first_node();
	while(child)
	{
		Name childName = child->name();

		if( childName == NAME(asset) )
		{
			DAE_CHECK(LoadAsset(child));
		}
		else if( childName == NAME(geometry) )
		{
			m_allGeometries.AddLastItem(GeometryDAE());
			GeometryDAE& geometry = m_allGeometries[m_allGeometries.GetCount()-1];
			DAE_CHECK(LoadGeometry(child,geometry));
		}
		else if( childName == NAME(extra) )
		{
			DAE_CHECK(LoadExtra(child));
		}
		else 
		{
			DAE_UNKNOWN(childName.GetStr().GetArray());
		}

		child = child->next_sibling();
	}

	return TRUE;
}

//------------------------------

Bool	Loader_DAE::LoadGeometry(const rapidxml::xml_node<>* _xmlNode, GeometryDAE& _outGeometry)
{
	const rapidxml::xml_node<> *child = _xmlNode->first_node();
	Bool bUnique = FALSE;

	Str id;
	DAE_CHECK(ReadParameterStr(_xmlNode,"id",id));
	_outGeometry.m_Name = id;
	
	while(child)
	{
		Name childName = child->name();

		if( childName == NAME(asset) )
		{
			LoadAsset(child);
		}
		else if( childName == NAME(convex_mesh) )
		{
			DEBUG_Require(!bUnique);
			bUnique = TRUE;
			DAE_UNSUPPORTED(convex_mesh);
		}
		else if( childName == NAME(mesh) )
		{
			DEBUG_Require(!bUnique);
			bUnique = TRUE;
			LoadMesh(child,_outGeometry.m_Mesh);
			_outGeometry.m_Type = GeometryDAE::TYPE_MESH;
		}
		else if( childName == NAME(spline) )
		{
			DEBUG_Require(!bUnique);
			bUnique = TRUE;
			DAE_UNSUPPORTED(spline);
		}
		else if( childName == NAME(extra) )
		{
			LoadExtra(child);
		}
		else 
		{
			DAE_UNKNOWN(childName.GetStr().GetArray());
		}

		child = child->next_sibling();
	}

	return TRUE;
}

//------------------------------

Bool	Loader_DAE::LoadMesh(const rapidxml::xml_node<>* _xmlNode, MeshDAE& _mesh)
{
	const rapidxml::xml_node<> *child = _xmlNode->first_node();
	ArrayOf<SourceDAE>	allSources;
	while(child)
	{
		Name childName = child->name();

		if( childName == NAME(source) )
		{
			SourceDAE& source = allSources.AddLastItem(SourceDAE());
			LoadSource(child,source);
		}
		else if( childName == NAME(vertices) )
		{
			// load inputs
			ArrayOf<InputDAE>	allInput;
			const rapidxml::xml_node<> *input = child->first_node();
			while(input)
			{
				Name inputName = input->name();
				if( inputName == NAME(input) )
				{
					Str sourceStr;
					DAE_CHECK(ReadParameterStr(input,"source",sourceStr));
					sourceStr = sourceStr.GetTextInsideRange(1,sourceStr.GetLength()-1); // remove #
					Str semantic;
					DAE_CHECK(ReadParameterStr(input,"semantic",semantic));
					Name sourceName = sourceStr;
					SourceDAE* pSourceInput = NULL;
					for(U32 s = 0; s < allSources.GetCount(); s++)
					{
						if( allSources[s].m_Name == sourceName )
						{
							InputDAE& input = allInput.AddLastItem(InputDAE());
							input.m_Semantic = semantic;
							input.m_Source = s;
							break;
						}
					}
				}
				else if( inputName == NAME(extra) )
				{
					LoadExtra(input);
				}

				input = input->next_sibling();
			}

			// create vertices sources
			if( allInput.GetCount() > 0 )
			{
				// create new source for vertices
				SourceDAE& verticesSource = allSources.AddLastItem(SourceDAE());
				Str id;
				DAE_CHECK(ReadParameterStr(child,"id",id));
				verticesSource.m_Name = id;
				verticesSource.m_stride = 0;
				for(U32 i=0; i<allInput.GetCount(); i++)
				{
					verticesSource.m_stride += allSources[allInput[i].m_Source].m_stride;
				}
				U32 count = allSources[allInput[0].m_Source].m_floatArray.GetCount() / allSources[allInput[0].m_Source].m_stride;
				for(U32 i=0; i<count; i++)
				{
					for(U32 j=0; j<allInput.GetCount(); j++)
					{
						SourceDAE* pCurSource = &allSources[allInput[j].m_Source];
						U32 firstIndex = i * pCurSource->m_stride;
						for(U32 k=0; k<pCurSource->m_stride; k++)
						{
							Float value = pCurSource->m_floatArray[firstIndex+k];
							verticesSource.m_floatArray.AddLastItem(value);
						}

						if( allInput[j].m_Semantic == NAME(POSITION))
						{
							VertexDAE& vertex = _mesh.m_Vertices.AddLastItem(VertexDAE());
							vertex.m_Position.x = pCurSource->m_floatArray[firstIndex];
							vertex.m_Position.y = pCurSource->m_floatArray[firstIndex+1];
							vertex.m_Position.z = pCurSource->m_floatArray[firstIndex+2];
						}
						else
						{
							DAE_UNKNOWN(allInput[j].m_Semantic.GetStr().GetArray());
						}
					}
				}
			}
		}
		else if( childName == NAME(lines) )
		{
			DAE_UNSUPPORTED(lines);
		}
		else if( childName == NAME(linestrips) )
		{
			DAE_UNSUPPORTED(linestrips);
		}
		else if( childName == NAME(polygons) )
		{
			DAE_UNSUPPORTED(polygons);
		}
		else if( childName == NAME(polylist) )
		{
			DAE_UNSUPPORTED(polylist);
		}
		else if( childName == NAME(triangles) )
		{
			// load inputs
			TrianglesListDAE&	list = _mesh.m_TrianglesLists.AddLastItem(TrianglesListDAE());
			ReadParameterStr(child, "material", list.m_Material);
			U32 triangleCount = 0;
			DAE_CHECK(ReadParameterU32(child, "count", &triangleCount));
			ArrayOf<InputDAE>	allInput;
			const rapidxml::xml_node<> *input = child->first_node();
			while(input)
			{
				Name inputName = input->name();
				if( inputName == NAME(input) )
				{
					SourceDAE* pSourceInput = NULL;
					Str sourceStr;
					DAE_CHECK(ReadParameterStr(input,"source",sourceStr));
					sourceStr = sourceStr.GetTextInsideRange(1,sourceStr.GetLength()-1);	// remove #
					Name sourceName = sourceStr;
					Str semantic;
					DAE_CHECK(ReadParameterStr(input,"semantic",semantic));
					for(U32 s = 0; s < allSources.GetCount(); s++)
					{
						if( allSources[s].m_Name == sourceName )
						{
							InputDAE& input = allInput.AddLastItem(InputDAE());
							input.m_Semantic = semantic;
							input.m_Source = s;
							break;
						}
					}
				}
				else if( inputName == NAME(p) )
				{
					Str data = input->value();
					m_Values.ReserveCount(triangleCount * 3 * allInput.GetCount());
					data.Split(" ", m_Values);
					DEBUG_Require( triangleCount  == (m_Values.GetCount() / allInput.GetCount()) / 3);
					for(U32 t=0; t<triangleCount; t++)
					{
						TriangleDAE& triangle = list.m_Triangles.AddLastItem(TriangleDAE());
						for(U32 v=0;v<3;v++)
						{
							VertexDAE& vertex = triangle.m_Points[v];
							S32 firstIdx = (t * 3 + v) * allInput.GetCount();
							for(U32 i=0;i<allInput.GetCount(); i++)
							{
								U32 idx = m_Values[firstIdx+i].ToU32();
								SourceDAE* pCurSource = &allSources[allInput[i].m_Source];
								S32 firstInputValue = idx * pCurSource->m_stride;
								if( allInput[i].m_Semantic == NAME(POSITION))
								{
									vertex.m_Position.x = pCurSource->m_floatArray[firstInputValue];
									vertex.m_Position.y = pCurSource->m_floatArray[firstInputValue+1];
									vertex.m_Position.z = pCurSource->m_floatArray[firstInputValue+2];
								}
								else if( allInput[i].m_Semantic == NAME(VERTEX))
								{
									vertex = _mesh.m_Vertices[idx];
									triangle.m_VertexId[v] = idx;
								}
								else if( allInput[i].m_Semantic == NAME(NORMAL))
								{
									vertex.m_Normal.x = pCurSource->m_floatArray[firstInputValue];
									vertex.m_Normal.y = pCurSource->m_floatArray[firstInputValue+1];
									vertex.m_Normal.z = pCurSource->m_floatArray[firstInputValue+2];
								}
								else if( allInput[i].m_Semantic == NAME(TEXCOORD))
								{
									vertex.m_TexCoord.x = pCurSource->m_floatArray[firstInputValue];
									vertex.m_TexCoord.y = pCurSource->m_floatArray[firstInputValue+1];
								}
								else
								{
									DAE_UNKNOWN(allInput[i].m_Semantic.GetStr().GetArray());
								}
							}
						}
					}
					m_Values.SetEmpty();
				}
				else if( inputName == NAME(extra))
				{
					LoadExtra(input);
				}

				input = input->next_sibling();
			}
		}
		else if( childName == NAME(trifans))
		{
			DAE_UNSUPPORTED(trifans);
		}
		else if( childName == NAME(tristrips))
		{
			DAE_UNSUPPORTED(tristrips);
		}
		else if( childName == NAME(extra))
		{
			LoadExtra(child);
		}
		else 
		{
			DAE_UNKNOWN(childName.GetStr().GetArray());
		}

		child = child->next_sibling();
	}

	return TRUE;
}

//------------------------------

Bool	Loader_DAE::LoadLibControllers(const rapidxml::xml_node<>* _xmlNode)
{
	// todo attrib 

	const rapidxml::xml_node<> *child = _xmlNode->first_node();
	while(child)
	{
		Name childName = child->name();

		if( childName == NAME(asset) )
		{
			DAE_CHECK(LoadAsset(child));
		}
		else if( childName == NAME(controller) )
		{
			m_allControllers.AddLastItem(ControllerDAE());
			ControllerDAE& controller = m_allControllers[m_allControllers.GetCount()-1];
			DAE_CHECK(LoadController(child,controller));
		}
		else if( childName == NAME(extra) )
		{
			DAE_CHECK(LoadExtra(child));
		}
		else 
		{
			DAE_UNKNOWN(childName.GetStr().GetArray());
		}

		child = child->next_sibling();

	}
	return TRUE;
}

//------------------------------

Bool	Loader_DAE::LoadController(const rapidxml::xml_node<>* _xmlNode, ControllerDAE& _outController)
{
	const rapidxml::xml_node<> *child = _xmlNode->first_node();
	Bool bUnique = FALSE;

	Str id;
	DAE_CHECK(ReadParameterStr(_xmlNode,"id",id));
	_outController.m_Name = id;
	
	while(child)
	{
		Name childName = child->name();

		if( childName == NAME(asset) )
		{
			DAE_CHECK(LoadAsset(child));
		}
		else if( childName == NAME(skin) )
		{
			DEBUG_Require(!bUnique);
			bUnique = TRUE;
			_outController.m_Type = ControllerDAE::TYPE_SKIN;
			DAE_CHECK(LoadSkin(child,_outController.m_Skin));
		}
		else if( childName == NAME(morph) )
		{
			DEBUG_Require(!bUnique);
			bUnique = TRUE;
			DAE_UNSUPPORTED(morph);
		}
		else if( childName == NAME(extra) )
		{
			DAE_CHECK(LoadExtra(child));
		}
		else 
		{
			DAE_UNKNOWN(childName.GetStr().GetArray());
		}

		child = child->next_sibling();
	}

	return TRUE;
}

//------------------------------

Bool	Loader_DAE::LoadSkin(const rapidxml::xml_node<>* _xmlNode, SkinDAE& _outSkin)
{
	Str source;
	DAE_CHECK(ReadParameterStr(_xmlNode,"source",source));
	_outSkin.m_Geometry = source.GetTextInsideRange(1,source.GetLength()-1);	// remove #

	const rapidxml::xml_node<> *child = _xmlNode->first_node();
	ArrayOf<SourceDAE>	allSources;
	
	while(child)
	{
		Name childName = child->name();

		if( childName == NAME(asset) )
		{
			LoadAsset(child);
		}
		else if( childName == NAME(bind_shape_matrix) )
		{
			LoadMatrix(child,_outSkin.m_BindShapeMatrix);
		}
		else if( childName == NAME(source) )
		{
			allSources.AddLastItem(SourceDAE());
			SourceDAE& source = allSources[allSources.GetCount()-1];
			LoadSource(child,source);
		}
		else if( childName == NAME(joints) )
		{
			// load inputs
			ArrayOf<SourceDAE*> inputs;
			const rapidxml::xml_node<> *input = child->first_node();
			while(input)
			{
				Name inputName = input->name();
				if( inputName == NAME(input) )
				{
					SourceDAE* pSourceInput = NULL;
					Str sourceStr;
					DAE_CHECK(ReadParameterStr(input,"source",sourceStr));
					sourceStr = sourceStr.GetTextInsideRange(1,sourceStr.GetLength()-1);	// remove #
					Name sourceName = sourceStr;
					for(U32 s = 0; s < allSources.GetCount(); s++)
					{
						if( allSources[s].m_Name == sourceName )
						{
							pSourceInput = &allSources[s];
							break;
						}
					}

					if( pSourceInput )
					{
						inputs.AddLastItem( pSourceInput );
					}
				}
				else if( inputName == NAME(extra) )
				{
					LoadExtra(input);
				}

				input = input->next_sibling();
			}

			DEBUG_Require(inputs.GetCount() == 2);
			DEBUG_Require(inputs[0]->m_Type == SourceDAE::TYPE_NAMEARRAY);		// name
			DEBUG_Require(inputs[1]->m_Type == SourceDAE::TYPE_FLOATARRAY);	// matrix
			DEBUG_Require(inputs[0]->m_nameArray.GetCount() * 16 == inputs[1]->m_floatArray.GetCount());

			for(U32 i=0; i<inputs[0]->m_nameArray.GetCount(); i++)
			{
				JointDAE& joint = _outSkin.m_Joints.AddLastItem(JointDAE());
				joint.m_Name = inputs[0]->m_nameArray[i];
				joint.m_InvBindShapeMatrix.xx = inputs[1]->m_floatArray[i*16];
				joint.m_InvBindShapeMatrix.xy = inputs[1]->m_floatArray[i*16+1];
				joint.m_InvBindShapeMatrix.xz = inputs[1]->m_floatArray[i*16+2];
				joint.m_InvBindShapeMatrix.xw = inputs[1]->m_floatArray[i*16+3];
				joint.m_InvBindShapeMatrix.yx = inputs[1]->m_floatArray[i*16+4];
				joint.m_InvBindShapeMatrix.yy = inputs[1]->m_floatArray[i*16+5];
				joint.m_InvBindShapeMatrix.yz = inputs[1]->m_floatArray[i*16+6];
				joint.m_InvBindShapeMatrix.yw = inputs[1]->m_floatArray[i*16+7];
				joint.m_InvBindShapeMatrix.zx = inputs[1]->m_floatArray[i*16+8];
				joint.m_InvBindShapeMatrix.zy = inputs[1]->m_floatArray[i*16+9];
				joint.m_InvBindShapeMatrix.zz = inputs[1]->m_floatArray[i*16+10];
				joint.m_InvBindShapeMatrix.zw = inputs[1]->m_floatArray[i*16+11];
				joint.m_InvBindShapeMatrix.wx = inputs[1]->m_floatArray[i*16+12];
				joint.m_InvBindShapeMatrix.wy = inputs[1]->m_floatArray[i*16+13];
				joint.m_InvBindShapeMatrix.wz = inputs[1]->m_floatArray[i*16+14];
				joint.m_InvBindShapeMatrix.ww = inputs[1]->m_floatArray[i*16+15];
			}
		}

		else if( childName == NAME(vertex_weights) )
		{
			U32 count = 0;
			DAE_CHECK(ReadParameterU32(child,"count",&count));

			// load inputs
			ArrayOf<SourceDAE*> inputs;
			const rapidxml::xml_node<> *input = child->first_node();
			while(input)
			{
				Name inputName = input->name();
				if( inputName == NAME(input) )
				{
					SourceDAE* pSourceInput = NULL;
					Str sourceStr;
					DAE_CHECK(ReadParameterStr(input,"source",sourceStr));
					sourceStr = sourceStr.GetTextInsideRange(1,sourceStr.GetLength()-1);	// remove #
					Name sourceName = sourceStr;
					for(U32 s = 0; s < allSources.GetCount(); s++)
					{
						if( allSources[s].m_Name == sourceName )
						{
							pSourceInput = &allSources[s];
							break;
						}
					}

					if( pSourceInput )
					{
						inputs.AddLastItem( pSourceInput );
					}
				}
				else if( inputName == NAME(vcount) )
				{
					Str content = input->value();
					StrArray values;
					content.Split(" \n", m_Values);

					DAE_CHECK(values.GetCount() == count);
					_outSkin.m_VertexWeightCount = 0;
					for(U32 i=0; i<values.GetCount(); i++)
					{
						_outSkin.m_VertexWeight.AddLastItem(VertexWeightDAE());
						U32 currentCount = values[i].ToU32();
						_outSkin.m_VertexWeight[_outSkin.m_VertexWeight.GetCount()-1].m_Count = currentCount;
						_outSkin.m_VertexWeightCount += currentCount;
					}	

					m_Values.SetEmpty();
				}
				else if( inputName == NAME(v) )
				{
					Str content = input->value();
					StrArray values;
					content.Split(" \n",values);

					DAE_CHECK(values.GetCount() == _outSkin.m_VertexWeightCount * 2);
					DAE_CHECK(inputs.GetCount() == 2);

					S32 currentCount = 0;
					for(U32 i=0; i<_outSkin.m_VertexWeight.GetCount(); i++)
					{
						VertexWeightDAE& vertexWeight = _outSkin.m_VertexWeight[i];
						for(U32 v=0; v<vertexWeight.m_Count; v++)
						{
							U32 idx = values[2*currentCount].ToU32();
							vertexWeight.m_Joints.AddLastItem(inputs[0]->m_nameArray[idx]);
							idx = values[2*currentCount+1].ToU32();
							vertexWeight.m_Weight.AddLastItem(inputs[1]->m_floatArray[idx]);
							currentCount++;
						}
					}	
				}
				else if( inputName == NAME(extra) )
				{
					LoadExtra(input);
				}

				input = input->next_sibling();
			}


		}
		else if( childName == NAME(extra) )
		{
			LoadExtra(child);
		}
		else 
		{
			DAE_UNKNOWN(childName.GetStr().GetArray());
		}

		child = child->next_sibling();
	}

	return TRUE;
}

//------------------------------

Bool	Loader_DAE::LoadLibAnimations(const rapidxml::xml_node<>* _xmlNode)
{
	// todo attrib 

	const rapidxml::xml_node<> *child = _xmlNode->first_node();
	while(child)
	{
		Name childName = child->name();

		if( childName == NAME(asset) )
		{
			DAE_CHECK(LoadAsset(child));
		}
		else if( childName == NAME(animation) )
		{
			m_allAnimations.AddLastItem(AnimationDAE());
			AnimationDAE& animation = m_allAnimations[m_allAnimations.GetCount()-1];
			DAE_CHECK(LoadAnimation(child,animation));
		}
		else if( childName == NAME(extra) )
		{
			DAE_CHECK(LoadExtra(child));
		}
		else 
		{
			DAE_UNKNOWN(childName.GetStr().GetArray());
		}

		child = child->next_sibling();

	}
	return TRUE;
}

//------------------------------

Bool	Loader_DAE::LoadAnimation(const rapidxml::xml_node<>* _xmlNode, AnimationDAE& _outAnimation)
{
	const rapidxml::xml_node<> *child = _xmlNode->first_node();
	Bool bUnique = FALSE;

	Str id;
	if(ReadParameterStr(_xmlNode,"id",id))
		_outAnimation.m_Name = id;

	Str target;
	if(ReadParameterStr(_xmlNode,"name",target))
		_outAnimation.m_Target = target;
	
	ArrayOf<SourceDAE>	allSources;
	while(child)
	{
		Name childName = child->name();

		if( childName == NAME(asset) )
		{
			DAE_CHECK(LoadAsset(child));
		}
		else if( childName == NAME(source) )
		{
			allSources.AddLastItem(SourceDAE());
			SourceDAE& source = allSources[allSources.GetCount()-1];
			LoadSource(child,source);
		}
		else if( childName == NAME(sampler) )
		{
			// load inputs
			ArrayOf<InputDAE>	allInput;
			const rapidxml::xml_node<> *input = child->first_node();
			while(input)
			{
				Name inputName = input->name();
				if( inputName == NAME(input) )
				{
					Str sourceStr;
					DAE_CHECK(ReadParameterStr(input,"source",sourceStr));
					sourceStr = sourceStr.GetTextInsideRange(1,sourceStr.GetLength()-1); // remove #
					Str semantic;
					DAE_CHECK(ReadParameterStr(input,"semantic",semantic));
					Name sourceName = sourceStr;
					SourceDAE* pSourceInput = NULL;
					for(U32 s = 0; s < allSources.GetCount(); s++)
					{
						if( allSources[s].m_Name == sourceName )
						{
							allInput.AddLastItem(InputDAE());
							allInput[allInput.GetCount()-1].m_Semantic = semantic;
							allInput[allInput.GetCount()-1].m_Source = s;
							break;
						}
					}
				}

				input = input->next_sibling();
			}

			Str id;
			DAE_CHECK(ReadParameterStr(child,"id",id));
			_outAnimation.m_Sampler.m_Name = id;

			S32 count = allSources[allInput[0].m_Source].m_floatArray.GetCount();
			for(S32 i=0; i<count; i++)
			{
				_outAnimation.m_Sampler.m_Keys.AddLastItem(KeyDAE());
				KeyDAE& key = _outAnimation.m_Sampler.m_Keys[_outAnimation.m_Sampler.m_Keys.GetCount()-1];
				for(U32 j=0; j<allInput.GetCount(); j++)
				{
					SourceDAE* pCurSource = &allSources[allInput[j].m_Source];
					S32 firstInputValue = i * pCurSource->m_stride;
					if( allInput[j].m_Semantic == NAME(input))
					{
						key.m_Time = pCurSource->m_floatArray[firstInputValue];
					}
					else if( allInput[j].m_Semantic == NAME(OUTPUT))
					{
						key.m_Matrix.xx = pCurSource->m_floatArray[firstInputValue];
						key.m_Matrix.xy = pCurSource->m_floatArray[firstInputValue+1];
						key.m_Matrix.xz = pCurSource->m_floatArray[firstInputValue+2];
						key.m_Matrix.xw = pCurSource->m_floatArray[firstInputValue+3];
						key.m_Matrix.yx = pCurSource->m_floatArray[firstInputValue+4];
						key.m_Matrix.yy = pCurSource->m_floatArray[firstInputValue+5];
						key.m_Matrix.yz = pCurSource->m_floatArray[firstInputValue+6];
						key.m_Matrix.yw = pCurSource->m_floatArray[firstInputValue+7];
						key.m_Matrix.zx = pCurSource->m_floatArray[firstInputValue+8];
						key.m_Matrix.zy = pCurSource->m_floatArray[firstInputValue+9];
						key.m_Matrix.zz = pCurSource->m_floatArray[firstInputValue+10];
						key.m_Matrix.zw = pCurSource->m_floatArray[firstInputValue+11];
						key.m_Matrix.wx = pCurSource->m_floatArray[firstInputValue+12];
						key.m_Matrix.wy = pCurSource->m_floatArray[firstInputValue+13];
						key.m_Matrix.wz = pCurSource->m_floatArray[firstInputValue+14];
						key.m_Matrix.ww = pCurSource->m_floatArray[firstInputValue+15];
					}
					else if( allInput[j].m_Semantic == NAME(INTERPOLATION))
					{
						key.m_Interpolation = pCurSource->m_nameArray[firstInputValue];
					}
					else
					{
						DAE_UNKNOWN(allInput[j].m_Semantic.GetStr().GetArray());
					}
				}
			}
		}
		else if( childName == NAME(channel) )
		{
			Str source;
			DAE_CHECK(ReadParameterStr(child,"source",source));
			source = source.GetTextInsideRange(1,source.GetLength()-1); // remove #
			_outAnimation.m_Channel.m_Sampler = source;

			Str target;
			DAE_CHECK(ReadParameterStr(child,"target",target));
			_outAnimation.m_Channel.m_Target = target;

		}
		else if( childName == NAME(animation) )
		{
			_outAnimation.m_Animations.AddLastItem(AnimationDAE());
			AnimationDAE& animation = _outAnimation.m_Animations[_outAnimation.m_Animations.GetCount()-1];
			DAE_CHECK(LoadAnimation(child,animation));
		}
		else if( childName == NAME(extra) )
		{
			DAE_CHECK(LoadExtra(child));
		}
		else 
		{
			DAE_UNKNOWN(childName.GetStr().GetArray());
		}

		child = child->next_sibling();
	}

	return TRUE;
}

//------------------------------

Bool	Loader_DAE::LoadLibLights(const rapidxml::xml_node<>* _xmlNode)
{
	// todo 
	return TRUE;
}

//------------------------------

Bool	Loader_DAE::LoadLibNodes(const rapidxml::xml_node<>* _xmlNode)
{
	// todo attrib 

	const rapidxml::xml_node<> *child = _xmlNode->first_node();
	while(child)
	{
		Name childName = child->name();

		if( childName == NAME(asset) )
		{
			LoadAsset(child);
		}
		else if( childName == NAME(node) )
		{
			m_allNodes.AddLastItem(NodeDAE());
			NodeDAE& node = m_allNodes[m_allNodes.GetCount()-1];
			LoadNode(child,node);
		}
		else if( childName == NAME(extra) )
		{
			LoadExtra(child);
		}
		else 
		{
			DAE_UNKNOWN(childName.GetStr().GetArray());
		}

		child = child->next_sibling();

	}
	return TRUE;
}

//------------------------------

Bool	Loader_DAE::LoadNode(const rapidxml::xml_node<>* _xmlNode, NodeDAE& _outNode)
{
	_outNode.m_Type = NodeDAE::TYPE_NODE;
	Str type;
	if( ReadParameterStr(_xmlNode,"type",type) && type == "JOINT" )
	{
		_outNode.m_Type = NodeDAE::TYPE_JOINT;
	}

	Str id;
	if(ReadParameterStr(_xmlNode,"id",id))
		_outNode.m_Name = id;

	const rapidxml::xml_node<> *child = _xmlNode->first_node();
	while(child)
	{
		Name childName = child->name();

		if( childName == NAME(asset) )
		{
			LoadAsset(child);
		}
		else if( childName == NAME(lookat) )
		{
			DAE_UNSUPPORTED(lookat);
		}
		else if( childName == NAME(matrix) )
		{
			LoadMatrix(child,_outNode.m_Matrix);
		}
		else if( childName == NAME(rotate) )
		{
			DAE_UNSUPPORTED(rotate);
		}
		else if( childName == NAME(scale) )
		{
			DAE_UNSUPPORTED(scale);
		}
		else if( childName == NAME(skew) )
		{
			DAE_UNSUPPORTED(skew);
		}
		else if( childName == NAME(translate) )
		{
			DAE_UNSUPPORTED(translate);
		}
		else if( childName == NAME(instance_camera) )
		{
			DAE_UNSUPPORTED(instance_camera);
		}
		else if( childName == NAME(instance_controller) )
		{
			DAE_CHECK(ReadParameterStr(child,"url",_outNode.m_Controller));
			_outNode.m_Controller = _outNode.m_Controller.GetTextInsideRange(1,_outNode.m_Controller.GetLength()-1); // remove #
		}
		else if( childName == NAME(instance_geometry) )
		{
			DAE_CHECK(ReadParameterStr(child,"url",_outNode.m_Geometry));
			_outNode.m_Geometry = _outNode.m_Geometry.GetTextInsideRange(1,_outNode.m_Geometry.GetLength()-1); // remove #

			rapidxml::xml_node<>* bind_mat = child->first_node("bind_material");
			if( bind_mat )
			{
				rapidxml::xml_node<>* technique = bind_mat->first_node("technique_common");
				if( technique )
				{
					rapidxml::xml_node<>* material = technique->first_node("instance_material");
					if( material )
					{
						DAE_CHECK(ReadParameterStr(material,"target",_outNode.m_Material));
						_outNode.m_Material = _outNode.m_Material.GetTextInsideRange(1,_outNode.m_Material.GetLength()-1); // remove #
					}
				}
			}
		}
		else if( childName == NAME(instance_light) )
		{
			//todo
		}
		else if( childName == NAME(instance_node) )
		{
			DAE_UNSUPPORTED(instance_node);
		}
		else if( childName == NAME(node) )
		{
			_outNode.m_Nodes.AddLastItem(NodeDAE());
			NodeDAE& node = _outNode.m_Nodes[_outNode.m_Nodes.GetCount()-1];
			LoadNode(child,node);
		}
		else if( childName == NAME(extra) )
		{
			LoadExtra(child);
		}
		else 
		{
			DAE_UNKNOWN(childName.GetStr().GetArray());
		}

		child = child->next_sibling();

	}
	return TRUE;
}

//------------------------------

Bool	Loader_DAE::LoadLibVisualScenes(const rapidxml::xml_node<>* _xmlNode)
{
	// todo attrib 

	const rapidxml::xml_node<> *child = _xmlNode->first_node();
	while(child)
	{
		Name childName = child->name();

		if( childName == NAME(asset) )
		{
			LoadAsset(child);
		}
		else if( childName == NAME(visual_scene) )
		{
			m_allVisualScene.AddLastItem(VisualSceneDAE());
			VisualSceneDAE& visualScene = m_allVisualScene[m_allVisualScene.GetCount()-1];
			LoadVisualScene(child,visualScene);
		}
		else if( childName == NAME(extra) )
		{
			LoadExtra(child);
		}

		child = child->next_sibling();

	}

	return TRUE;
}

//------------------------------

Bool	Loader_DAE::LoadVisualScene(const rapidxml::xml_node<>* _xmlNode, VisualSceneDAE& _outVisualScene)
{
	Str id;
	DAE_CHECK(ReadParameterStr(_xmlNode,"id",id));
	_outVisualScene.m_Name = id;

	const rapidxml::xml_node<> *child = _xmlNode->first_node();
	while(child)
	{
		Name childName = child->name();

		if( childName == NAME(asset) )
		{
			LoadAsset(child);
		}
		else if( childName == NAME(node) )
		{
			_outVisualScene.m_Nodes.AddLastItem(NodeDAE());
			NodeDAE& node = _outVisualScene.m_Nodes[_outVisualScene.m_Nodes.GetCount()-1];
			LoadNode(child,node);
		}
		else if( childName == NAME(extra) )
		{
			LoadExtra(child);
		}
		else 
		{
			DAE_UNKNOWN(childName.GetStr().GetArray());
		}

		child = child->next_sibling();

	}
	return TRUE;
}

//------------------------------

Bool	Loader_DAE::LoadSource(const rapidxml::xml_node<>* _xmlNode, SourceDAE& _outSource)
{
	const rapidxml::xml_node<> *child = _xmlNode->first_node();
	Bool bUnique = FALSE;
	Str id;
	DAE_CHECK(ReadParameterStr(_xmlNode,"id",id));
	_outSource.m_Name = id;
	_outSource.m_stride = 1;
	while(child)
	{
		Name childName = child->name();

		if( childName == NAME(asset) )
		{
			LoadAsset(child);
		}
		else if( childName == NAME(IDREF_array) )
		{
			DEBUG_Require(!bUnique);
			bUnique = TRUE;
			DAE_UNSUPPORTED(IDREF_array);
		}
		else if( childName == NAME(Name_array) )
		{
			DEBUG_Require(!bUnique);
			bUnique = TRUE;
			DAE_CHECK(LoadNameArray(child,_outSource.m_nameArray) );

			_outSource.m_Type = SourceDAE::TYPE_NAMEARRAY;
		}
		else if( childName == NAME(bool_array) )
		{
			DEBUG_Require(!bUnique);
			bUnique = TRUE;
			DAE_UNSUPPORTED(bool_array);
		}
		else if( childName == NAME(float_array) )
		{
			DEBUG_Require(!bUnique);
			bUnique = TRUE;
			DAE_CHECK(LoadFloatArray(child,_outSource.m_floatArray) );

			_outSource.m_Type = SourceDAE::TYPE_FLOATARRAY;
		}
		else if( childName == NAME(int_array) )
		{
			DEBUG_Require(!bUnique);
			bUnique = TRUE;
			DAE_UNSUPPORTED(int_array);
		}
 		else if( childName == NAME(technique_common) )
		{
			rapidxml::xml_node<>* accessor = child->first_node("accessor");
			if( accessor )
			{
				ReadParameterU32(accessor,"stride",&_outSource.m_stride);
			}
		}
		else if( childName == NAME(technique) )
		{
			DAE_UNSUPPORTED(technique);
		}
		else 
		{
			DAE_UNKNOWN(childName.GetStr().GetArray());
		}

		child = child->next_sibling();

	}
	return TRUE;
}

//------------------------------

Bool	Loader_DAE::LoadNameArray(const rapidxml::xml_node<>* _xmlNode, StrArray& _outArray)
{
	U32 count = 0;
	DAE_CHECK(ReadParameterU32(_xmlNode,"count",&count) );

	Str content = _xmlNode->value();
	_outArray.ReserveCount(count);
	content.Split(" \n", _outArray);

	DAE_CHECK(_outArray.GetCount() == count );

	return TRUE;
}

//------------------------------

Bool	Loader_DAE::LoadFloatArray(const rapidxml::xml_node<>* _xmlNode, FloatArray& _outArray)
{
	U32 count = 0;
	DAE_CHECK(ReadParameterU32(_xmlNode,"count",&count) );

	Str content = _xmlNode->value();
	m_Values.ReserveCount(count);
	content.Split(" \n", m_Values);

	DAE_CHECK(m_Values.GetCount() == count );

	_outArray.ReserveCount(count);
	for(U32 i=0; i<m_Values.GetCount(); i++)
		_outArray.AddLastItem(m_Values[i].ToFloat());

	m_Values.SetEmpty();

	return TRUE;
}

//------------------------------

Bool	Loader_DAE::LoadMatrix(const rapidxml::xml_node<>* _xmlNode, Mat44& _outMatrix)
{
	Str data = _xmlNode->value();
	m_Values.ReserveCount(16);
	data.Split(" ", m_Values);
	DAE_CHECK( (m_Values.GetCount() == 16));
	_outMatrix.xx = m_Values[0].ToFloat();
	_outMatrix.xy = m_Values[1].ToFloat();
	_outMatrix.xz = m_Values[2].ToFloat();
	_outMatrix.xw = m_Values[3].ToFloat();
	_outMatrix.yx = m_Values[4].ToFloat();
	_outMatrix.yy = m_Values[5].ToFloat();
	_outMatrix.yz = m_Values[6].ToFloat();
	_outMatrix.yw = m_Values[7].ToFloat();
	_outMatrix.zx = m_Values[8].ToFloat();
	_outMatrix.zy = m_Values[9].ToFloat();
	_outMatrix.zz = m_Values[10].ToFloat();
	_outMatrix.zw = m_Values[11].ToFloat();
	_outMatrix.wx = m_Values[12].ToFloat();
	_outMatrix.wy = m_Values[13].ToFloat();
	_outMatrix.wz = m_Values[14].ToFloat();
	_outMatrix.ww = m_Values[15].ToFloat();
	m_Values.SetEmpty();
	return TRUE;
}

//------------------------------

Bool	Loader_DAE::ReadParameterStr(const rapidxml::xml_node<>* _xmlNode, const Char* _attribute, Str& _outAttribute)
{
	rapidxml::xml_attribute<>* xmlAttrib = _xmlNode->first_attribute(_attribute);
	if( !xmlAttrib )
		return FALSE;
	
	_outAttribute = xmlAttrib->value();
	return TRUE;
}

//------------------------------

Bool	Loader_DAE::ReadParameterU32(const rapidxml::xml_node<>* _xmlNode, const Char* _attribute, U32* _outAttribute)
{
	rapidxml::xml_attribute<>* xmlAttrib = _xmlNode->first_attribute(_attribute);
	if( !xmlAttrib )
		return FALSE;
	
	*_outAttribute = (U32)Str(xmlAttrib->value()).GetInteger();
	return TRUE;
}

//------------------------------

Bool	Loader_DAE::GenerateResourceFromNode(const VisualSceneDAE& _scene, const NodeDAE& _node, ResourceRefArray& _outResources)
{
	if( _node.m_Controller.GetLength() > 0 )
	{
		GenerateResourceFromController(_scene,_node,_node.m_Controller,_node.m_Material,_outResources);
		GenerateAnimation(_outResources);
	}
	if( _node.m_Geometry.GetLength() > 0 )
	{
		GenerateResourceFromGeometry(_node.m_Geometry,_node.m_Material,_node.m_Matrix,_outResources);
	}

	// generate child nodes
	for(U32 n=0; n<_node.m_Nodes.GetCount(); n++)
	{
		DAE_CHECK(GenerateResourceFromNode(_scene,_node.m_Nodes[n],_outResources) );
	}

	return TRUE;
}

//-----------------------------------------------

Bool	Loader_DAE::GenerateResourceFromGeometry(const Str& _geometry, const Str& _material, const Mat44& _matrix, ResourceRefArray& _outResources)
{
	// find geometry
	const GeometryDAE* geometry = NULL;
	for(U32 g=0; g<m_allGeometries.GetCount(); g++)
	{
		if( m_allGeometries[g].m_Name == _geometry )
		{
			geometry = &m_allGeometries[g];
			break;
		}
	}

	// create & add resource
	if( geometry && geometry->m_Type == GeometryDAE::TYPE_MESH)
	{
		// try find mesh
		ResourceRef ref;
		Name resName = ResourceManager::GetInstance().ComputeResourceName<Mesh>(m_Path);
		for (ResourceRef& curRes : _outResources)
		{
			if (curRes->GetName() == resName)
			{
				ref = curRes;
			}
		}

		// if not, create new one
		if (ref.IsNull())
		{
			ref = ResourceManager::GetInstance().CreateNewResource<Mesh>(m_Path);
			_outResources.AddLastItem(ref);
		}
		::Mesh* mesh = (::Mesh*)ref.GetPtr();

		// creat vb&ib
		U32 nbBadNormal = 0;
		for(U32 l=0; l<geometry->m_Mesh.m_TrianglesLists.GetCount(); l++)
		{
			const TrianglesListDAE& list = geometry->m_Mesh.m_TrianglesLists[l];
			if (list.m_Triangles.GetCount() == 0)
				continue;

			S32 subMeshId = mesh->AddSubMesh();
			VertexBufferRef vb;
			IndexBufferRef ib;
			mesh->GetPrimitive(subMeshId)->Lock(vb,ib);
			for(U32 i=0; i<list.m_Triangles.GetCount(); i++)
			{
				const TriangleDAE& triangle = list.m_Triangles[i];
				for(S32 v=0; v<3; v++)
				{
					const VertexDAE& vertex = triangle.m_Points[v];
					if (vertex.m_Normal.GetLength() < Float_Eps)
					{
						nbBadNormal++;
					}

					Mat33 rotmat(_matrix);
					rotmat.Inverse();
					rotmat.Transpose();
					U16 newIdx = vb->AddLastVertex(_matrix*vertex.m_Position,vertex.m_TexCoord,rotmat*vertex.m_Normal);
					ib->m_Array.AddLastItem(newIdx);
				}
			}
			mesh->GetPrimitive(subMeshId)->Unlock();

			if( list.m_Material.GetLength() > 0 )
				GenerateMaterial(list.m_Material,*mesh->GetMaterial(subMeshId));
			else
				GenerateMaterial(_material,*mesh->GetMaterial(subMeshId));
		}

		if (nbBadNormal>0)
		{
			LOGGER_LogError("-- %d bad normals in mesh %s\n", nbBadNormal, m_Path.GetArray());
		}

		return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------

Bool	Loader_DAE::GenerateResourceFromController(const VisualSceneDAE& _scene, const NodeDAE& _node, const Str& _controller, const Str& _material, ResourceRefArray& _outResources)
{
	// find geometry
	const ControllerDAE* controller = NULL;
	for(U32 c=0; c<m_allControllers.GetCount(); c++)
	{
		if( m_allControllers[c].m_Name == _controller )
		{
			controller = &m_allControllers[c];
			break;
		}
	}

	if( controller && controller->m_Type == ControllerDAE::TYPE_SKIN)
	{
		ResourceRef ref = ResourceManager::GetInstance().CreateNewResource<CSkin>(m_Path);
		CSkin* skin = (CSkin*)ref.GetPtr();

		// find geometry
		const GeometryDAE* geometry = NULL;
		for(U32 g=0; g<m_allGeometries.GetCount(); g++)
		{
			if( m_allGeometries[g].m_Name == controller->m_Skin.m_Geometry )
			{
				geometry = &m_allGeometries[g];
				break;
			}
		}

		// creat vb&ib
		if (geometry)
		{
			for (U32 l = 0; l < geometry->m_Mesh.m_TrianglesLists.GetCount(); l++)
			{
				const TrianglesListDAE& list = geometry->m_Mesh.m_TrianglesLists[l];
				S32 subMeshId = skin->AddSubMesh();
				VertexBufferRef vb;
				IndexBufferRef ib;
				skin->GetPrimitive(subMeshId)->Lock(vb, ib);
				for (U32 i = 0; i < list.m_Triangles.GetCount(); i++)
				{
					const TriangleDAE& triangle = list.m_Triangles[i];
					for (S32 v = 0; v < 3; v++)
					{
						const VertexDAE& vertex = triangle.m_Points[v];
						U16 newIdx = vb->AddLastVertex(vertex.m_Position, vertex.m_TexCoord, vertex.m_Normal);
						ib->m_Array.AddLastItem(newIdx);
						skin->AddVertexMapping(subMeshId, newIdx, triangle.m_VertexId[v]);
					}
				}
				skin->GetPrimitive(subMeshId)->Unlock();

				if (list.m_Material.GetLength() > 0)
					GenerateMaterial(list.m_Material, *skin->GetMaterial(subMeshId));
				else
					GenerateMaterial(_material, *skin->GetMaterial(subMeshId));
			}

			// create skeleton
			GenerateSkeletonFromNodes(_scene, *controller, *skin, &_node);
			skin->SetBindShapeMatrix(controller->m_Skin.m_BindShapeMatrix);

			// create vertex weight
			for (U32 v = 0; v < controller->m_Skin.m_VertexWeight.GetCount(); v++)
			{
				const VertexWeightDAE& vweight = controller->m_Skin.m_VertexWeight[v];
				for (U32 j = 0; j < vweight.m_Count; j++)
				{
					for (S32 l = 0; l < skin->GetNbSubMesh(); l++)
					{
						skin->AddSkinnedVertex(l, v, geometry->m_Mesh.m_Vertices[v].m_Position, vweight.m_Joints[j], vweight.m_Weight[j]);
					}
				}
			}
		}

		_outResources.AddLastItem(ref);
		return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------

Bool	Loader_DAE::GenerateAnimation(ResourceRefArray& _outResources)
{
	ResourceRef ref = ResourceManager::GetInstance().CreateNewResource<CAnimation>(m_Path);
	CAnimation* animation = (CAnimation*)ref.GetPtr();

	Bool bOk = FALSE;
	for(U32 a=0; a<m_allAnimations.GetCount(); a++)
	{
		const AnimationDAE& mainAnimation = m_allAnimations[a];
		for(U32 i=0; i<mainAnimation.m_Animations.GetCount(); i++)
		{
			const AnimationDAE& channelAnimation = mainAnimation.m_Animations[i];
			Name channelName = mainAnimation.m_Target;
			for(U32 j=0; j<channelAnimation.m_Sampler.m_Keys.GetCount();j++)
			{
				const KeyDAE& key = channelAnimation.m_Sampler.m_Keys[j];
				animation->AddKey(channelName,key.m_Time,key.m_Matrix);
				bOk = TRUE;
			}
		}
	}

	if( bOk )
	{
		_outResources.AddLastItem(ref);
		return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------

Bool	Loader_DAE::GenerateMaterial(const Str& _materialStr, ::Material& _material)
{
	if( _materialStr.GetLength() > 0 )
	{
		//find effect
		EffectDAE* effect = NULL;
		Name materialName = _materialStr;
		for(U32 m=0; m<m_allMaterials.GetCount(); m++)
		{
			if( m_allMaterials[m].m_Name == materialName )
			{
				for(U32 e=0; e<m_allEffects.GetCount(); e++)
				{
					if( m_allEffects[e].m_Name == m_allMaterials[m].m_Effect )
					{
						effect = &m_allEffects[e];
					}
				}
				break;
			}
		}

		if( effect )
		{
			_material.SetDiffuseColor(effect->m_Diffuse);
			_material.SetEmissiveColor(effect->m_Emissive);
			for(U32 i=0; i<m_allImages.GetCount(); i++)
			{
				if (m_allImages[i].m_Type == ImageDAE::TYPE_FROMPATH)
				{
					if( m_allImages[i].m_Name == effect->m_DiffuseTexture)
					{
						Str path = m_allImages[i].m_Path;
						U32 idx = path.GetLastIndexOfCharacter('\\');
						path = path.GetTextInsideRange(idx+1,path.GetLength()-1);
						_material.LoadTexture(Str("textures/mesh/")+path);
						if (_material.GetTexture())
							_material.GetTexture()->SetWrapping(Texture::TEXTUREWRAP_Repeat);
					}

					if (m_allImages[i].m_Name == effect->m_EmissiveTexture)
					{
						Str path = m_allImages[i].m_Path;
						U32 idx = path.GetLastIndexOfCharacter('\\');
						path = path.GetTextInsideRange(idx + 1, path.GetLength() - 1);
						_material.LoadEmissiveTexture(Str("textures/mesh/") + path);
						if (_material.GetEmissiveTexture())
							_material.GetEmissiveTexture()->SetWrapping(Texture::TEXTUREWRAP_Repeat);
					}
				}
			}

			return TRUE;
		}
	}

	return FALSE;
}

//-----------------------------------------------

Bool	Loader_DAE::GenerateSkeletonFromNodes(const VisualSceneDAE& _scene, const ControllerDAE& _controller, CSkin& _skin, const NodeDAE* _avoidNode, const NodeDAE* _parent/*= NULL*/)
{
	Name parentName = Name::Null;
	const ArrayOf<NodeDAE>* pNodes = &(_scene.m_Nodes);
	if( _parent )
	{
		pNodes = &(_parent->m_Nodes);
		parentName = _parent->m_Name;
	}	

	for(U32 n=0; n<pNodes->GetCount(); n++)
	{
		const NodeDAE& node = (*pNodes)[n];
		//if( node.m_Type == Node::TYPE_JOINT ")
		if( &node != _avoidNode )
		{
			{
				CBone* pBone = _skin.GetSkel().AddBone(node.m_Name,node.m_Matrix,parentName);

				// find joint
				for(U32 i=0;i<_controller.m_Skin.m_Joints.GetCount(); i++)
				{
					if( _controller.m_Skin.m_Joints[i].m_Name == node.m_Name )
					{
						pBone->SetInvShapeMatrix(_controller.m_Skin.m_Joints[i].m_InvBindShapeMatrix);
						break;
					}
				}
			}

			for(U32 i=0; i<node.m_Nodes.GetCount(); i++)
			{
				GenerateSkeletonFromNodes(_scene,_controller,_skin,_avoidNode,&node);
			}
		}
	}

	return FALSE;
}