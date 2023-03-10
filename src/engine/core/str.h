#ifndef __TEXT_H__
    #define __TEXT_H__

#include "array_of_.h"

//------------------------------------------------------------

struct StrLowerUpperMapping
{
	StrLowerUpperMapping()
	{
		for (U32 i = 0; i <= 255; i++)
		{
			char delta = -32;// 'A' - 'a';
			if (i >= 'a' && i <= 'z')
			{
				m_UpperTab[i] = (char)i + delta;
				m_LowerTab[i] = (char)i;
			}
			else if (i >= 'A' && i <= 'Z')
			{
				m_UpperTab[i] = (char)i;
				m_LowerTab[i] = (char)i - delta;
			}
			else
			{
				m_UpperTab[i] = (char)i;
				m_LowerTab[i] = (char)i;
			}	
		}
	}

	char m_LowerTab[256];
	char m_UpperTab[256];
};

//------------------------------------------------------------

class Str;
typedef ArrayOf<Str> StrArray;

class Str
{
public:
	Str( void ) { m_size = 0; m_allocsize = 0; m_buffer = NULL; }
	Str(const Str & _other):Str(){CopyFromChar(_other.GetArray()); }
	Str(const char * _text):Str() { CopyFromChar(_text); }
	template<typename... Args>
	Str(const char* _text, Args... _args) :Str() { SetFrom(_text, _args...); }

	virtual ~Str( void ) 
	{ 
		SetEmpty(); 
	}
	Str & operator=( const Str & other ) { CopyFromChar( other.GetArray() );  return (*this); }

	Str & operator=( const char * text ) { CopyFromChar( text );  return (*this); }
	Bool operator==( const Str & other ) const;
	Bool operator!=(const Str & other) const { return !((*this) == other); }
	Bool operator==( const char * other ) const;
	Bool operator!=(const char * other) const { return !((*this) == other); }
	Str & operator+=( const Str & text );
	Str & operator+=( const char & character );
	Str operator+(const Str & text);
	operator Char*() const { return m_buffer; }

	const Char * GetArray(void) const { return m_buffer; }
	Char * GetModifiableArray( void ) const { return m_buffer; }
	U32 GetLength( void )const { return m_size; }
	Bool IsEmpty( void )const { return m_size == 0; }
	void SetEmpty( void );
	void Alloc(U32 _size);

	const Str GetWordAtIndex( const S32 index ) const;
	Float ToFloat( void ) const; 
	U32	ToU32() const;
	S32 GetInteger( void ) const;
	Bool IsInteger() const;
	Bool IsFloat() const;
	const Str GetTextInsideRange( const S32 first_index, const S32 second_index ) const;
	S32 GetFirstIndexOfCharacter( const char character ) const;
	S32 GetLastIndexOfCharacter( const char character ) const;
	S32 GetCharacterCount(const Char _char) const;
	Str	GetExtension() const;
	Str	GetFilename() const;
	Str	GetDirectory() const;
	S32	GetWordLastIndex(const Str& _subStr) const;
	void Replace(Char _from, Char _to);

	const Str& SetFrom(const char * text, ... );
	void ChangeLength(U32 _length, const char * _newtext);

	void Split(const Char* _separators, StrArray& _out) const;

	void ToUpper();
	void ToLower();

	static U32 GetLength( const Char * text );

private:
	void CopyFromChar( const char * text );

	Char*	m_buffer;
	U32		m_size;
	U32		m_allocsize;

public:
	static Str	Void;
	static StrLowerUpperMapping g_LowerUpperMapping;
};

#ifdef WIN32
	#define VSPrintf(_dest,_source,_args)	vsprintf_s(_dest,4096,_source, _args)
#elif ANDROID || IOS
	#define VSPrintf(_dest,_source,_args)	vsprintf(_dest,_source,_args)
#endif

#endif
