#ifndef __STREAM_H__
#define __STREAM_H__

class Stream
{
public:
	Stream();
	virtual ~Stream();

	const S8*		GetBuffer() const { return m_Buffer.GetPointer(); }
	S8*		GetBuffer() { return m_Buffer.GetPointer(); }
	U32		GetSize() const { return m_Buffer.GetCount(); }
	U32		GetCurrentPos() const { return m_iPos; }

	virtual void MoveToBegin();
	virtual void MoveToEnd();
	virtual void MoveToPos(U32 _index);

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
	void WriteU8(U8 _value);
	void WriteU16(U16 _value);
	void WriteS16(S16 _value);
	void WriteU32(U32 _value);
	void WriteS32(S32 _value);
	void WriteU64(U64 _value);
	void WriteFloat(Float _value);

	Bool ReadBool();
	U8 ReadU8();
	U16	ReadU16();
	S16	ReadS16();
	S32 ReadS32();
	U32 ReadU32();
	U64 ReadU64();
	Float ReadFloat();

private:
	S8Array		m_Buffer;
	U32			m_iPos;
};

typedef RefTo<Stream> StreamRef;

#endif
