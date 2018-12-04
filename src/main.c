#include <gtk/gtk.h>
#include <config.h>
#include "sql.h"

#define		PROGRAM_TITLE		"Книжка оценок"
#define		DATABASE_FILENAME	(DATA_PATH "/" DATABASE_NAME)

enum {WINDOW_WIDTH = 600, WINDOW_HEIGHT = 400};

static GtkWidget	*create_login_page(void);
static GtkWidget	*create_subject_page(void);
static GtkWidget	*create_pupil_page(void);
static void setup_tree_view(GtkWidget *tree_view);
static void on_button_add_clicked(GtkWidget *button, gpointer data);
static void on_button_update_clicked(GtkWidget *button, gpointer data);
static void on_button_delete_clicked(GtkWidget *button, gpointer data);
static void on_button_pupil_login_clicked(GtkWidget *button, gpointer data);
static void on_button_teacher_login_clicked(GtkWidget *button, gpointer data);
static int show_subject_callback(void *opt_arg, int row_count, char **rows,
	char **col_name);
static int fill_pupil_callback(void *opt_arg, int row_count, char **rows,
	char **col_name);
static int fill_teacher_login_callback(void *opt_arg, int col_count,
	char **cols, char **col_names);
static int check_pupil_password_callback(void *opt_arg, int col_count,
	char **cols, char **col_names);
static int check_teacher_password_callback(void *opt_arg, int col_count,
	char **cols, char **col_names);
static int select_max_subject_id_callback(void *opt_arg, int col_count,
	char **cols, char **col_names);
static void	load_subject(void);
static void on_treeview_subject_row_activated(GtkTreeView *tree_view,
	GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
static int is_selected_subject_row(void);
static void fill_pupil_store(void);
static void fill_teacher_login(void);
static void login_pupil(int id);
static void login_teacher(void);
static gboolean check_pupil_login(int id, const gchar *password);
static gboolean check_teacher_login(int id, const gchar *password);
static void show_message_box(const char *message);

static GtkWidget	*window;
static GtkWidget	*combo_box_pupil;
static GtkWidget	*notebook;
static GtkWidget	*hbox_subject;
static GtkListStore	*store_subject;
static GtkListStore	*store_pupil;
static GtkWidget	*tree_view_subject;
static int			selected_subject_id;
static int			max_subject_id;
static gchar		*teacher_login;
static gboolean		is_pupil_password_match;
static gboolean		is_teacher_password_match;

int main(int argc, char *argv[])
{
	GtkWidget *label_login, *label_subject, *label_pupil;

    gtk_init(&argc, &argv);

	store_pupil = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	store_subject = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);

	fill_pupil_store();
	fill_teacher_login();
	load_subject();

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), PROGRAM_TITLE);
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);
	gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_WIDTH, WINDOW_HEIGHT);

	label_login = gtk_label_new("Вход в книжку оценок");
	label_subject = gtk_label_new("Список предметов");
	label_pupil = gtk_label_new("Список учеников");

	notebook = gtk_notebook_new();
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), create_login_page(),
		label_login);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), create_subject_page(),
		label_subject);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), create_pupil_page(),
		label_pupil);

	gtk_container_add(GTK_CONTAINER(window), notebook);

	g_signal_connect(G_OBJECT(window), "destroy",
						G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
	gtk_widget_hide(hbox_subject);

    gtk_main();

	g_free(teacher_login);
	g_object_unref(store_subject);
	g_object_unref(store_pupil);
}

