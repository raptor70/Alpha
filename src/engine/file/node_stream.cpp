#include "node_stream.h"

#define RAPIDJSON_NOMEMBERITERATORCLASS
#undef max
#undef min
#include INCLUDE_LIB(LIB_RAPIDJSON, include/rapidjson/rapidjson.h)
#include INCLUDE_LIB(LIB_RAPIDJSON, include/rapidjson/document.h)
#include INCLUDE_LIB(LIB_RAPIDJSON, include/rapidjson/stringbuffer.h)
#include INCLUDE_LIB(LIB_RAPIDJSON, include/rapidjson/prettywriter.h)

#include "base_file.h"

struct SharedData
{
	rapidjson::Document*	m_pDoc = NULL;
	const NodeStream::AddUserLeafFunc*		m_pAddUserLeafFunc = NULL;
	const NodeStream::ReadUserLeafFunc*		m_pReadUserLeafFunc = NULL;
};

struct LeafData
{
	NodeStream*				m_pStream = NULL;
	rapidjson::Value*		m_pValue = NULL;
};

//----------------------------------

struct ToJSonValue
{
	ToJSonValue() {}
	ToJSonValue(Bool _v, rapidjson::Document::AllocatorType& _a) { m_Value.SetBool(_v); }
	ToJSonValue(Float _v, rapidjson::Document::AllocatorType& _a) { m_Value.SetFloat(_v); }
	ToJSonValue(S32 _v, rapidjson::Document::AllocatorType& _a) { m_Value.SetInt(_v); }
	ToJSonValue(const Char* _c, rapidjson::Document::AllocatorType& _a) { m_Value.SetString(_c, _a); }
	ToJSonValue(const Vec2f& _v, rapidjson::Document::AllocatorType& _a)
	{
		m_Value.SetArray();
		m_Value.PushBack(_v.x, _a);
		m_Value.PushBack(_v.y, _a);
	}
	ToJSonValue(const Vec3f& _v, rapidjson::Document::AllocatorType& _a)
	{
		m_Value.SetArray();
		m_Value.PushBack(_v.x, _a);
		m_Value.PushBack(_v.y, _a);
		m_Value.PushBack(_v.z, _a);
	}
	ToJSonValue(const Vec4f& _v, rapidjson::Document::AllocatorType& _a)
	{
		m_Value.SetArray();
		m_Value.PushBack(_v.x, _a);
		m_Value.PushBack(_v.y, _a);
		m_Value.PushBack(_v.z, _a);
		m_Value.PushBack(_v.w, _a);
	}
	ToJSonValue(const Quat& _q, rapidjson::Document::AllocatorType& _a)
	{
		m_Value.SetArray();
		m_Value.PushBack(_q.x, _a);
		m_Value.PushBack(_q.y, _a);
		m_Value.PushBack(_q.z, _a);
		m_Value.PushBack(_q.w, _a);
	}
	ToJSonValue(const Color& _c, rapidjson::Document::AllocatorType& _a)
	{
		m_Value.SetArray();
		m_Value.PushBack(ROUNDINT(_c.r * 255.f), _a);
		m_Value.PushBack(ROUNDINT(_c.g * 255.f), _a);
		m_Value.PushBack(ROUNDINT(_c.b * 255.f), _a);
		m_Value.PushBack(ROUNDINT(_c.a * 255.f), _a);
	}

	operator rapidjson::Value& () { return m_Value; }
	rapidjson::Value m_Value;
};

//----------------------------------

