/*

Copyright (C) 2000  Paul Wilkins

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
/* setup_menu.c by Paul Wilkins 1/2/2000 */

#include "menu.h"
#include "cursor.h"
#include "file_dialog.h"
#include "options.h"
#include "render.h"

gpointer render_compute_thread(gpointer data) {
  (void)data;

  if (globals.in_im_scaled) {
    /* compute the new image */
    render();
  }

  /* save it if a file name was provided */
  if (globals.out_im && globals.out_fname) {
    save_image();
  }

  /* Exit if we are in non interactive mode */
  if (globals.command_mode) {
    gtk_main_quit();
  }

  return NULL;
}

static void render_callback(GtkMenuItem *menuitem, gpointer user_data) {
  (void)menuitem;
  (void)user_data;

  g_thread_new("render", render_compute_thread, NULL);
}

static void open_callback(GtkMenuItem *menuitem, gpointer user_data) {
  (void)menuitem;

  GtkWidget *dialog = GTK_WIDGET(user_data);

  if (dialog) {
    gint res = gtk_dialog_run(GTK_DIALOG(dialog));

    if (res == GTK_RESPONSE_ACCEPT) {
      GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);

      if (chooser) {
        char *filename = gtk_file_chooser_get_filename(chooser);

        if (filename) {

          cursor_busy();

          if (globals.in_fname) {
            g_free(globals.in_fname);
          }
          globals.in_fname = filename;

          if (globals.out_fname) {
            g_free(globals.out_fname);
            globals.out_fname = NULL;
          }

          open_image();

          cursor_normal();
        }
      }
    }

    gtk_widget_hide(dialog);
  }
}

static void save_callback(GtkMenuItem *menuitem, gpointer user_data) {
  (void)menuitem;

  GtkWidget *dialog = GTK_WIDGET(user_data);

  if (dialog) {
    GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);

    if (chooser) {
      gint res;

      gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);

      gtk_file_chooser_set_current_name(
          chooser, globals.out_fname ? globals.out_fname : "Untitled document");

      res = gtk_dialog_run(GTK_DIALOG(dialog));

      if (res == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(chooser);

        if (filename) {

          cursor_busy();

          if (globals.out_fname) {
            g_free(globals.out_fname);
          }
          globals.out_fname = filename;

          save_image();

          cursor_normal();
        }
      }
    }

    gtk_widget_hide(dialog);
  }
}

static void about_callback(GtkMenuItem *menuitem, gpointer user_data) {
  (void)menuitem;
  (void)user_data;

  GtkWidget *dialog = GTK_WIDGET(user_data);

  if (dialog) {
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_hide(dialog);
  } else {
    g_printerr("Error: Can't get about dialog box\n");
  }
}

/* create the menubar */
void setup_menu(GtkBuilder *builder) {

  GtkWidget *menu_item;

  /* Open */
  menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "file_menu_open"));
  if (menu_item) {
    GtkWidget *dialog =
        GTK_WIDGET(gtk_builder_get_object(builder, "open_file_dialog"));

    if (dialog) {
      g_signal_connect(menu_item, "activate", G_CALLBACK(open_callback),
                       dialog);
    } else {
      g_printerr("Error: Can't locate open file dialog\n");
      exit(1);
    }
  } else {
    g_printerr("Error: Can't locate open file menu item\n");
    exit(1);
  }

  /* Save */
  menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "file_menu_save"));
  if (menu_item) {
    GtkWidget *dialog =
        GTK_WIDGET(gtk_builder_get_object(builder, "save_file_dialog"));

    if (dialog) {
      g_signal_connect(menu_item, "activate", G_CALLBACK(save_callback),
                       dialog);
    } else {
      g_printerr("Error: Can't locate save file dialog\n");
      exit(1);
    }
  } else {
    g_printerr("Error: Can't locate save file menu item\n");
    exit(1);
  }

  /* Quit */
  menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "file_menu_quit"));
  if (menu_item) {
    g_signal_connect(menu_item, "activate", G_CALLBACK(gtk_main_quit), NULL);
  } else {
    g_printerr("Error: Can't locate quit menu item\n");
    exit(1);
  }

  /* Render */
  menu_item =
      GTK_WIDGET(gtk_builder_get_object(builder, "options_menu_render"));
  if (menu_item) {
    g_signal_connect(menu_item, "activate", G_CALLBACK(render_callback), NULL);
  } else {
    g_printerr("Error: Can't locate render menu item\n");
    exit(1);
  }

  /* Configure */
  menu_item =
      GTK_WIDGET(gtk_builder_get_object(builder, "options_menu_configure"));
  if (menu_item) {
    GtkWidget *dialog =
        GTK_WIDGET(gtk_builder_get_object(builder, "options_dialog"));

    if (dialog) {
      g_signal_connect(menu_item, "activate", G_CALLBACK(optionsCB), dialog);
    } else {
      g_printerr("Error: Can't locate options dialog\n");
      exit(1);
    }
  } else {
    g_printerr("Error: Can't locate render menu item\n");
    exit(1);
  }

  /* About */
  menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "about_menu_item"));
  if (menu_item) {
    GtkWidget *dialog =
        GTK_WIDGET(gtk_builder_get_object(builder, "about_dialog"));

    if (dialog) {
      g_signal_connect(menu_item, "activate", G_CALLBACK(about_callback),
                       dialog);
    } else {
      g_printerr("Error: Can't locate about dialog\n");
      exit(1);
    }
  } else {
    g_printerr("Error: Can't locate about menu item\n");
    exit(1);
  }
}
