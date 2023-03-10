#include "str.h"

#include "debug/debug.h"

StrLowerUpperMapping Str::g_LowerUpperMapping;

//..

Bool Str::operator==( const Str & other ) const
{
    if( m_size == other.m_size && memcmp(m_buffer,other.m_buffer,m_size) == 0 )
	{
		return TRUE;
	}

    return FALSE;
}

//..

Bool Str::operator==( const char * other ) const
{
	if( m_size == GetLength(other) && memcmp(m_buffer,other,m_size) == 0 )
	{
		return TRUE;
	}

    return FALSE;
}

//..

Str & Str::operator +=( const Str & text )
{
    U32 new_character_count = m_size + text.m_size;
	char * new_character_table = NEW char[ new_character_count + 1 ];

	memcpy(new_character_table,m_buffer,m_size);
	memcpy(new_character_table+m_size,text.m_buffer,text.m_size+1);

	DELETE[] m_buffer;
    m_buffer = new_character_table;
    m_size = new_character_count;
	m_allocsize = new_character_count + 1;

    return *this;
}

//..

Str & Str::operator +=( const char & character )
{
	if( m_size + 1 > m_allocsize )
	{
		char * new_character_table = NEW char[ m_size + 2 ];
		memcpy(new_character_table,m_buffer,m_size);
		DELETE[] m_buffer;
		m_buffer = new_character_table;
		m_allocsize = m_size + 2;
	}
	
	m_buffer[ m_size ] = character;
	m_buffer[ m_size + 1 ] = '\0';
	m_size++;

	return *this;
}

//..

Str Str::operator+( const Str & text )
{
	Str current( *this );
	current += text;
	return current;
}

//..

U32 Str::GetLength( const Char * text )
{
	if( text)
		return U32(strlen(text));

	return 0;
}

//..

void Str::SetEmpty( void )
{
	if(m_allocsize > 0 )
	{
		DELETE[] m_buffer; 
		m_buffer = NULL; 
	}
	m_size = 0; 
	m_allocsize = 0;
}

//..

void Str::Alloc(U32 _size)
{
	if (_size > m_allocsize)
	{
		m_allocsize = _size;
		Char* new_buffer = NEW char[_size + 1];
		if (m_buffer)
		{
			memcpy(new_buffer, m_buffer, m_size + 1);
			DELETE[] m_buffer;
		}
		m_buffer = new_buffer;
	}
}

//..

const Str Str::GetWordAtIndex( const S32 index ) const
{
    Str word;
    S32 word_count = 0;

    // Empty Text
    if( m_size == 0 )
    {
        return *this;
    }

    //Advance to first word
    U32 character_index = 0;
    while( character_index < m_size && m_buffer[ character_index ] == ' '  )
    {
        character_index++;
    }

    //Move into table
    while( character_index < m_size )
    {
        if( m_buffer[ character_index ] == ' ' )
        {
            //Move to next word
            while( character_index < m_size && m_buffer[ character_index ] == ' '  )
            {
                character_index++;
            }

            word_count++;

            if( word_count > index )
            {
                break;
            }
        }
        else
        {
            if( word_count == index )
            {
                word += m_buffer[ character_index ];
            }

            character_index++;
        }
    }

    return word;
}

//..

Float Str::ToFloat( void ) const
{
    return (Float)atof( m_buffer );
}

//..

U32	Str::ToU32() const
{
	return (U32)atoi( m_buffer );
}

//..

S32 Str::GetInteger( void ) const
{
    return atoi( m_buffer );
}

Bool Str::IsInteger() const
{
	char* cur = m_buffer;
	while (*cur != 0)
	{
		char c = *cur;
		if (c <= '0' || c >= '9')
		{
			return FALSE;
		}
	}

	return TRUE;
}

//..

