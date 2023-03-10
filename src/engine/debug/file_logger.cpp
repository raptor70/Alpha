#include "file_logger.h"

#define LOCAL_DefaultFileName "application.log"

OldFileLogger::OldFileLogger( void ): 
	BaseLogger(), 
	m_out( LOCAL_DefaultFileName, OldFile::USE_TYPE(OldFile::USE_TYPE_Log | OldFile::USE_TYPE_Write) )
{
	m_out.Open();
}

OldFileLogger::OldFileLogger( const Bool _overwrite ):
	BaseLogger(),
	m_out( LOCAL_DefaultFileName, OldFile::USE_TYPE(OldFile::USE_TYPE_Log | OldFile::USE_TYPE_Write) )
{
	m_out.Open();
	if( _overwrite )
		m_out.MoveToBegin();
}

OldFileLogger::OldFileLogger( const Str & _path, const Bool _overwrite ):
	BaseLogger(),
	m_out( _path.GetArray(), OldFile::USE_TYPE(OldFile::USE_TYPE_Log | OldFile::USE_TYPE_Write) )
{
	m_out.Open();
	if( _overwrite )
		m_out.MoveToBegin();
}

OldFileLogger::OldFileLogger( const OldFileLogger & _other ): 
	BaseLogger( _other ), 
	m_out( LOCAL_DefaultFileName, OldFile::USE_TYPE(OldFile::USE_TYPE_Log | OldFile::USE_TYPE_Write) )
{
	m_out.Open();
}

OldFileLogger::~OldFileLogger( void )
{
	m_out.Close();
}

void OldFileLogger::Write( const LogType _type, const Str & _message ) const
{
    m_out.AppendText( _message.GetArray() );
}

#undef LOCAL_DefaultFileName