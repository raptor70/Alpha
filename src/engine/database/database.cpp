#include "database.h"

#include INCLUDE_LIB(LIB_SQLITE, sqlite3.h)

Database::Database():SUPER()
{
	m_DB = NULL;
	m_bIsOpen = FALSE;
}

//----------------------------------

Database::~Database()
{
	Close();
}

//----------------------------------

Bool Database::Open()
{
	DEBUG_Require(!m_bIsOpen);
	int result = sqlite3_open(GetPath().GetArray(), &m_DB);
	if( result != SQLITE_OK )
	{
		LOGGER_LogError("SQL error(%d) : Unable to open %s !!\n",result,GetPath().GetArray());
		Close();
		return FALSE;
	}

	m_bIsOpen = TRUE;
	return TRUE;
}

//----------------------------------

void Database::Close()
{
	if(m_bIsOpen)
	{
		sqlite3_close(m_DB);
		m_DB = NULL;
		m_bIsOpen = FALSE;
	}
}

//----------------------------------

Bool Database::ExecuteQuery(const Str& _query)
{
	DEBUG_Require(m_bIsOpen);
	Char* errMsg = NULL;
	int result = sqlite3_exec(m_DB, _query.GetArray(), NULL,NULL, &errMsg);
	if( result != SQLITE_OK )
	{
		LOGGER_LogError("SQL error(%d) : %s\n", result, errMsg);
		LOGGER_LogError("%s",_query.GetArray());
		sqlite3_free(errMsg);
		return FALSE;
	}

	return TRUE;
}
	
//----------------------------------

Bool Database::GetDataFromQuery(const Str& _query, DatabaseTable& _outTable)
{
	DEBUG_Require(m_bIsOpen);

	sqlite3_stmt* statement = NULL;
	int result = sqlite3_prepare_v2(m_DB,_query.GetArray(),_query.GetLength(),&statement,NULL);
	if( result != SQLITE_OK )
	{
		LOGGER_LogError("SQL error(%d) : Unable to prepare query\n", result);
		LOGGER_LogError("%s",_query.GetArray());
		return FALSE;
	}

	result = sqlite3_step(statement);
	while( result == SQLITE_ROW )
	{
		S32 colCount = sqlite3_column_count(statement);
		DatabaseRow& row = _outTable.AddLastItem(DatabaseRow());
		for(S32 c=0; c<colCount; c++)
		{
			Params& newValue = row.AddLastItem(Params());
			int colType = sqlite3_column_type(statement,c);
			switch(colType)
			{
			case SQLITE_INTEGER:
				{
					newValue.SetS32(sqlite3_column_int(statement,c));
					break;
				}
			case SQLITE_FLOAT:
				{
					newValue.SetFloat((Float)sqlite3_column_double(statement,c));
					break;
				}
			case SQLITE_TEXT:
			case SQLITE_BLOB:
				{
					newValue.SetStr((const Char*)sqlite3_column_text(statement,c));
					break;
				}

			case SQLITE_NULL:
				{
					newValue.SetNull();
					break;
				}
			}
		}
		result = sqlite3_step(statement);
	}

	if( result != SQLITE_DONE )
	{
		LOGGER_LogError("SQL error(%d) : Unable to get data from query\n", result);
		LOGGER_LogError("%s",_query.GetArray());
		return FALSE;
	}

	return TRUE;
}