#include <stddef.h>
#include "chip_sqlite.h"

sqlite3		*chip_sqlite_open(const char *db_path)
{
	sqlite3		*db;
	int			rc;

	db = NULL;
	rc = sqlite3_open(db_path, &db);
	if (rc != SQLITE_OK)
        sqlite3_close(db);

	return db;
}