Bool Str::IsFloat() const
{
	char* cur = m_buffer;
	while (*cur != 0)
	{
		char c = *cur;
		if (c <= '0' || c >= '9')
		{
			if (c != '.')
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

//..

const Str Str::GetTextInsideRange( const S32 first_index, const S32 second_index ) const
{
    DEBUG_Require( first_index >= 0 );
    DEBUG_Require( second_index >= 0 );
    DEBUG_Require( first_index <= second_index );
    DEBUG_Require( first_index < (S32)m_size );
    DEBUG_Require( second_index < (S32)m_size );

	S32 char_count = second_index-first_index + 1 ;
	char * new_buffer = NEW char[ char_count + 1 ];
	memcpy(new_buffer,m_buffer+first_index,char_count);
	new_buffer[char_count] = '\0';
    Str out = Str(new_buffer);
	DELETE[] new_buffer;
	return out;
}

//..

S32 Str::GetFirstIndexOfCharacter( const char character ) const
{
    char * char_ptr = strchr(m_buffer,character);
	if( char_ptr )
	{
		return S32(char_ptr-m_buffer);
	}

    return -1;
}

//..

S32 Str::GetLastIndexOfCharacter( const char character ) const
{
	char * char_ptr = strrchr(m_buffer,character);
	if( char_ptr )
	{
		return S32(char_ptr-m_buffer);
	}

	return -1;
}

// ..

S32 Str::GetCharacterCount(const Char _char) const
{
	S32 count = 0;
	for(U32 i=0;i<m_size;i++)
		if( m_buffer[i] == _char )
			count++;

	return count;
}

// ..

S32	Str::GetWordLastIndex(const Str& _subStr) const
{
	S32 firstIdx = GetLength() - _subStr.GetLength() - 1;
	for(S32 i=firstIdx;i>=0; i--)
	{
		for(U32 j=0;j<_subStr.GetLength();j++)
		{
			if( _subStr.GetArray()[j] == GetArray()[i+j] )
			{
				if( j == _subStr.GetLength()-1)
				{
					return i;
				}
			}
			else
				break;
		}
	}

	return -1;
}

// ..

void Str::Replace(Char _from, Char _to)
{
	for(U32 i=0;i<GetLength();i++)
		if( m_buffer[i] == _from )
			m_buffer[i] = _to;
}

// ..

Str	Str::GetExtension() const
{
	S32 i = GetLastIndexOfCharacter('.');
	return GetTextInsideRange(i+1,GetLength()-1);
}

// ..

Str	Str::GetFilename() const
{
	S32 a = GetLastIndexOfCharacter('\\');
	S32 b = GetLastIndexOfCharacter('/');
	S32 idx = Max(a, b);
	if (idx >= 0)
		return GetTextInsideRange(idx+1, GetLength() - 1);

	return *this;
}

// ..

Str	Str::GetDirectory() const
{
	S32 a = GetLastIndexOfCharacter('\\');
	S32 b = GetLastIndexOfCharacter('/');
	S32 idx = Max(a, b);
	if(idx >= 0)
		return GetTextInsideRange(0, idx);

	return "";
}

// ..

const Str& Str::SetFrom(const char * text, ... )
{
	char buffer[4096];
	va_list args;
	va_start(args, text);
	VSPrintf(buffer,text, args);
	va_end(args);

	CopyFromChar(buffer);

	return *this;
}

//..

void Str::Split(const Char* _separators, StrArray& _out) const
{
	S32 current_word = 0;
	S32 last_index = -1;
	U32 separator_count = U32(strlen(_separators));
	for(U32 i=0;i<m_size;i++)
	{
		Bool bFound = FALSE;
		U32 s = 0;
		while(_separators[s]!='\0')
		{
			if( m_buffer[i] == _separators[s] )
			{
				bFound = TRUE;
				break;
			}
			s++;
		}

		if( bFound )
		{
			S32 count = i-last_index-1;
			if( count > 0 )
			{
				Char new_word[256];
				memcpy(new_word,m_buffer+last_index+1,count);
				new_word[count]=0;
				_out.AddLastItem(new_word);
				current_word++;
			}
			last_index = i;
		}
	}

	// last word
	S32 count = m_size-last_index-1;
	if( count > 0)
	{
		Char new_word[256];
		memcpy(new_word,m_buffer+last_index+1,count);
		new_word[count]=0;
		_out.AddLastItem(new_word);
		current_word++;
	}

	_out.SetItemCount(current_word);
}

void Str::ToUpper()
{
	for(U32 i=0;i<m_size;i++)
	{
		m_buffer[i] = g_LowerUpperMapping.m_UpperTab[m_buffer[i]];
	}
}

void Str::ToLower()
{
	for (U32 i = 0; i<m_size; i++)
	{
		m_buffer[i] = g_LowerUpperMapping.m_LowerTab[m_buffer[i]];
	}
}

void Str::CopyFromChar( const char * text )
{
	U32 new_size = text?GetLength( text ):0;
	ChangeLength(new_size, text);
	if( new_size != 0 )
	{
		memcpy(m_buffer, text, new_size);
	}
}

void Str::ChangeLength(U32 _length , const char * _newtext)
{
	if (_length == 0)
	{
		m_allocsize = 0;
		DELETE[] m_buffer;
		m_buffer = NULL;
	}
	else
	{
		if (_length + 1 > m_allocsize)
		{
			m_allocsize = _length + 1;
			DELETE[] m_buffer;
			m_buffer = NEW_NAMED(_newtext) char[_length + 1];
		}

		m_buffer[_length] = '\0';
	}

	m_size = _length;
}

Str Str::Void("");