#ifndef SQL_H
#define SQL_H

typedef int (*sql_callback_t)(void *, int, char **, char **);

int		sql_open(const char *filename, void *connection);
int		sql_close(void *connection);
int		sql_exec(void *connection, const char *query, sql_callback_t callback,
	void *callback_opt_arg);

#endif /* SQL_H */
