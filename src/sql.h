#ifndef SQL_H
#define SQL_H

typedef int (*sql_callback_t)(void *, int, char **, char **);

int		sql_open(const char *filename, void *connection);
int		sql_close(void *connection);
int		sql_exec(const char *query, sql_callback_t callback);

#endif /* SQL_H */
