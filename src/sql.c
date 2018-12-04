#include <sqlite3.h>
#include "sql.h"

int		sql_open(const char *filename, void *connection)
{
	sqlite3		*db;
	int			rc;

	rc = sqlite3_open(filename, &db);
	if (rc != SQLITE_OK)
	{
		sqlite3_close(db);
		return 1;
	}
	connection = db;
	return 0;
}

int		sql_close(void *connection)
{
	return sqlite3_close(connection);
}

int		sql_exec(const char *query, sql_callback_t callback)
{
	return 0;
}
