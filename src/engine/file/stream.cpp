#include "stream.h"

Stream::Stream():
	m_iPos(0)
{   
}

// ..

Stream::~Stream()
{
}

// ..

void Stream::MoveToBegin()
{
	MoveToPos(0);
}

//..

void Stream::MoveToEnd( void )
{
	MoveToPos(GetSize());
}

//..

void Stream::MoveToPos(U32 _index)
{
	m_iPos = _index;
}


void Stream::WriteByte( const S8 * data, const U32 data_to_write )
{
	U32 newCount = m_iPos + data_to_write;
	if (newCount >= m_Buffer.GetCount())
		m_Buffer.SetItemCount(newCount);
	memcpy(&(m_Buffer[m_iPos]), data, data_to_write);
	m_iPos += data_to_write;
}

//..

Bool Stream::ReadByte( S8 * data, const U32 data_to_read )
{
	if( data_to_read == 0 )
		return TRUE;

	U32 dataleft = Max<S32>(0,m_Buffer.GetCount() - m_iPos);
	if( data_to_read < dataleft)
	{
		memcpy(data,&(m_Buffer[m_iPos]),data_to_read);
		m_iPos  += data_to_read;
		return TRUE;
	}
		
	memcpy(data, &(m_Buffer[m_iPos]),dataleft);
	m_iPos += dataleft;
	return FALSE;
}

// .. 

void Stream::WriteText( const Str & text )
{
    const Char* character_array = text.GetArray();
    U32 first_character_to_write = 0;

    for( U32 character_index = 0; character_index < text.GetLength(); character_index++ )
    {
        if( character_array[ character_index ] == '\n' )
        {
            S32 character_to_write_count = character_index - first_character_to_write;
            if( character_to_write_count )
            {
                WriteByte( &character_array[ first_character_to_write ], character_to_write_count );
            }
            
            WriteByte( "\r\n", 2 );

            first_character_to_write = character_index + 1;
        }
    }

    if( first_character_to_write < text.GetLength() )
    {
        WriteByte( &character_array[ first_character_to_write ], text.GetLength() - first_character_to_write );
    }
}

//..

void Stream::WriteTextLine( const Str & text )
{
    WriteText( text );
    WriteByte( "\r\n", 2 );
}

//..

void Stream::AppendText( const Str & text )
{
    MoveToEnd();
    WriteText( text );
}

//..

void Stream::AppendTextLine( const Str & text )
{
    MoveToEnd();
    WriteTextLine( text );
}

//..

Bool Stream::ReadText( Str & text, const S64 length_to_read )
{
    if( length_to_read <= 0 )
    {
        return FALSE;
    }

    char * char_array = NEW char[ (S32)length_to_read + 1 ];
    ReadByte( char_array, (S32)length_to_read );

	char_array[length_to_read] = '\0';
    text = char_array;

    DELETE[] char_array;

    return TRUE;
}

//..

Bool Stream::ReadCharacter( char & character )
{
    return ReadByte( &character, 1 );
}

//..

void Stream::WriteBool(Bool _value)
{
	S8 v = _value?1:0;
	WriteByte(&v, 1);
}

//..

void Stream::WriteU8(U8 _value)
{
	WriteByte((S8*)&_value, 1);
}

//..

void Stream::WriteU16(U16 _value)
{
	WriteByte((S8*)&_value,2);
}

//..

void Stream::WriteS16(S16 _value)
{
	WriteByte((S8*)&_value,2);
}

//..

void Stream::WriteS32(S32 _value)
{
	WriteByte((S8*)&_value,4);
}

//..

void Stream::WriteU32(U32 _value)
{
	WriteByte((S8*)&_value,4);
}

//..

void Stream::WriteU64(U64 _value)
{
	WriteByte((S8*)&_value, 8);
}

//..

void Stream::WriteFloat(Float _value)
{
	WriteByte((S8*)&_value,4);
}

//..

Bool Stream::ReadBool()
{
	S8 value = 0;
	ReadByte((S8*)&value,1);
	return (value == 1)?TRUE:FALSE;
}

//..

U8 Stream::ReadU8()
{
	U8 value = 0;
	ReadByte((S8*)&value, 1);
	return value;
}

//..

S16	Stream::ReadS16()
{
	S16 value = 0;
	ReadByte((S8*)&value,2);
	return value;
}

//..

U16	Stream::ReadU16()
{
	U16 value = 0;
	ReadByte((S8*)&value,2);
	return value;
}

//..

S32 Stream::ReadS32()
{
	S32 value = 0;
	ReadByte((S8*)&value,4);
	return value;
}

//..

U32 Stream::ReadU32()
{
	U32 value = 0;
	ReadByte((S8*)&value,4);
	return value;
}

//..

U64 Stream::ReadU64()
{
	U64 value = 0;
	ReadByte((S8*)&value, 8);
	return value;
}

//..

Float Stream::ReadFloat()
{
	Float value = 0;
	ReadByte((S8*)&value,4);
	return value;
}

//..

Bool Stream::ReadCompleteTextFile( Str & text )
{
	MoveToBegin();
	return ReadText( text, GetSize() );
}

//..

Bool Stream::ReadLine( Str & text )
{
    Char character;

	text.Alloc(128);

    while( ReadCharacter( character )  && character != '\r' && character != '\n' )
    {
        text += character;
    }

    if( character == '\n' )
    {
        return TRUE;
    }
    
    if( character == '\r' )
    {
        if( ReadCharacter( character ) && character == '\n' )
        {
            return TRUE;
        }
        else
        {
            DEBUG_Forbidden( "Bad format line\n" );
            return FALSE;
        }
    }

    return TRUE;
}

//..