static GtkWidget	*create_login_page(void)
{
	GtkWidget		*grid_subject;
	GtkWidget		*label_space;
	GtkWidget		*label_pupil;
	GtkWidget		*label_teacher;
	GtkWidget		*label_pupil_login;
	GtkWidget		*entry_teacher_login;
	GtkWidget		*entry_pupil_password;
	GtkWidget		*entry_teacher_password;
	GtkWidget		*button_pupil_login;
	GtkWidget		*button_teacher_login;
	GtkCellRenderer	*column;

	label_space = gtk_label_new(NULL);
	label_pupil = gtk_label_new("Ученик");
	label_teacher = gtk_label_new("Учитель");
	label_pupil_login = gtk_label_new("Логин ученика");
	entry_teacher_login = gtk_entry_new();
	entry_pupil_password = gtk_entry_new();
	entry_teacher_password = gtk_entry_new();
	button_pupil_login = gtk_button_new_with_label("Вход");
	button_teacher_login = gtk_button_new_with_label("Вход");

	gtk_entry_set_text(GTK_ENTRY(entry_teacher_login), teacher_login);
	gtk_entry_set_alignment(GTK_ENTRY(entry_teacher_login), 0.5);
	gtk_widget_set_sensitive(entry_teacher_login, FALSE);

	gtk_entry_set_visibility(GTK_ENTRY(entry_pupil_password), FALSE);
	gtk_entry_set_visibility(GTK_ENTRY(entry_teacher_password), FALSE);

	combo_box_pupil = gtk_combo_box_new_with_model(GTK_TREE_MODEL(store_pupil));

	column = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo_box_pupil), column, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo_box_pupil), column,
									"text", 1,
									NULL);

	gtk_combo_box_set_active(GTK_COMBO_BOX(combo_box_pupil), 0);

	grid_subject = gtk_grid_new();
	gtk_container_set_border_width(GTK_CONTAINER(grid_subject), 10);
	gtk_widget_set_halign(grid_subject, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(grid_subject, GTK_ALIGN_CENTER);
	gtk_grid_set_row_spacing(GTK_GRID(grid_subject), 10);
	gtk_grid_set_column_spacing(GTK_GRID(grid_subject), 10);
	gtk_grid_set_row_homogeneous(GTK_GRID(grid_subject), FALSE);
	gtk_grid_set_column_homogeneous(GTK_GRID(grid_subject), FALSE);

	gtk_grid_attach(GTK_GRID(grid_subject), label_pupil, 0, 0, 1, 2);
	gtk_grid_attach(GTK_GRID(grid_subject), combo_box_pupil, 1, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid_subject), entry_pupil_password, 1, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid_subject), button_pupil_login, 2, 0, 1, 2);
	gtk_grid_attach(GTK_GRID(grid_subject), label_space, 0, 2, 3, 1);
	gtk_grid_attach(GTK_GRID(grid_subject), label_teacher, 0, 3, 1, 2);
	gtk_grid_attach(GTK_GRID(grid_subject), entry_teacher_login, 1, 3, 1, 1);
	gtk_grid_attach(GTK_GRID(grid_subject), entry_teacher_password, 1, 4, 1, 1);
	gtk_grid_attach(GTK_GRID(grid_subject), button_teacher_login, 2, 3, 1, 2);

	g_signal_connect(G_OBJECT(button_pupil_login), "clicked",
						G_CALLBACK(on_button_pupil_login_clicked),
						(gpointer)entry_pupil_password);
	g_signal_connect(G_OBJECT(button_teacher_login), "clicked",
						G_CALLBACK(on_button_teacher_login_clicked),
						(gpointer)entry_teacher_password);

	return grid_subject;
}

