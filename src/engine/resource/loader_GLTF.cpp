#include "loader_GLTF.h"

#include "file/base_file.h"
#include <core/name_static.h>
#include "3d/mesh.h"

#include "platform/platform_win32.h"
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_IMPLEMENTATION
#include INCLUDE_LIB(LIB_TINYGLTF, tiny_gltf.h)

DEFINE_LOADER_CLASS(GLTF);

Loader_GLTF::Loader_GLTF()
{
}

//-----------------------------------------------

Loader_GLTF::~Loader_GLTF()
{
}

//-----------------------------------------------

// Declaration of default filesystem callbacks

bool FileExists(const std::string& _filename, void* _user_data)
{
	OldFile file(_filename.c_str(), OldFile::USE_TYPE_ReadBuffer);
	return file.Exists();
}

std::string ExpandFilePath(const std::string& _filepath, void* _user_data)
{
	Loader_GLTF* pLoader = (Loader_GLTF*)_user_data;
	
	std::string directory = pLoader->GetPath().GetDirectory();
	return directory + _filepath;
}

bool ReadWholeFile(std::vector<unsigned char>* _out, std::string* _err, const std::string& _filepath, void* _user_data)
{
	OldFile file(_filepath.c_str(), OldFile::USE_TYPE_ReadBuffer);
	file.Open();
	if (!file.IsOpen())
		return FALSE;

	_out->resize(file.GetBufferSize());
	memcpy(&_out->at(0), file.GetBuffer(), file.GetBufferSize());
	return TRUE;
}

bool WriteWholeFile(std::string* _err, const std::string& _filepath, const std::vector<unsigned char>& _contents, void* _user_data)
{
	return FALSE;
}

bool LoadImageData(tinygltf::Image* _image, const int _image_idx, std::string* _err, std::string* _warn, int _req_width, int _req_height, const unsigned char* _bytes, int size, void* _user_data) 
{
	return FALSE;
}

struct Buffer
{
	tinygltf::Model*		m_pModel = NULL;
	tinygltf::Accessor*		m_pAccessor = NULL;
	tinygltf::BufferView*	m_pBufferView = NULL;
	tinygltf::Buffer*		m_pBuffer = NULL;

	void SetFrom(tinygltf::Model* _pModel, int _accIdx)
	{
		m_pModel = _pModel;
		m_pAccessor = &_pModel->accessors[_accIdx];
		m_pBufferView = &_pModel->bufferViews[m_pAccessor->bufferView];
		m_pBuffer = &_pModel->buffers[m_pBufferView->buffer];
	}

	Bool IsValid() const
	{
		return m_pAccessor != NULL;
	}

	template<typename __TYPE>
	__TYPE Get(U32 _idx) const
	{
		const U8* pData = (const U8*)m_pBuffer->data.data();
		pData += m_pBufferView->byteOffset;
		pData += m_pAccessor->byteOffset;
		if(m_pBufferView->byteStride)
			pData += _idx* m_pBufferView->byteStride;
		else
			pData += _idx * sizeof(__TYPE);
		return *(const __TYPE*)pData;
	}
};

//-----------------------------------------------