void	FromJsonValue(const rapidjson::Value& _v, Bool& _b) { _b = _v.GetBool(); }
void	FromJsonValue(const rapidjson::Value& _v, Float& _b) { _b = _v.GetFloat(); }
void	FromJsonValue(const rapidjson::Value& _v, S32& _b) { _b = _v.GetInt(); }
void	FromJsonValue(const rapidjson::Value& _v, Str& _b) { _b = Str(_v.GetString()); }
void	FromJsonValue(const rapidjson::Value& _v, Vec2f& _b) { _b = Vec2f(_v[0].GetFloat(), _v[1].GetFloat()); }
void	FromJsonValue(const rapidjson::Value& _v, Vec3f& _b) { _b = Vec3f(_v[0].GetFloat(), _v[1].GetFloat(), _v[2].GetFloat()); }
void	FromJsonValue(const rapidjson::Value& _v, Vec4f& _b) { _b = Vec4f(_v[0].GetFloat(), _v[1].GetFloat(), _v[2].GetFloat(), _v[3].GetFloat()); }
void	FromJsonValue(const rapidjson::Value& _v, Quat& _b) { _b = Quat(_v[0].GetFloat(), _v[1].GetFloat(), _v[2].GetFloat(), _v[3].GetFloat()); }
void	FromJsonValue(const rapidjson::Value& _v, Color& _b) { _b = Color((S32)_v[0].GetInt(), (S32)_v[1].GetInt(), (S32)_v[2].GetInt(), (S32)_v[3].GetInt()); }

//----------------------------------

template<typename T>
void NodeStream::Leaf::Private_AddLeaf(const Char* _leafID, const T& _leaf)
{
	LeafData* pLeafData = (LeafData*)m_pLeafData;
	SharedData* pSharedData = (SharedData*)pLeafData->m_pStream->m_pSharedData;
	auto& a = pSharedData->m_pDoc->GetAllocator();

	rapidjson::Value name;
	name.SetString(_leafID, a);
	pLeafData->m_pValue->AddMember(name, ToJSonValue(_leaf, a).m_Value, a);
}

//----------------------------------

template<>void NodeStream::Leaf::AddLeaf(const Char* _leafID, const Bool& _leaf) { Private_AddLeaf(_leafID, _leaf); }
template<>void NodeStream::Leaf::AddLeaf(const Char* _leafID, const Float& _leaf) { Private_AddLeaf(_leafID, _leaf); }
template<>void NodeStream::Leaf::AddLeaf(const Char* _leafID, const S32& _leaf) { Private_AddLeaf(_leafID, _leaf); }
template<>void NodeStream::Leaf::AddLeaf(const Char* _leafID, Char const* const& _leaf) { Private_AddLeaf(_leafID, _leaf); }
template<>void NodeStream::Leaf::AddLeaf(const Char* _leafID, const Str& _leaf) { Private_AddLeaf(_leafID, _leaf); }
template<>void NodeStream::Leaf::AddLeaf(const Char* _leafID, const Vec2f& _leaf) { Private_AddLeaf(_leafID, _leaf); }
template<>void NodeStream::Leaf::AddLeaf(const Char* _leafID, const Vec3f& _leaf) { Private_AddLeaf(_leafID, _leaf); }
template<>void NodeStream::Leaf::AddLeaf(const Char* _leafID, const Vec4f& _leaf) { Private_AddLeaf(_leafID, _leaf); }
template<>void NodeStream::Leaf::AddLeaf(const Char* _leafID, const Quat& _leaf) { Private_AddLeaf(_leafID, _leaf); }
template<>void NodeStream::Leaf::AddLeaf(const Char* _leafID, const Color& _leaf) { Private_AddLeaf(_leafID, _leaf); }

//----------------------------------

template<typename T>
Bool NodeStream::Leaf::Private_FindLeaf(const Char* _leafID, T& _value)
{
	LeafData* pLeafData = (LeafData*)m_pLeafData;
	auto it = pLeafData->m_pValue->FindMember(_leafID);
	if (it != pLeafData->m_pValue->MemberEnd())
	{
		FromJsonValue(it->value, _value);
		return TRUE;
	}
	return FALSE;
	
}

//----------------------------------

