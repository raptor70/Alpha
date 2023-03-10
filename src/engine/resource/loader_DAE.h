#ifndef __LOADER_DAE_H__
#define __LOADER_DAE_H__

#include "loader.h"
#include "loader_XML.h"

class CSkin;

BEGIN_LOADER_CLASS_INHERITED(DAE,XML)
public:
	Loader_DAE();
	virtual ~Loader_DAE();

	virtual Bool LoadToResources(const Str& _path, ResourceRefArray& _outResources);
	virtual Bool LoadToResourcesFromXml(const rapidxml::xml_document<>& _xmlDoc, ResourceRefArray& _outResources);

protected:
	Bool	LoadAsset(const rapidxml::xml_node<>* _xmlNode);
	Bool	LoadExtra(const rapidxml::xml_node<>* _xmlNode);
	
	// TEXTURES
	struct ImageDAE
	{
		enum Type
		{
			TYPE_FROMPATH
		};
		Str			m_Path;
		Type		m_Type;
		Name		m_Name;
	};
	Bool	LoadLibImages(const rapidxml::xml_node<>* _xmlNode);
	Bool	LoadImage(const rapidxml::xml_node<>* _xmlNode, ImageDAE& _outImage);

	// MATERIAL
	struct MaterialDAE
	{
		Str		m_Effect;
		Name	m_Name;
	};
	Bool	LoadLibMaterials(const rapidxml::xml_node<>* _xmlNode);
	Bool	LoadMaterial(const rapidxml::xml_node<>* _xmlNode, MaterialDAE& _outMaterial);

	// EFFECTS
	struct EffectDAE
	{
		enum Type
		{
			TYPE_PHONG
		};

		Color	m_Emissive;
		Name	m_EmissiveTexture;
		Color	m_Ambient;
		Color	m_Diffuse;
		Name	m_DiffuseTexture;
		Color	m_Specular;
		Float	m_Shininess;
		Color	m_Reflective;
		Float	m_Reflectivity;
		Color	m_Transparent;
		Float	m_Transparency;
		Type	m_Type;
		Name	m_Name;

		EffectDAE()
		{
			m_Emissive = Color::Black;
			m_Ambient = Color::White;
			m_Diffuse = Color::White;
			m_Specular = Color::Black;
			m_Shininess = 0.f;
			m_Reflective = Color::Black;
			m_Reflectivity = 0.f;
			m_Transparent = Color::White;
			m_Transparency = 0.f;
			m_Type = TYPE_PHONG;
		}
	};
	Bool	LoadLibEffects(const rapidxml::xml_node<>* _xmlNode);
	Bool	LoadEffect(const rapidxml::xml_node<>* _xmlNode, EffectDAE& _outEffect);
	Bool	LoadColor(const rapidxml::xml_node<>* _xmlNode, Color& _outColor);
	Bool	LoadFloat(const rapidxml::xml_node<>* _xmlNode, Float& _outValue);

	// GEOMETRY
	struct VertexDAE
	{
		Vec3f		m_Position;
		Vec3f		m_Normal;
		Vec2f		m_TexCoord;
	};
	struct TriangleDAE
	{
		VertexDAE	m_Points[3];
		U32			m_VertexId[3];
	};
	struct TrianglesListDAE
	{
		Str						m_Material;
		ArrayOf<TriangleDAE>	m_Triangles;
	};
	struct MeshDAE
	{
		ArrayOf<VertexDAE>			m_Vertices;
		ArrayOf<TrianglesListDAE>	m_TrianglesLists;
	};
	struct GeometryDAE
	{
		enum Type
		{
			TYPE_MESH
		};

		MeshDAE			m_Mesh;
		Type			m_Type;
		Name			m_Name;
	};
	Bool	LoadLibGeometries(const rapidxml::xml_node<>* _xmlNode);
	Bool	LoadGeometry(const rapidxml::xml_node<>* _xmlNode, GeometryDAE& _outGeometry);
	Bool	LoadMesh(const rapidxml::xml_node<>* _xmlNode, MeshDAE& _mesh);

	// CONTROLLERS
	struct JointDAE
	{
		Mat44		m_InvBindShapeMatrix;
		Name		m_Name;
	};
	struct VertexWeightDAE
	{
		U32					m_Count;
		ArrayOf<Name>		m_Joints;
		FloatArray			m_Weight;
	};
	struct SkinDAE
	{
		Str							m_Geometry;
		Mat44						m_BindShapeMatrix;
		ArrayOf<JointDAE>			m_Joints;
		U32							m_VertexWeightCount;
		ArrayOf<VertexWeightDAE>	m_VertexWeight;
	};
	struct ControllerDAE
	{
		enum Type
		{
			TYPE_SKIN
		};
		SkinDAE			m_Skin;
		Type			m_Type;
		Name			m_Name;
	};
	Bool	LoadLibControllers(const rapidxml::xml_node<>* _xmlNode);
	Bool	LoadController(const rapidxml::xml_node<>* _xmlNode, ControllerDAE& _outController);
	Bool	LoadSkin(const rapidxml::xml_node<>* _xmlNode, SkinDAE& _outSkin);

	// ANIMATIONS
	struct KeyDAE
	{
		Float		m_Time;
		Mat44		m_Matrix;
		Name		m_Interpolation;
	};
	struct SamplerDAE
	{
		ArrayOf<KeyDAE>		m_Keys;
		Name				m_Name;
	};
	struct ChannelDAE
	{
		Name				m_Sampler;
		Str					m_Target;
	};
	struct AnimationDAE
	{
		Name				m_Name;
		Name				m_Target;
		SamplerDAE			m_Sampler;
		ChannelDAE			m_Channel;
		ArrayOf<AnimationDAE>	m_Animations;
	};
	Bool	LoadLibAnimations(const rapidxml::xml_node<>* _xmlNode);
	Bool	LoadAnimation(const rapidxml::xml_node<>* _xmlNode, AnimationDAE& _outAnimation);

	// HIERARCHY
	struct NodeDAE
	{
		enum Type
		{
			TYPE_NODE,
			TYPE_JOINT
		};
		Type				m_Type;
		Mat44				m_Matrix;			
		Str					m_Geometry;
		Str					m_Material;
		Str					m_Controller;
		Name				m_Name;
		ArrayOf<NodeDAE>	m_Nodes;
	};
	struct VisualSceneDAE
	{
		Name				m_Name;
		ArrayOf<NodeDAE>	m_Nodes;
	};
	Bool	LoadLibNodes(const rapidxml::xml_node<>* _xmlNode);
	Bool	LoadNode(const rapidxml::xml_node<>* _xmlNode, NodeDAE& _outNode);
	Bool	LoadLibVisualScenes(const rapidxml::xml_node<>* _xmlNode);
	Bool	LoadVisualScene(const rapidxml::xml_node<>* _xmlNode, VisualSceneDAE& _outVisualScene);

	// LIGHTS
	Bool	LoadLibLights(const rapidxml::xml_node<>* _xmlNode);

	// DATA
	struct InputDAE
	{
		U32				m_Source;
		Name			m_Semantic;
	};
	struct SourceDAE
	{
		enum Type
		{
			TYPE_NAMEARRAY,
			TYPE_FLOATARRAY
		};

		StrArray		m_nameArray;
		FloatArray		m_floatArray;
		U32				m_stride;
		Type			m_Type;
		Name			m_Name;
	};
	Bool	LoadSource(const rapidxml::xml_node<>* _xmlNode, SourceDAE& _outSource);
	Bool	LoadNameArray(const rapidxml::xml_node<>* _xmlNode, StrArray& _outArray);
	Bool	LoadFloatArray(const rapidxml::xml_node<>* _xmlNode, FloatArray& _outArray);
	Bool	LoadMatrix(const rapidxml::xml_node<>* _xmlNode, Mat44& _outMatrix);

	// HELPER
	Bool	ReadParameterStr(const rapidxml::xml_node<>* _xmlNode, const Char* _attribute, Str& _outAttribute);
	Bool	ReadParameterU32(const rapidxml::xml_node<>* _xmlNode, const Char* _attribute, U32* _outAttribute);

	// GENERATION
	Bool	LoadScene(const rapidxml::xml_node<>* _xmlNode, ResourceRefArray& _outResources);
	Bool	GenerateResourceFromNode(const VisualSceneDAE& _scene, const NodeDAE& _node, ResourceRefArray& _outResources);
	Bool	GenerateResourceFromGeometry(const Str& _geometry, const Str& _material, const Mat44& _matrix, ResourceRefArray& _outResources); 
	Bool	GenerateResourceFromController(const VisualSceneDAE& _scene, const NodeDAE& _node, const Str& _controller, const Str& _material, ResourceRefArray& _outResources);
	Bool	GenerateAnimation(ResourceRefArray& _outResources); 
	Bool	GenerateMaterial(const Str& _materialName, ::Material& _material);
	Bool	GenerateSkeletonFromNodes(const VisualSceneDAE& _scene, const ControllerDAE& _controller, CSkin& _skin, const NodeDAE* _avoidNode , const NodeDAE* _parent = NULL);

	ArrayOf<ImageDAE>			m_allImages;
	ArrayOf<EffectDAE>			m_allEffects;
	ArrayOf<MaterialDAE>		m_allMaterials;
	ArrayOf<GeometryDAE>		m_allGeometries;
	ArrayOf<ControllerDAE>		m_allControllers;
	ArrayOf<AnimationDAE>		m_allAnimations;
	ArrayOf<VisualSceneDAE>		m_allVisualScene;
	ArrayOf<NodeDAE>			m_allNodes;
	Str							m_Path;

	// optim cache
	StrArray				m_Values;

END_LOADER_CLASS

#endif

