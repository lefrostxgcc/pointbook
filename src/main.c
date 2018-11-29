#include <gtk/gtk.h>
#include <sqlite3.h>
#include <config.h>
#include "chip_sqlite.h"

#define		PROGRAM_TITLE	"Книжка оценок"
enum {WINDOW_WIDTH = 600, WINDOW_HEIGHT = 400};

static GtkWidget	*create_subject_page(void);
static GtkWidget	*create_pupil_page(void);
static void setup_tree_view(GtkWidget *tree_view);
static void on_button_add_clicked(GtkWidget *button, gpointer data);
static void on_button_update_clicked(GtkWidget *button, gpointer data);
static void on_button_delete_clicked(GtkWidget *button, gpointer data);
static int show_subject_callback(void *NotUsed, int argc, char **argv,
	char **azColName);
static void	load_subject(void);

static	GtkListStore *store_subject;

int main(int argc, char *argv[])
{
	GtkWidget *window, *notebook, *label_subject, *label_pupil;

    gtk_init (&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), PROGRAM_TITLE);
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);
	gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_WIDTH, WINDOW_HEIGHT);

	label_subject = gtk_label_new ("Список предметов");
	label_pupil = gtk_label_new ("Список учеников");

	notebook = gtk_notebook_new();
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), create_subject_page(),
		label_subject);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), create_pupil_page(),
		label_pupil);

	gtk_container_add(GTK_CONTAINER(window), notebook);

	g_signal_connect(G_OBJECT(window), "destroy",
						G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);

	sqlite3 *db;

	if ((db = chip_sqlite_open(DATA_PATH "/" DATABASE_NAME)) != NULL)
		g_message("OK open");
	else
		g_message("FAIL open");

	if (chip_sqlite_close(db) == SQLITE_OK)
		g_message("OK close");
	else
		g_message("FAIL close");

    gtk_main();
	g_object_unref(store_subject);
}

static GtkWidget	*create_subject_page(void)
{
	GtkWidget	*hbox;
	GtkWidget	*vbox;
	GtkWidget	*tree_view;
	GtkWidget	*entry_subject;
	GtkWidget	*button_update;
	GtkWidget	*button_add;
	GtkWidget	*button_delete;
	GtkWidget	*space;

	tree_view = gtk_tree_view_new();
	entry_subject = gtk_entry_new();
	button_update = gtk_button_new_with_label("Изменить");
	button_add = gtk_button_new_with_label("Добавить");
	button_delete = gtk_button_new_with_label("Удалить");
	space = gtk_label_new(NULL);

	setup_tree_view(tree_view);

	store_subject = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);

	gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view),
		GTK_TREE_MODEL(store_subject));

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

	gtk_box_pack_start(GTK_BOX(vbox), entry_subject, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), button_add, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), button_update, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), button_delete, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), space, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), tree_view, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, FALSE, 5);

	g_signal_connect(G_OBJECT(button_add), "clicked",
						G_CALLBACK(on_button_add_clicked),
						(gpointer)entry_subject);
	g_signal_connect(G_OBJECT(button_update), "clicked",
						G_CALLBACK(on_button_update_clicked),
						(gpointer)entry_subject);
	g_signal_connect(G_OBJECT(button_delete), "clicked",
						G_CALLBACK(on_button_delete_clicked),
						(gpointer)entry_subject);

	return hbox;
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
		g_object_set(render, "editable", TRUE, NULL);
		column = gtk_tree_view_column_new_with_attributes(headers[i],
			render, "text", i, NULL);
		gtk_tree_view_column_set_resizable(column, TRUE);
		gtk_tree_view_column_set_min_width(column, 20);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
	}

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
	gtk_tree_selection_set_mode(selection, GTK_SELECTION_BROWSE);
}

static void	load_subject(void)
{
	sqlite3			*db;
	char			*err_msg;
	char			*sql;
	sqlite3_stmt	*res;
	int				rc;
	int				index;

	rc = sqlite3_open(DATA_PATH "/" DATABASE_NAME, &db);
	if (rc != SQLITE_OK)
	{
		g_warning("Cannot open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return;
	}

	gtk_list_store_clear(store_subject);

	sql = "SELECT id, subject FROM subject;";
	rc = sqlite3_exec(db, sql, show_subject_callback, 0, &err_msg);
	if (rc != SQLITE_OK )
	{
		g_warning("Cannot open database: %s\n", sqlite3_errmsg(db));
		sqlite3_free(err_msg);
		sqlite3_close(db);
        return;
	}    
    sqlite3_close(db);
}

static int show_subject_callback(void *NotUsed, int argc, char **argv,
	char **azColName)
{
	GtkTreeModel	*model;
	GtkTreeIter		iter;

    (void) NotUsed;

	gtk_list_store_append(store_subject, &iter);
	gtk_list_store_set(store_subject, &iter,
		0, argv[0],
		1, argv[1],
		-1);

    return 0;
}

static void on_button_add_clicked(GtkWidget *button, gpointer data)
{
	sqlite3			*db;
	char			*err_msg;
	char			*sql;
	sqlite3_stmt	*res;
	int				rc;
	int				index;

	if (gtk_entry_get_text_length(GTK_ENTRY(data)) == 0)
		return;

	rc = sqlite3_open(DATA_PATH "/" DATABASE_NAME, &db);
	if (rc != SQLITE_OK)
	{
		g_warning("Cannot open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return;
	}

	sql = "SELECT MAX(id) FROM subject;";
	rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
	if (rc != SQLITE_OK)
	{
		g_warning("Failed to fetch data: %s", sqlite3_errmsg(db));
		sqlite3_finalize(res);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(res);
	if (rc == SQLITE_ROW)
	    index = sqlite3_column_int(res, 0);
	index++;
	sqlite3_finalize(res);

	sql = "INSERT INTO subject (id, subject) VALUES (?, ?);";
	rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
	if (rc == SQLITE_OK)
	{
		sqlite3_bind_int(res, 1, index);
		sqlite3_bind_text(res, 2, gtk_entry_get_text(GTK_ENTRY(data)), -1, NULL);
	}
	else
	{
		g_warning("Failed to execute statement: %s", sqlite3_errmsg(db));
		sqlite3_finalize(res);
		sqlite3_close(db);
		return;
	}
	sqlite3_step(res);
    sqlite3_finalize(res);
    sqlite3_close(db);

	g_message("%d", index);
}

static void on_button_update_clicked(GtkWidget *button, gpointer data)
{
	g_message("update");
}

static void on_button_delete_clicked(GtkWidget *button, gpointer data)
{
	g_message("delete");
}

static GtkWidget	*create_pupil_page(void)
{
	return gtk_label_new(NULL);
}
