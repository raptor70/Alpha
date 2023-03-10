#include "base_file.h"

#include <fstream>

#include "platform/platform_win32.h"

OldFile::OldFile( const Str & path, const USE_TYPE & use_type ):
	m_UseType( use_type ),
	m_bOpen( FALSE ),
	m_bAtEnd( FALSE ),
	m_pBuffer(NULL),
	m_pCurrentBuffer(NULL),
	m_iBufferSize(0)
{
	m_pFile = NULL;
	m_sPath = path;
#ifdef ANDROID    
	if( m_UseType & USE_TYPE_Data )
		m_sPath = m_sDataPath + m_sPath;
	else if( m_UseType & USE_TYPE_Cache )
		m_sPath = m_sCachePath + m_sPath;
#elif defined(IOS)
	if( m_UseType & USE_TYPE_Data )
	{
	    Str dirPath = [[NSSearchPathForDirectoriesInDomains( NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0] UTF8String];
		m_sPath = dirPath + "/" + m_sPath;
	}
	else if( m_UseType & USE_TYPE_Cache )
	{
	    Str dirPath = [[NSSearchPathForDirectoriesInDomains( NSCachesDirectory, NSUserDomainMask, YES) objectAtIndex:0] UTF8String];
		m_sPath = dirPath + "/" + m_sPath;
	}
	else
	{
		S32 id = path.GetLastIndexOfCharacter('.');
		Str ext = path.GetTextInsideRange(id+1,path.GetLength()-1);
		Str mainpath = path.GetTextInsideRange(0,id-1);
		CFStringRef srext = CFStringCreateWithCString(NULL,ext.GetArray(),kCFStringEncodingMacRoman);
		CFStringRef srmainpath = CFStringCreateWithCString(NULL,mainpath.GetArray(),kCFStringEncodingMacRoman);

		CFBundleRef mainBundle = CFBundleGetMainBundle();
		CFURLRef imageURL = CFBundleCopyResourceURL(mainBundle, srmainpath, srext, NULL);
		if( imageURL )
		{
			CFStringRef imagePath = CFURLCopyFileSystemPath(imageURL, kCFURLPOSIXPathStyle);
			CFStringEncoding encodingMethod = CFStringGetSystemEncoding();
			m_sPath = CFStringGetCStringPtr(imagePath, encodingMethod);
			CFRelease(imagePath);
		}
		else
			m_sPath = path;
        
        CFRelease(srext);
        CFRelease(srmainpath);
	}
#else
	if( m_UseType & USE_TYPE_Cache )
		m_sPath = Str(CACHE_DIR) + m_sPath;
	if (m_UseType & USE_TYPE_Data)
		m_sPath = Str(SAVE_DIR) + m_sPath;
	if (m_UseType & USE_TYPE_Log)
		m_sPath = Str(LOG_DIR) + m_sPath;
	if (m_UseType & USE_TYPE_Temp)
		m_sPath = Str(TMP_DIR) + m_sPath;
#endif
    
    
}

// ..

OldFile::~OldFile( void ) 
{
	if( IsOpen() )
		Close();
}

// ..

Bool OldFile::Exists( void )
{
	if( IsOpen() )
		return TRUE;

	std::ifstream new_file( m_sPath.GetArray() ); 
	if( !new_file.fail() )
		return TRUE;

#ifdef ANDROID
	int err = 0;
	zip *z = zip_open(m_sAssetPath.GetArray(), 0, &err);
	if( z ) 
	{
		struct zip_stat st;
		zip_stat_init(&st);
		zip_stat(z, GetPath().GetArray(), 0, &st);
		if( st.valid) 
			return TRUE;
	}
#endif
	return FALSE;
}

// ..

Bool OldFile::Delete()
{
	std::remove(m_sPath.GetArray());
	return !Exists();
}

// ..

S64 OldFile::GetSize( void )
{
	DEBUG_Require( IsOpen() );

	if( m_pBuffer )
		return m_iBufferSize;

	S64 pos = ftell(m_pFile);
	fseek(m_pFile, 0L, SEEK_END);
	S64 size = ftell(m_pFile);
	fseek(m_pFile, (long)pos, SEEK_SET);
	return size;
}

// ..

void OldFile::Open( Bool _bClearContent /*=FALSE*/ )
{
	DEBUG_Require( !IsOpen() );
	DEBUG_Require( m_UseType );

	Bool bOk = FALSE;

#if defined(WIN32) || defined(IOS)
	if( ( m_UseType & USE_TYPE_Write ) == USE_TYPE_Write )
	{	
#if defined(WIN32)
		// try to create folder
		S32 i = m_sPath.GetLastIndexOfCharacter('/');
		if( i >= 0 )
		{
			const unsigned long maxDir = 260;
			char currentDir[maxDir];
		    GetCurrentDirectory(maxDir, currentDir);

			Str folderPath = m_sPath.GetTextInsideRange(0,i-1);
			Str command;
			command += "if not exist \"";
			command += folderPath;
			command += "\" ";
			command += "mkdir \"";
			command += folderPath;
			command += "\"";
			system(command.GetArray());
		}
#endif
	}

	Str mode ="";
	if ( ( m_UseType & USE_TYPE_Read ) == USE_TYPE_Read )
	{
		mode += "r";
	}

	if( ( m_UseType & USE_TYPE_Write ) == USE_TYPE_Write )
	{
		mode += "w";
	}

	mode += "b";

	m_pFile = fopen(GetPath().GetArray(),mode.GetArray());
	bOk = (m_pFile != NULL);

#elif defined(ANDROID)
	Bool bUseZip = FALSE;
	if( m_UseType & USE_TYPE_Data )
	{
		struct stat sb;
		int32_t res = stat(m_sDataPath.GetArray(), &sb);
		if (0 == res && sb.st_mode & S_IFDIR)
		{
			LOGGER_Log("%s already exists\n",m_sDataPath.GetArray());;
		}
		else if (ENOENT == errno)
		{
			res = mkdir(m_sDataPath.GetArray(), 0770);
			LOGGER_Log("Create %s\n",m_sDataPath.GetArray());
		}
	}
	else if( m_UseType & USE_TYPE_Cache )
	{
		struct stat sb;
		int32_t res = stat(m_sCachePath.GetArray(), &sb);
		if (0 == res && sb.st_mode & S_IFDIR)
		{
			LOGGER_Log("%s already exists\n",m_sCachePath.GetArray());;
		}
		else if (ENOENT == errno)
		{
			res = mkdir(m_sCachePath.GetArray(), 0770);
			LOGGER_Log("Create %s\n",m_sCachePath.GetArray());
		}
	}
	else
	{
		bUseZip = TRUE;
	}

	if( bUseZip )
	{
		//Open the ZIP archive
		int err = 0;
		zip *z = zip_open(m_sAssetPath.GetArray(), 0, &err);
			
		if( z ) 
		{
			//Search for the file of given name
			struct zip_stat st;
			zip_stat_init(&st);
			zip_stat(z, GetPath().GetArray(), 0, &st);
				
			if( st.valid )
			{
				//Alloc memory for its uncompressed contents
				m_iBufferSize = st.size;
				S8* data = NEW S8[m_iBufferSize];
					
				//Read the compressed file
				zip_file *f = zip_fopen(z, GetPath().GetArray(), 0);
				if( f )
				{
					zip_fread(f, data, m_iBufferSize);
					zip_fclose(f);
				}

				m_pBuffer = data;
				m_pCurrentBuffer = m_pBuffer;
				bOk = TRUE;
			}

			//And close the archive
			zip_close(z);
		}
	}
	else
	{
		Str mode ="";
		if ( ( m_UseType & USE_TYPE_Read ) == USE_TYPE_Read )
		{
			mode += "r";
		}

		if( ( m_UseType & USE_TYPE_Write ) == USE_TYPE_Write )
		{
			mode += "w";
		}

		mode += "b";

		m_pFile = fopen(GetPath().GetArray(),mode.GetArray());
		bOk = (m_pFile != NULL);
	}
#endif

	if( !bOk )
	{
		LOGGER_LogError( "Couldn't OPEN file : %s\n", GetPath().GetArray() );
		m_bOpen = FALSE;
		return;
	}
	else
	{
		m_bOpen = TRUE;
	}

	// setup to write file
	if( IsWritable() )
	{
		if( _bClearContent )
			MoveToBegin();
		else
			MoveToEnd();
		SetIsAtEnd( TRUE );
	}

	// load in buffer if needed
	if( !m_pBuffer && m_bOpen && ( (m_UseType & (USE_TYPE_ReadBuffer)) == (USE_TYPE_ReadBuffer ) ) )
	{
		m_iBufferSize = (U32)GetSize();
		S8* data = NEW S8[m_iBufferSize];
		ReadByte(data,m_iBufferSize);
		m_pBuffer = data;
		m_pCurrentBuffer = m_pBuffer;
	}
}

//..

void OldFile::Close( void )
{
	DEBUG_Require( IsOpen() );

	m_bOpen = FALSE;
	SetIsAtEnd( FALSE );

	if( m_pBuffer )
	{
		DELETE m_pBuffer;
		m_pCurrentBuffer = m_pBuffer = NULL;
		m_iBufferSize = 0;
	}

	if( m_pFile )
	{
		fclose (m_pFile);
		m_pFile = NULL;
	}
}

//..

void OldFile::MoveToBegin( void )
{
	DEBUG_Require( IsOpen() );

	if( m_pBuffer )
	{
		m_pCurrentBuffer = m_pBuffer;
		SetIsAtEnd( FALSE );
		return;
	}

	MoveToAdress(0);
	SetIsAtEnd( FALSE );
}

//..

void OldFile::MoveToEnd( void )
{
	DEBUG_Require( IsOpen() );
	
	if( m_pBuffer )
	{
		m_pCurrentBuffer = m_pBuffer + m_iBufferSize;
		SetIsAtEnd( TRUE );
		return;
	}

	fseek(m_pFile, 0L, SEEK_END);
	SetIsAtEnd( TRUE );
}

//..

void OldFile::MoveToAdress(U32 _index)
{
	DEBUG_Require( IsOpen() );

	if( m_pBuffer )
	{
		m_pCurrentBuffer = m_pBuffer + _index;
		SetIsAtEnd( TRUE );
		return;
	}
	
	fseek(m_pFile, (long int)_index, SEEK_SET);
	SetIsAtEnd(feof(m_pFile)!=0);
}

S64 OldFile::GetCurrentAdress()
{
	DEBUG_Require( IsOpen() );

	if( m_pBuffer )
	{
		return m_pCurrentBuffer - m_pBuffer;
	}

	S64 current = ftell(m_pFile);
	return current;
}

void OldFile::WriteByte( const S8 * data, const U32 data_to_write )
{
	DEBUG_Require( IsOpen() );
	fwrite(data,1,data_to_write,m_pFile);
	SetIsAtEnd( feof(m_pFile)!=0 );
}

//..

Bool OldFile::ReadByte( S8 * data, const U32 data_to_read )
{
	if( data_to_read == 0 )
		return TRUE;

	if( m_pBuffer )
	{
		U32 dataleft = (m_iBufferSize - U32(m_pCurrentBuffer - m_pBuffer));
		if( data_to_read <  dataleft)
		{
			memcpy(data,m_pCurrentBuffer,data_to_read);
			m_pCurrentBuffer += data_to_read;
			return TRUE;
		}
		
		memcpy(data,m_pCurrentBuffer,dataleft);
		m_pCurrentBuffer = m_pBuffer + m_iBufferSize;
		SetIsAtEnd( TRUE );
		return FALSE;
	}

	DEBUG_Require( IsOpen() );
	fread(data,1,data_to_read,m_pFile);
	if( feof(m_pFile) )
	{
		SetIsAtEnd( TRUE );
		return FALSE;
	}

	return TRUE;
}

// .. 

void OldFile::WriteText( const Str & text )
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

void OldFile::WriteTextLine( const Str & text )
{
    WriteText( text );
    WriteByte( "\r\n", 2 );
}

//..

void OldFile::AppendText( const Str & text )
{
    MoveToEnd();
    WriteText( text );
}

//..

void OldFile::AppendTextLine( const Str & text )
{
    MoveToEnd();
    WriteTextLine( text );
}

//..

Bool OldFile::ReadText( Str & text, const S64 length_to_read )
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

Bool OldFile::ReadCharacter( char & character )
{
    return ReadByte( &character, 1 );
}

void OldFile::WriteBool(Bool _value)
{
	S8 v = _value?1:0;
	WriteByte(&v, 1);
}
void OldFile::WriteU16(U16 _value)
{
	WriteByte((S8*)&_value,2);
}
void OldFile::WriteS16(S16 _value)
{
	WriteByte((S8*)&_value,2);
}
void OldFile::WriteS32(S32 _value)
{
	WriteByte((S8*)&_value,4);
}
void OldFile::WriteU32(U32 _value)
{
	WriteByte((S8*)&_value,4);
}
void OldFile::WriteFloat(Float _value)
{
	WriteByte((S8*)&_value,4);
}

Bool OldFile::ReadBool()
{
	S8 value = 0;
	ReadByte((S8*)&value,1);
	return (value == 1)?TRUE:FALSE;
}

S16	OldFile::ReadS16()
{
	S16 value = 0;
	ReadByte((S8*)&value,2);
	return value;
}

U16	OldFile::ReadU16()
{
	U16 value = 0;
	ReadByte((S8*)&value,2);
	return value;
}

S32 OldFile::ReadS32()
{
	S32 value = 0;
	ReadByte((S8*)&value,4);
	return value;
}

U32 OldFile::ReadU32()
{
	U32 value = 0;
	ReadByte((S8*)&value,4);
	return value;
}

Float OldFile::ReadFloat()
{
	Float value = 0;
	ReadByte((S8*)&value,4);
	return value;
}

//..

Bool OldFile::ReadCompleteTextFile( Str & text )
{
	MoveToBegin();
	return ReadText( text, GetSize() );
}

//..

Bool OldFile::ReadLine( Str & text )
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
            DEBUG_Forbidden( "Bad format line in file %s", GetPath().GetArray() );
            return FALSE;
        }
    }

    return TRUE;
}

//..

U32	OldFile::GetModificationDate() const
{
	struct stat fileInfo;
	if (stat(m_sPath.GetArray(), &fileInfo)==0)
	{
		return (U32)fileInfo.st_mtime;
	}

	return 0;
}

//..

void OldFile::SetPathNearExe(Str& _path)
{
#ifdef WIN32
	HMODULE hModule = GetModuleHandleW(NULL);
	WCHAR WExePath[256];
	GetModuleFileNameW(hModule, WExePath, 256);
	Char ExePath[256];
	wcstombs(ExePath, WExePath, 256);
	Str dirPath = ExePath;
	S32 lastSlash = dirPath.GetLastIndexOfCharacter('\\');
	dirPath = dirPath.GetTextInsideRange(0, lastSlash);
	_path = dirPath + _path;
#endif
}

//..

#ifdef ANDROID
Str OldFile::m_sDataPath = "";
Str OldFile::m_sCachePath = "";
Str OldFile::m_sAssetPath = "";
#endif
