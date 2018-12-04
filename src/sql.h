#ifndef SQL_H
#define SQL_H

enum { SQL_OK, SQL_FAIL };
typedef int (*sql_callback_t)(void *, int, char **, char **);

int		sql_open(const char *filename, void **connection);
int		sql_close(void *connection);
int		sql_exec(void *connection, const char *query, sql_callback_t callback,
	void *callback_opt_arg);
const char * sql_error_msg(void *connection);

#endif /* SQL_H */
