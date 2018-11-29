#ifndef CHIP_SQLITE_H
#define CHIP_SQLITE_H
#include <sqlite3.h>

sqlite3		*chip_sqlite_open(const char *db_path);
int			chip_sqlite_close(sqlite3 *db);

#endif /* CHIP_SQLITE_H */