static GtkWidget	*create_subject_page(void)
{
	GtkWidget	*frame_subject;
	GtkWidget	*vbox;
	GtkWidget	*label_subject;
	GtkWidget	*entry_subject;
	GtkWidget	*button_update;
	GtkWidget	*button_add;
	GtkWidget	*button_delete;
	GtkWidget	*space;
	GtkWidget	*frame_tree;
	GtkWidget	*frame_buttons;

	tree_view_subject = gtk_tree_view_new();
	label_subject = gtk_label_new("Предмет");
	entry_subject = gtk_entry_new();
	button_update = gtk_button_new_with_label("Изменить");
	button_add = gtk_button_new_with_label("Добавить");
	button_delete = gtk_button_new_with_label("Удалить");
	space = gtk_label_new(NULL);

	setup_tree_view(tree_view_subject);

	gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view_subject),
		GTK_TREE_MODEL(store_subject));

	frame_subject = gtk_frame_new(NULL);
	frame_tree = gtk_frame_new(NULL);
	frame_buttons = gtk_frame_new(NULL);

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	hbox_subject = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

	gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
	gtk_container_set_border_width(GTK_CONTAINER(frame_tree), 5);
	gtk_container_set_border_width(GTK_CONTAINER(frame_buttons), 5);

	gtk_box_pack_start(GTK_BOX(vbox), label_subject, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), entry_subject, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), button_add, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), button_update, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), button_delete, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), space, TRUE, TRUE, 5);

	gtk_container_add(GTK_CONTAINER(frame_tree), tree_view_subject);
	gtk_container_add(GTK_CONTAINER(frame_buttons), vbox);

	gtk_box_pack_start(GTK_BOX(hbox_subject), frame_tree, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(hbox_subject), frame_buttons, FALSE, FALSE, 5);

	gtk_container_add(GTK_CONTAINER(frame_subject), hbox_subject);

	g_signal_connect(G_OBJECT(tree_view_subject), "row-activated",
						G_CALLBACK(on_treeview_subject_row_activated),
						(gpointer)entry_subject);
	g_signal_connect(G_OBJECT(button_add), "clicked",
						G_CALLBACK(on_button_add_clicked),
						(gpointer)entry_subject);
	g_signal_connect(G_OBJECT(button_update), "clicked",
						G_CALLBACK(on_button_update_clicked),
						(gpointer)entry_subject);
	g_signal_connect(G_OBJECT(button_delete), "clicked",
						G_CALLBACK(on_button_delete_clicked),
						(gpointer)entry_subject);

	return frame_subject;
}

static GtkWidget	*create_pupil_page(void)
{
	return gtk_label_new(NULL);
}

static void setup_tree_view(GtkWidget *tree_view)
{
	GtkTreeViewColumn	*column;
	GtkCellRenderer		*render;
	GtkTreeSelection	*selection;

	char	*headers[] = { "id", "subject" };

	for (int i = 0; i < 2; i++)
	{
		render = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes(headers[i],
			render, "text", i, NULL);
		gtk_tree_view_column_set_resizable(column, TRUE);
		gtk_tree_view_column_set_min_width(column, 20);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
	}

	gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(tree_view),
		GTK_TREE_VIEW_GRID_LINES_BOTH);
	g_object_set(tree_view, "activate-on-single-click", TRUE, NULL);
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
	gtk_tree_selection_set_mode(selection, GTK_SELECTION_BROWSE);
}

static void on_treeview_subject_row_activated(GtkTreeView *tree_view,
	GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data)
{
	GtkTreeIter		iter;
	GtkTreeModel	*model;
	const gchar		*id;
	const gchar		*subject;

	model = gtk_tree_view_get_model(tree_view);
	if (gtk_tree_model_get_iter(model, &iter, path))
		gtk_tree_model_get(model, &iter, 0, &id, 1, &subject, -1);

	selected_subject_id = (int) g_ascii_strtoll(id, NULL, 10);
	gtk_entry_set_text(GTK_ENTRY(user_data), subject);
}

static int is_selected_subject_row(void)
{
	GtkTreeSelection	*selection;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view_subject));
	return gtk_tree_selection_get_selected(selection, NULL, NULL);
}

static void on_button_pupil_login_clicked(GtkWidget *button, gpointer data)
{
	GtkTreeIter		iter;
	const gchar		*id_str;
	int				id;

	if (!gtk_combo_box_get_active_iter(GTK_COMBO_BOX(combo_box_pupil), &iter))
		return;

	gtk_tree_model_get(GTK_TREE_MODEL(store_pupil), &iter, 0, &id_str, -1);
	id = (int) g_ascii_strtoll(id_str, NULL, 10);
	if (check_pupil_login(id, gtk_entry_get_text(GTK_ENTRY(data))))
		login_pupil(id);
	else
		show_message_box("Неверный логин или пароль пользователя");
}

