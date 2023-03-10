#ifndef __NODE_STREAM_H__
#define __NODE_STREAM_H__

class NodeStream
{
public:
	class Leaf
	{
	public:
		Leaf(void* _pData) :m_pLeafData(_pData) {}

		template<typename T>
		void AddLeaf(const Char* _leafID, const T& _leaf);

		template<typename T>
		T	GetLeaf(const Char* _leafID);

		template<typename T>
		Bool FindLeaf(const Char* _leafID, T& _v);
		
		void AddUserLeaf(const Char* _leafID, Object* _pUserLeaf);
		void ReadLeaves(const Char* _leafID, Object* _pUserLeaf);

	private:
		template<typename T>
		void Private_AddLeaf(const Char* _leafID, const T& _leaf);

		template<typename T>
		Bool Private_FindLeaf(const Char* _leafID, T& _value);

		void*	m_pLeafData = NULL;
	};

	typedef std::function<void(Leaf&, Object*)> AddUserLeafFunc;
	typedef std::function<void(Leaf&, const Char*, Object* _pParent)> ReadUserLeafFunc;

	Bool Save(const Str& _path, const Char* _leafID, const AddUserLeafFunc& _addUserLeafFunc, Object* _pUserData);
	Bool Load(const Str& _path, const Char* _leafID, const ReadUserLeafFunc& _readUserLeafFunc, Object* _pUserData);

private:
	void* m_pSharedData = NULL;
};

#endif
