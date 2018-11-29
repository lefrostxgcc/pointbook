#include <gtk/gtk.h>
#include <sqlite3.h>
#include <config.h>
#include "chip_sqlite.h"

#define		PROGRAM_TITLE	"Книжка оценок"

int main(int argc, char *argv[])
{
	GtkWidget *window;

    gtk_init (&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), PROGRAM_TITLE);

	g_signal_connect(G_OBJECT(window), "destroy",
						G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);

	/*if (chip_sqlite_open(DATA_PATH "/" DATABASE_NAME) != NULL)
		g_message("OK");
	else
		g_message("FAIL");*/

    gtk_main();
}