static void on_button_teacher_login_clicked(GtkWidget *button, gpointer data)
{
	if (check_teacher_login(1, gtk_entry_get_text(GTK_ENTRY(data))))
		login_teacher();
	else
		show_message_box("Неверный логин или пароль пользователя");
}

static void on_button_add_clicked(GtkWidget *button, gpointer data)
{
	void		*connection;
	char		*query;

	if (gtk_entry_get_text_length(GTK_ENTRY(data)) == 0)
		return;

	if (sql_open(DATABASE_FILENAME, &connection) != SQL_OK)
	{
		show_message_box(sql_error_msg(connection));
		sql_close(connection);
		return;
	}
	query = "SELECT MAX(id) FROM subject;";
	if (sql_exec(connection, query, select_max_subject_id_callback, NULL)
		!= SQL_OK)
	{
		show_message_box(sql_error_msg(connection));
	}
	query = g_strdup_printf(
		"INSERT INTO subject (id, subject) VALUES ('%d', '%s');",
		max_subject_id + 1, gtk_entry_get_text(GTK_ENTRY(data)));
	if (sql_exec(connection, query, NULL, NULL)
		!= SQL_OK)
	{
		show_message_box(sql_error_msg(connection));
	}
	g_free(query);
	sql_close(connection);
	load_subject();
}

static void on_button_update_clicked(GtkWidget *button, gpointer data)
{
	void		*connection;
	char		*query;

	if (gtk_entry_get_text_length(GTK_ENTRY(data)) == 0)
		return;

	if (!is_selected_subject_row())
		return;

	if (sql_open(DATABASE_FILENAME, &connection) != SQL_OK)
	{
		show_message_box(sql_error_msg(connection));
		sql_close(connection);
		return;
	}
	query = g_strdup_printf(
		"UPDATE subject SET subject = '%s' WHERE id = '%d';",
		gtk_entry_get_text(GTK_ENTRY(data)), selected_subject_id);
	if (sql_exec(connection, query, NULL, NULL)
		!= SQL_OK)
	{
		show_message_box(sql_error_msg(connection));
	}
	g_free(query);
	sql_close(connection);
	load_subject();
}

static void on_button_delete_clicked(GtkWidget *button, gpointer data)
{
	void		*connection;
	char		*query;

	if (selected_subject_id == 0)
		return;

	if (!is_selected_subject_row())
		return;

	if (sql_open(DATABASE_FILENAME, &connection) != SQL_OK)
	{
		show_message_box(sql_error_msg(connection));
		sql_close(connection);
		return;
	}
	query = g_strdup_printf(
		"DELETE FROM subject WHERE id = %d;", selected_subject_id);
	if (sql_exec(connection, query, NULL, NULL)
		!= SQL_OK)
	{
		show_message_box(sql_error_msg(connection));
	}
	g_free(query);
	sql_close(connection);
	load_subject();
}

static void show_message_box(const char *message)
{
	GtkWidget *dialog;

	dialog = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_MODAL,
									GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
									message);

	gtk_window_set_title(GTK_WINDOW(dialog), PROGRAM_TITLE);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

static void	load_subject(void)
{
	void		*connection;
	const char	*query;

	if (sql_open(DATABASE_FILENAME, &connection) != SQL_OK)
	{
		show_message_box(sql_error_msg(connection));
		sql_close(connection);
		return;
	}

	gtk_list_store_clear(store_subject);

	query = "SELECT id, subject FROM subject;";
	if (sql_exec(connection, query, show_subject_callback, 0) != SQL_OK)
	{
		show_message_box(sql_error_msg(connection));
		return;
	}

	sql_close(connection);
}

static void fill_pupil_store(void)
{
	void		*connection;
	const char	*query;

	if (sql_open(DATABASE_FILENAME, &connection) != SQL_OK)
	{
		show_message_box(sql_error_msg(connection));
		sql_close(connection);
		return;
	}

	gtk_list_store_clear(store_pupil);

	query = "SELECT id, pupil FROM pupil ORDER BY pupil;";
	if (sql_exec(connection, query, fill_pupil_callback, 0) != SQL_OK)
	{
		show_message_box(sql_error_msg(connection));
		return;
	}

	sql_close(connection);
}

