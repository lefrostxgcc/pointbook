#include <gtk/gtk.h>
#include <sqlite3.h>

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

	g_message("%s\n", sqlite3_libversion());

    gtk_main();
}