template<> Bool		NodeStream::Leaf::GetLeaf(const Char* _leafID) { Bool v; Private_FindLeaf(_leafID, v); return v; }
template<> Float	NodeStream::Leaf::GetLeaf(const Char* _leafID) { Float v; Private_FindLeaf(_leafID, v); return v; }
template<> S32		NodeStream::Leaf::GetLeaf(const Char* _leafID) { S32 v; Private_FindLeaf(_leafID, v); return v; }
template<> Str		NodeStream::Leaf::GetLeaf(const Char* _leafID) { Str v; Private_FindLeaf(_leafID, v); return v; }
template<> Vec2f	NodeStream::Leaf::GetLeaf(const Char* _leafID) { Vec2f v; Private_FindLeaf(_leafID, v); return v; }
template<> Vec3f	NodeStream::Leaf::GetLeaf(const Char* _leafID) { Vec3f v; Private_FindLeaf(_leafID, v); return v; }
template<> Vec4f	NodeStream::Leaf::GetLeaf(const Char* _leafID) { Vec4f v; Private_FindLeaf(_leafID, v); return v; }
template<> Quat		NodeStream::Leaf::GetLeaf(const Char* _leafID) { Quat v; Private_FindLeaf(_leafID, v); return v; }
template<> Color	NodeStream::Leaf::GetLeaf(const Char* _leafID) { Color v; Private_FindLeaf(_leafID, v); return v; }

//----------------------------------

template<> Bool		NodeStream::Leaf::FindLeaf(const Char* _leafID, Bool& _v) { return Private_FindLeaf(_leafID, _v); }
template<> Bool		NodeStream::Leaf::FindLeaf(const Char* _leafID, Float& _v) { return Private_FindLeaf(_leafID, _v); }
template<> Bool		NodeStream::Leaf::FindLeaf(const Char* _leafID, S32& _v)  { return Private_FindLeaf(_leafID, _v); }
template<> Bool		NodeStream::Leaf::FindLeaf(const Char* _leafID, Str& _v) { return Private_FindLeaf(_leafID, _v); }
template<> Bool		NodeStream::Leaf::FindLeaf(const Char* _leafID, Vec2f& _v) { return Private_FindLeaf(_leafID, _v); }
template<> Bool		NodeStream::Leaf::FindLeaf(const Char* _leafID, Vec3f& _v) { return Private_FindLeaf(_leafID, _v); }
template<> Bool		NodeStream::Leaf::FindLeaf(const Char* _leafID, Vec4f& _v) { return Private_FindLeaf(_leafID, _v); }
template<> Bool		NodeStream::Leaf::FindLeaf(const Char* _leafID, Quat& _v) { return Private_FindLeaf(_leafID, _v); }
template<> Bool		NodeStream::Leaf::FindLeaf(const Char* _leafID, Color& _v) { return Private_FindLeaf(_leafID, _v); }

//----------------------------------

void NodeStream::Leaf::AddUserLeaf(const Char* _leafID, Object* _pUserLeaf)
{
	LeafData* pLeafData = (LeafData*)m_pLeafData;
	SharedData* pSharedData = (SharedData*)pLeafData->m_pStream->m_pSharedData;

	rapidjson::Value value;
	value.SetObject();

	LeafData newLeafData;
	newLeafData.m_pStream = pLeafData->m_pStream;
	newLeafData.m_pValue = &value;
	Leaf newLeaf(&newLeafData);
	(*pSharedData->m_pAddUserLeafFunc)(newLeaf, _pUserLeaf);

	rapidjson::Value name;
	name.SetString(_leafID, pSharedData->m_pDoc->GetAllocator());
	pLeafData->m_pValue->AddMember(name, value, pSharedData->m_pDoc->GetAllocator());
}

//--------------------------------------------

void NodeStream::Leaf::ReadLeaves(const Char* _leafID, Object* _pUserLeaf)
{
	LeafData* pLeafData = (LeafData*)m_pLeafData;
	SharedData* pSharedData = (SharedData*)pLeafData->m_pStream->m_pSharedData;

	rapidjson::Value wantedName;
	wantedName.SetString(_leafID, pSharedData->m_pDoc->GetAllocator());
	for (rapidjson::Value::MemberIterator itr = pLeafData->m_pValue->MemberBegin(); itr != pLeafData->m_pValue->MemberEnd(); ++itr)
	{
		if (itr->name == wantedName)
		{
			LeafData newLeafData;
			newLeafData.m_pStream = pLeafData->m_pStream;
			newLeafData.m_pValue = &itr->value;
			Leaf newLeaf(&newLeafData);
			(*pSharedData->m_pReadUserLeafFunc)(newLeaf, _leafID, _pUserLeaf);
		}
	}
}