Bool Loader_GLTF::LoadToResources(const Str& _path, ResourceRefArray& _outResources)
{
	m_Path = _path;

	OldFile file(_path, OldFile::USE_TYPE_ReadBuffer);
	file.Open();
	if (!file.IsOpen())
		return FALSE;

	tinygltf::Model model;
	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;

	tinygltf::FsCallbacks callbacks;
	callbacks.FileExists = &FileExists;
	callbacks.ExpandFilePath = &ExpandFilePath;
	callbacks.ReadWholeFile = &ReadWholeFile;
	callbacks.WriteWholeFile = &WriteWholeFile;
	callbacks.user_data = this;
	loader.SetFsCallbacks(callbacks);
	loader.SetImageLoader(&LoadImageData, this);

	if (!loader.LoadASCIIFromString(&model, &err, &warn, file.GetBuffer(), file.GetBufferSize(),std::string()))
	{
		if (!err.empty()) 
		{
			LOGGER_LogError("%s\n", err.c_str());
		}
		LOGGER_LogError("Fail to load GLTF file : %s\n", _path.GetArray());
		return FALSE;
	}

	if (!warn.empty()) 
	{
		LOGGER_LogWarning("%s\n",warn.c_str());
	}

	file.Close();

	for (tinygltf::Node& n : model.nodes)
	{
		Vec3f vTranslation = Vec3f::Zero;
		if (n.translation.size() == 3)
			vTranslation.Set((Float)n.translation[0], (Float)n.translation[1], (Float)n.translation[2]);
		Quat qRotation = Quat::Identity;
		if (n.rotation.size() == 4)
		{
			qRotation.x = (Float)n.rotation[0];
			qRotation.y = (Float)n.rotation[1];
			qRotation.z = (Float)n.rotation[2];
			qRotation.w = (Float)n.rotation[3];
		}
		Vec3f vScale = Vec3f::One;
		if(n.scale.size()==3)
			vScale.Set((Float)n.scale[0], (Float)n.scale[1], (Float)n.scale[2]);
		
		Mat44 matrix(vTranslation, qRotation, vScale);
		
		for (tinygltf::Mesh& m : model.meshes)
		{
			ResourceRef	ref = ResourceManager::GetInstance().CreateNewResource<Mesh>(m_Path);
			_outResources.AddLastItem(ref);

			Mesh* pMesh = (Mesh*)ref.GetPtr();

			for (tinygltf::Primitive& p : m.primitives)
			{
				DEBUG_RequireMessage(p.mode == TINYGLTF_MODE_TRIANGLES, "Only triangle list supported right now");
				S32 subMeshId = pMesh->AddSubMesh();
				VertexBufferRef vb;
				IndexBufferRef ib;
				Primitive3D* pPrimitive = pMesh->GetPrimitive(subMeshId);
				pPrimitive->Lock(vb, ib);

				// VERTICES
				Buffer positionBuffer;
				Buffer normalBuffer;
				Buffer texCoordBuffer;
				auto it = p.attributes.find("POSITION");
				if (it != p.attributes.end())
					positionBuffer.SetFrom(&model, it->second);

				tinygltf::Accessor* pNormalAcc = NULL;
				it = p.attributes.find("NORMAL");
				if (it != p.attributes.end())
					normalBuffer.SetFrom(&model, it->second);

				tinygltf::Accessor* pTexCoordAcc = NULL;
				it = p.attributes.find("TEXCOORD_0");
				if (it != p.attributes.end())
					texCoordBuffer.SetFrom(&model, it->second);

				DEBUG_RequireMessage(positionBuffer.IsValid(), "No POSITION, strange !");

				for (U32 v = 0; v < positionBuffer.m_pAccessor->count; v++)
				{
					Vec3f vPos = matrix * positionBuffer.Get<Vec3f>(v);
					Vec2f vTexCoord = Vec2f::Zero;
					if (texCoordBuffer.IsValid())
						vTexCoord = texCoordBuffer.Get<Vec2f>(v);
					Vec3f vNormal = Vec3f::YAxis;
					if (normalBuffer.IsValid())
						vNormal = qRotation * normalBuffer.Get<Vec3f>(v);

					vb->AddLastVertex(vPos, vTexCoord, vNormal);
				}

				// INDICES
				Buffer indiceBuffer;
				indiceBuffer.SetFrom(&model, p.indices);
				DEBUG_RequireMessage(indiceBuffer.IsValid(), "No INDICES, strange !");
				for (U32 i = 0; i < indiceBuffer.m_pAccessor->count; i++)
				{
					ib->m_Array.AddLastItem(indiceBuffer.Get<U16>(i));
				}

				pPrimitive->Unlock();

				// material
				tinygltf::Material& m = model.materials[p.material];
				Material* pMaterial = pMesh->GetMaterial(subMeshId);
				if(m.pbrMetallicRoughness.baseColorFactor.size() == 4)
					pMaterial->SetDiffuseColor(Color((Float)m.pbrMetallicRoughness.baseColorFactor[0], (Float)m.pbrMetallicRoughness.baseColorFactor[1], (Float)m.pbrMetallicRoughness.baseColorFactor[2], (Float)m.pbrMetallicRoughness.baseColorFactor[3]));
				if (m.pbrMetallicRoughness.baseColorTexture.index >= 0)
				{
					tinygltf::Texture& t = model.textures[m.pbrMetallicRoughness.baseColorTexture.index];
					tinygltf::Sampler& s = model.samplers[t.sampler];
					tinygltf::Image& i = model.images[t.source];
					Str path = "textures/mesh/";
					path += Str(i.uri.c_str()).GetFilename();
					pMaterial->LoadTexture(path);
					Texture::EWrapping wrap = Texture::TEXTUREWRAP_Repeat;
					if (s.wrapS == TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE)
						wrap = Texture::TEXTUREWRAP_Clamp;
					if (pMaterial->GetTexture())
							pMaterial->GetTexture()->SetWrapping(wrap);
				}

				if(m.emissiveFactor.size() == 3)
					pMaterial->SetEmissiveColor(Color((Float)m.emissiveFactor[0], (Float)m.emissiveFactor[1], (Float)m.emissiveFactor[2], 1.f));
				
				if (m.emissiveTexture.index >= 0)
				{
					tinygltf::Texture& t = model.textures[m.emissiveTexture.index];
					tinygltf::Sampler& s = model.samplers[t.sampler];
					tinygltf::Image& i = model.images[t.source];
					Str path = "textures/mesh/";
					path += Str(i.uri.c_str()).GetFilename();
					pMaterial->LoadEmissiveTexture(path);
					Texture::EWrapping wrap = Texture::TEXTUREWRAP_Repeat;
					if (s.wrapS == TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE)
						wrap = Texture::TEXTUREWRAP_Clamp;
					if (pMaterial->GetEmissiveTexture())
						pMaterial->GetEmissiveTexture()->SetWrapping(wrap);
				}
			}
		}
	}

	// load textures
	// directory = "textures/mesh/";


	

	return TRUE;
}
