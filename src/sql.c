#include <sqlite3.h>
#include <stdio.h>
#include "sql.h"

int		sql_open(const char *filename, void **connection)
{
	sqlite3		*db;
	int			rc;

	rc = sqlite3_open(filename, &db);
	if (rc != SQLITE_OK)
	{
		sqlite3_close(db);
		return SQL_FAIL;
	}
	*connection = db;
	return SQL_OK;
}

int		sql_close(void *connection)
{
	return sqlite3_close(connection);
}

int		sql_exec(void *connection, const char *query, sql_callback_t callback,
	void *callback_opt_arg)
{
	int		rc;

	rc = sqlite3_exec(connection, query, callback, callback_opt_arg,
						callback_opt_arg);
	if (rc != SQLITE_OK)
	{
		sqlite3_close(connection);
        return SQL_FAIL;
	}    
	return SQL_OK;
}

const char * sql_error_msg(void *connection)
{
	return sqlite3_errmsg(connection);
}