//--------------------------------------------

Bool NodeStream::Save(const Str& _path, const Char* _leafID, const AddUserLeafFunc& _addUserLeafFunc, Object* _pUserData)
{
	Timer profilerTimer;
	profilerTimer.Start();

	// prepare
	rapidjson::Document doc;
	doc.SetObject();

	// shared data
	SharedData sharedData;
	sharedData.m_pDoc = &doc;
	sharedData.m_pAddUserLeafFunc = &_addUserLeafFunc;
	m_pSharedData = &sharedData;

	// compute tree	
	LeafData leafData;
	leafData.m_pStream = this;
	leafData.m_pValue = &doc;
	Leaf root(&leafData);
	root.AddUserLeaf(_leafID, _pUserData);

	// reset shared data
	m_pSharedData = NULL;

	// write
	OldFile file(_path, OldFile::USE_TYPE_Write);
	file.Open(TRUE);
	if (file.IsOpen())
	{
		std::string s;
		rapidjson::StringBuffer buffer;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
		writer.SetFormatOptions(rapidjson::kFormatSingleLineArray);
		doc.Accept(writer);
		file.WriteByte(buffer.GetString(), (U32)buffer.GetSize());
	}
	file.Close();	

	//rapidxml::xml_document<> doc;
	//rapidxml::xml_node<>* world = doc.allocate_node(rapidxml::node_element, "World");
	//doc.append_node(world);

	//SaveEntitiesToXml(&doc, world, _pRoot, _saveRoot);
	//	
	//OldFile file(_path, OldFile::USE_TYPE_Write);
	//file.Open(TRUE);
	//if (file.IsOpen())
	//{
	//	std::string s;
	//	print(std::back_inserter(s), doc, 0);
	//	file.WriteByte(s.c_str(), (U32)s.length());
	//}
	//file.Close();

	LOGGER_Log("Save %s : %fs\n", _path.GetArray(), profilerTimer.GetSecondCount());

	return TRUE;
}

//--------------------------------------------

Bool NodeStream::Load(const Str& _path, const Char* _leafID, const ReadUserLeafFunc& _readUserLeafFunc, Object* _pUserData)
{
	Timer profilerTimer;
	profilerTimer.Start();

	OldFile file(_path, OldFile::USE_TYPE_ReadBuffer);
	file.Open();
	if (file.IsOpen())
	{
		Str data;
		if (file.ReadCompleteTextFile(data))
		{
			rapidjson::Document doc;
			doc.Parse(data.GetArray());
			DEBUG_Require(doc.IsObject());
			const rapidjson::Value& v = doc["Entity"];
			if (v.IsObject())
			{
				// shared data
				SharedData sharedData;
				sharedData.m_pDoc = &doc;
				sharedData.m_pReadUserLeafFunc = &_readUserLeafFunc;
				m_pSharedData = &sharedData;

				// compute tree	
				LeafData leafData;
				leafData.m_pStream = this;
				leafData.m_pValue = &doc;
				Leaf root(&leafData);
				root.ReadLeaves(_leafID, _pUserData);

				// reset shared data
				m_pSharedData = NULL;
			}
		}
	}
	file.Close();

	//OldFile file(_path, OldFile::USE_TYPE_ReadBuffer);
//file.Open();
//if (!file.IsOpen())
//	return;

//Str data;
//if (!file.ReadCompleteTextFile(data))
//	return;

//rapidxml::xml_document<> doc;
//doc.parse<0>((Char*)data.GetArray());

//rapidxml::xml_node<>* pWorld = doc.first_node();
//if (Name(pWorld->name()) == NAME(World))
//{
//	LoadEntitiesFromXml(pWorld, _root, _editable);
//}

	LOGGER_Log("Load %s : %fs\n", _path.GetArray(), profilerTimer.GetSecondCount());

	return TRUE;
}