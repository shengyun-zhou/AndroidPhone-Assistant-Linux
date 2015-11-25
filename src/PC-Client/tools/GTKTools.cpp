#include "GTKTools.h"
#include <gtk/gtk.h>
#include <string.h>

void GTKTools::init_gtk()
{
	gtk_init(NULL, NULL);
}

string GTKTools::get_current_theme_name()
{
	GtkSettings* settings = gtk_settings_get_default();
	if(settings)
	{
		gchar* theme_name = NULL;
		g_object_get((GObject*)settings, "gtk-icon-theme-name", &theme_name, NULL);
		if(theme_name)
			return theme_name;
		return "";
	}
	return "";
}

string GTKTools::get_open_file_name(const char* title, const char* filter_name, const char* filter_pattern)
{
	GtkFileChooserDialog* file_dialog = GTK_FILE_CHOOSER_DIALOG(gtk_file_chooser_dialog_new(title, NULL, GTK_FILE_CHOOSER_ACTION_OPEN,
                                                              GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                                              GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL));
	GtkFileFilter* filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, filter_name);
	gtk_file_filter_add_pattern(filter, filter_pattern);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_dialog), filter);
	int ret = gtk_dialog_run(GTK_DIALOG(file_dialog));
	if(ret != GTK_RESPONSE_ACCEPT)
	{
		gtk_widget_destroy(GTK_WIDGET(file_dialog));
		return "";
	}
	else
	{
		char file_path[2048] = {0};
		strcpy(file_path, gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_dialog)));
		gtk_widget_destroy(GTK_WIDGET(file_dialog));
		return file_path;
	}
}

string GTKTools::get_save_file_name(const char* title, const char* filter_name, const char* filter_pattern)
{
	GtkFileChooserDialog* file_dialog = GTK_FILE_CHOOSER_DIALOG(gtk_file_chooser_dialog_new(title, NULL, GTK_FILE_CHOOSER_ACTION_SAVE,
                                                              GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                                              GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL));
	GtkFileFilter* filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, filter_name);
	gtk_file_filter_add_pattern(filter, filter_pattern);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_dialog), filter);
	gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(file_dialog), TRUE);
	int ret = gtk_dialog_run(GTK_DIALOG(file_dialog));
	if(ret != GTK_RESPONSE_ACCEPT)
	{
		gtk_widget_destroy(GTK_WIDGET(file_dialog));
		return "";
	}
	else
	{
		char file_path[2048] = {0};
		strcpy(file_path, gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_dialog)));
		gtk_widget_destroy(GTK_WIDGET(file_dialog));
		return file_path;
	}
}

string GTKTools::get_directory_name(const char* title)
{
	GtkFileChooserDialog* file_dialog = GTK_FILE_CHOOSER_DIALOG(gtk_file_chooser_dialog_new(title, NULL, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                                              GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                                              GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL));
	int ret = gtk_dialog_run(GTK_DIALOG(file_dialog));
	if(ret != GTK_RESPONSE_ACCEPT)
	{
		gtk_widget_destroy(GTK_WIDGET(file_dialog));
		return "";
	}
	else
	{
		char file_path[2048] = {0};
		strcpy(file_path, gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_dialog)));
		gtk_widget_destroy(GTK_WIDGET(file_dialog));
		return file_path;
	}
}
