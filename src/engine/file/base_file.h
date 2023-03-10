#ifndef __BASE_FILE_H__
#define __BASE_FILE_H__

class OldFile
{
public:
	enum USE_TYPE
	{
		USE_TYPE_None = 0x0,
		USE_TYPE_Read = (1<<0),
		USE_TYPE_Write = (1<<1),
		USE_TYPE_ReadWrite = USE_TYPE_Read | USE_TYPE_Write,
		USE_TYPE_Buffer = (1<<2),
		USE_TYPE_ReadBuffer = USE_TYPE_Read | USE_TYPE_Buffer,
		USE_TYPE_Data = (1<<3),
		USE_TYPE_Cache = (1<<4),
		USE_TYPE_Log = (1<<5),
		USE_TYPE_Temp = (1 << 6),
	};

	OldFile( const Str & path, const USE_TYPE & use_type = USE_TYPE_ReadBuffer );
	virtual ~OldFile( void );

	Bool IsOpen( void ) const { return m_bOpen; }
	Bool IsReadable( void ) const { return IsOpen() && ( m_UseType & USE_TYPE_Read ); }
	Bool IsWritable( void ) const { return IsOpen() && ( m_UseType & USE_TYPE_Write ); }
	void SetIsAtEnd( const Bool it_is_at_end ) { m_bAtEnd = it_is_at_end; }
	Bool IsAtEnd( void ) const { return m_bAtEnd; }
	const Str & GetPath( void ) const { return m_sPath; }

	virtual Bool Exists( void );
	virtual Bool Delete();

	virtual S64 GetSize( void );
	virtual void Open( Bool _bClearContent = FALSE );
	virtual void Close( void );
	virtual void MoveToBegin( void );
	virtual void MoveToEnd( void );
	virtual void MoveToAdress(U32 _index);
	virtual S64 GetCurrentAdress();
	virtual void WriteByte( const S8 * data, const U32 data_to_write );
	virtual Bool ReadByte( S8 * data, const U32 data_to_read );

	void WriteText( const Str & text );
	void WriteTextLine( const Str & text );
	void AppendText( const Str & text );
	void AppendTextLine( const Str & text );

	Bool ReadText( Str & text, const S64 length_to_read );
	Bool ReadCharacter( char & character );
	Bool ReadCompleteTextFile( Str & text );
	Bool ReadLine( Str & text );

	void WriteBool(Bool _value);
	void WriteU16(U16 _value);
	void WriteS16(S16 _value);
	void WriteU32(U32 _value);
	void WriteS32(S32 _value);
	void WriteFloat(Float _value);

	Bool ReadBool();
	U16	ReadU16();
	S16	ReadS16();
	S32 ReadS32();
	U32 ReadU32();
	Float ReadFloat();

	U32	GetModificationDate() const;

	S8*		GetBuffer() { return m_pBuffer; }
	U32		GetBufferSize() { return m_iBufferSize; }

	static void SetPathNearExe(Str& _path);

protected :
	USE_TYPE m_UseType;
	Bool m_bOpen;

private:
	OldFile( void ){}
	OldFile( const OldFile & ){}
	const OldFile & operator=( const OldFile & other ){return *this;}

	Str m_sPath;
	Bool m_bAtEnd;

	S8*		m_pBuffer;
	S8*		m_pCurrentBuffer;
	U32		m_iBufferSize;

	FILE*	m_pFile;
#ifdef ANDROID
public:
	static Str m_sDataPath;
	static Str m_sCachePath;
	static Str m_sAssetPath;
#endif
};

typedef RefTo<OldFile> FileRef;

#endif
