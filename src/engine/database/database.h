#ifndef __DATABASE_H__
#define __DATABASE_H__

struct sqlite3;

typedef ArrayOf<Params>		DatabaseRow;
typedef ArrayOf<DatabaseRow>	DatabaseTable;

BEGIN_SUPER_CLASS(Database,Resource)
public:
	Database();
	virtual ~Database();

	static  const Char* GetResourcePrefix() { return "db"; }

	Bool Open();
	void Close();

	Bool ExecuteQuery(const Str& _query);
	Bool GetDataFromQuery(const Str& _query, DatabaseTable& _outTable);

protected:
	sqlite3 *	m_DB;
	Bool		m_bIsOpen;
END_SUPER_CLASS

#endif