static void fill_teacher_login(void)
{
	void		*connection;
	const char	*query;

	if (sql_open(DATABASE_FILENAME, &connection) != SQL_OK)
	{
		show_message_box(sql_error_msg(connection));
		sql_close(connection);
		return;
	}

	g_free(teacher_login);
	query = "SELECT teacher FROM teacher WHERE id = 1 LIMIT 1;";
	if (sql_exec(connection, query, fill_teacher_login_callback, 0)
		!= SQL_OK)
	{
		show_message_box(sql_error_msg(connection));
		return;
	}

	sql_close(connection);
}

static gboolean check_pupil_login(int id, const gchar *password)
{
	void		*connection;
	char		*query;

	if (sql_open(DATABASE_FILENAME, &connection) != SQL_OK)
	{
		show_message_box(sql_error_msg(connection));
		sql_close(connection);
		return FALSE;
	}
	is_pupil_password_match = FALSE;
	query = g_strdup_printf(
		"SELECT COUNT(*) FROM pupil WHERE id = '%d' AND password = '%s';",
		id, password);
	if (sql_exec(connection, query, check_pupil_password_callback, NULL)
		!= SQL_OK)
	{
		show_message_box(sql_error_msg(connection));
	}
	g_free(query);
	sql_close(connection);
	return is_pupil_password_match;
}

static gboolean check_teacher_login(int id, const gchar *password)
{
	void		*connection;
	char		*query;

	if (sql_open(DATABASE_FILENAME, &connection) != SQL_OK)
	{
		show_message_box(sql_error_msg(connection));
		sql_close(connection);
		return FALSE;
	}
	is_teacher_password_match = FALSE;
	query = g_strdup_printf(
		"SELECT COUNT(*) FROM teacher WHERE id = '%d' AND password = '%s';",
		id, password);
	if (sql_exec(connection, query, check_teacher_password_callback, NULL)
		!= SQL_OK)
	{
		show_message_box(sql_error_msg(connection));
	}
	g_free(query);
	sql_close(connection);
	return is_teacher_password_match;
}

static void login_pupil(int id)
{
	gtk_widget_hide(hbox_subject);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 2);
}

static void login_teacher(void)
{
	gtk_widget_show_all(hbox_subject);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 1);
}

static int show_subject_callback(void *opt_arg, int row_count, char **rows,
	char **col_name)
{
	GtkTreeIter		iter;

	gtk_list_store_append(store_subject, &iter);
	gtk_list_store_set(store_subject, &iter, 0, rows[0], 1, rows[1], -1);

    return 0;
}

static int fill_pupil_callback(void *opt_arg, int row_count, char **rows,
	char **col_name)
{
	GtkTreeIter		iter;

	gtk_list_store_append(store_pupil, &iter);
	gtk_list_store_set(store_pupil, &iter, 0, rows[0], 1, rows[1], -1);

    return 0;
}

static int fill_teacher_login_callback(void *opt_arg, int col_count,
	char **cols, char **col_names)
{
	teacher_login = g_strdup(cols[0]);
	return 0;
}

static int select_max_subject_id_callback(void *opt_arg, int col_count,
	char **cols, char **col_names)
{
	max_subject_id = g_ascii_strtoll(cols[0], NULL, 10);
	return 0;
}

static int check_pupil_password_callback(void *opt_arg, int col_count,
	char **cols, char **col_names)
{
	is_pupil_password_match = g_ascii_strtoll(cols[0], NULL, 10) != 0;
	return 0;
}

static int check_teacher_password_callback(void *opt_arg, int col_count,
	char **cols, char **col_names)
{
	is_teacher_password_match = g_ascii_strtoll(cols[0], NULL, 10) != 0;
	return 0;
}
