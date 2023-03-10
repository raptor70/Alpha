#include "params_table.h"

#include "file/base_file.h"
#include "platform/platform_manager.h"

BEGIN_SCRIPT_CLASS(ParamsTable)
	SCRIPT_RESULT_METHOD1(GetValue,const Params*,Name)
	SCRIPT_RESULT_METHOD1(GetOrAddValue,const Params*,Name)
END_SCRIPT_CLASS

//------------------------------

ParamsTable& ParamsTable::operator=(const ParamsTable& _other)
{
	m_Values = _other.m_Values;
	m_bChanged = _other.m_bChanged;
	for (ParamHash::Iterator it = m_Values.GetFirst(); it != m_Values.GetLast(); it++)
	{
		m_ValuesPtr.AddLastItem(&(it.GetValue()));
	}
	
	return *this;
}

//------------------------------

Params*		ParamsTable::GetModifiableValue(const Name& _name)
{
	Params* pValue = &(m_Values[_name]);
	if (pValue)
	{
		m_bChanged = TRUE;
	}
	return pValue;
}

//------------------------------

const Params*		ParamsTable::GetValue(const Name& _name) const
{
	return m_Values.GetItem(_name);
}

//------------------------------

Params*		ParamsTable::GetOrAddValue(const Name& _name)
{
	Params* pValue = GetModifiableValue(_name);
	if(!pValue)
	{
		pValue = m_Values.AddItem(_name, Params(_name));
		m_ValuesPtr.AddLastItem(pValue);
	}

	m_bChanged = TRUE;
	return pValue;
}

//----------------------------------

#define APP_VERSION 1	// to define quelle version a été achetée
#define BUFFER_MAX_SIZE	1024*1024

void	ParamsTable::SaveToFile()
{
	LOGGER_Log("Start saving \n");
	U8*	buffer = NEW U8[BUFFER_MAX_SIZE];
	U8* pCur = buffer;
	U16 version = SAVE_VERSION;
	LOGGER_Log("-> Version %d\n",version);
	memcpy(pCur,&version,sizeof(U16)); pCur+= sizeof(U16);
	version = APP_VERSION;
	LOGGER_Log("-> App Version %d\n",version);
	memcpy(pCur,&version,sizeof(U16)); pCur+= sizeof(U16);
	U32 count = m_ValuesPtr.GetCount();
	LOGGER_Log("-> Values count =  %d\n",count);
	memcpy(pCur,&count,sizeof(U32)); pCur+= sizeof(U32);
	for(U32 i=0; i<m_ValuesPtr.GetCount(); i++ )
	{
		const Params& v = *(m_ValuesPtr[i]);
		LOGGER_Log("-> Value %s = (%f,%f,%f,%f) ou %s\n",v.GetName().GetStr().GetArray(),GetXYZW(v.GetVec4f()),v.GetStr().GetArray());
		memcpy(pCur,&(v.GetVec4f()),sizeof(Vec4f)); pCur+= sizeof(Vec4f);
		U32 length = v.GetStr().GetLength();
		memcpy(pCur,&length,sizeof(U32)); pCur+= sizeof(U32);
		memcpy(pCur,v.GetStr().GetArray(),v.GetStr().GetLength()); pCur+= v.GetStr().GetLength();
		length = v.GetName().GetStr().GetLength();
		memcpy(pCur,&length,sizeof(U32)); pCur+= sizeof(U32);
		memcpy(pCur,v.GetName().GetStr().GetArray(),v.GetName().GetStr().GetLength()); pCur+= v.GetName().GetStr().GetLength();
	}
	DEBUG_Require((pCur-buffer)<BUFFER_MAX_SIZE);

	// convert U8* to vector<uint8>
	std::vector< uint8_t >	data(buffer,pCur);
	LOGGER_Log("-> Data size %d\n",data.size());

	OldFile file(SAVE_FILE,OldFile::USE_TYPE(OldFile::USE_TYPE_Write | OldFile::USE_TYPE_Data));
	file.Open(TRUE); 
	LOGGER_Log("Save : Open %s\n",file.GetPath().GetArray());
	file.WriteS32(S32(pCur-buffer));
	file.WriteByte((const S8*)buffer,U32(pCur-buffer));
	file.Close();
	LOGGER_Log("Save : Close\n");
	if( file.Exists() )
	{
		LOGGER_Log("Save : Successfull\n");
	}
	else
	{
		LOGGER_Log("Save : Failed\n");
	}

	DELETE[] buffer;
}

//----------------------------------

void	ParamsTable::LoadFromFile(std::vector< uint8_t >* _data)
{
	LOGGER_Log("Start loading \n");
	std::vector< uint8_t >	data;

	OldFile file(SAVE_FILE,OldFile::USE_TYPE(OldFile::USE_TYPE_ReadBuffer | OldFile::USE_TYPE_Data));
	if( file.Exists() )
	{
		LOGGER_Log("Load : %s exist\n",file.GetPath().GetArray());
		file.Open(); 
		if( file.IsOpen() )
		{
			LOGGER_Log("Load : Open\n");
			S32 size = file.ReadS32();
			S8* rawdata = NEW S8[size];
			file.ReadByte(rawdata,size);
			data = std::vector< uint8_t >(rawdata,rawdata+ size);
			DELETE[] rawdata;
			file.Close();
			LOGGER_Log("Load : Close\n");
		}
	}

	if( data.size() == 0)
		return;

	LOGGER_Log("-> Data size %d\n",data.size());
	U32 idx = 0;

	U16 version = *((U16*)&(data[idx])); idx += sizeof(U16);
	LOGGER_Log("-> Version %d\n",version);
	if( version != SAVE_VERSION)
	{
		LOGGER_Log("Load : bad version %d\n",version);
		return;
	}

	version = *((U16*)&(data[idx])); idx += sizeof(U16);
	LOGGER_Log("-> App Version %d\n",version);
	
	U32 count = *((U32*)&(data[idx])); idx += sizeof(U32);
	LOGGER_Log("-> Values count =  %d\n",count);
	m_ValuesPtr.SetItemCount(count);
	for(U32 i=0; i<count; i++ )
	{
		Params v;
		Vec4f* vData = (Vec4f*)&(data[idx]);
		v.SetVec4f(*vData);
		idx += sizeof(Vec4f);

		U32 length = 0;
		memcpy(&length,&(data[idx]), sizeof(U32)); idx += sizeof(U32);
		S8Array str;
		str.SetItemCount(length+1);
		memcpy(str.GetPointer(),&(data[idx]),length); idx += length;
		str[length]=0;

		v.SetStr((const Char*)str.GetPointer());

		memcpy(&length,&(data[idx]), sizeof(U32)); idx += sizeof(U32);
		str.SetItemCount(length+1);
		memcpy(str.GetPointer(),&(data[idx]),length); idx += length;
		str[length]=0;
		v.SetName((const Char*)str.GetPointer());

		Params* pValue = m_Values.AddItem(v.GetName(), v);
		m_ValuesPtr[i] = pValue;

		LOGGER_Log("-> Value %s = (%f,%f,%f,%f) ou %s\n",v.GetName().GetStr().GetArray(),GetXYZW(v.GetVec4f()),v.GetStr().GetArray());
	}
}
