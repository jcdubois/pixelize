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
#include "globals.h"
#include "options.h"
#include "render.h"

gpointer render_compute_thread(gpointer data) {
  (void)data;

  if (globals.in_fname && !globals.in_im_scaled) {
    open_image();
  }

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

void options_menu_render_activate_cb(GtkMenuItem *menuitem,
                                     gpointer user_data) {
  (void)menuitem;
  (void)user_data;

  if (globals.in_im_scaled) {
    g_thread_new("render", render_compute_thread, NULL);
  } else {
    g_printerr("Error: No image loaded\n");
  }
}

void file_menu_open_activate_cb(GtkMenuItem *menuitem, gpointer user_data) {
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

          if (globals.in_im_scaled) {
            globals.show_rendered = FALSE;
	  }

          if (globals.draw_area) {
            gtk_widget_queue_draw(globals.draw_area);
          }

          cursor_normal();
        }
      }
    }

    gtk_widget_hide(dialog);
  }
}

void file_menu_save_activate_cb(GtkMenuItem *menuitem, gpointer user_data) {
  (void)menuitem;

  GtkWidget *dialog = GTK_WIDGET(user_data);

  if (dialog) {
    GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);

    if (chooser) {
      gint res = gtk_dialog_run(GTK_DIALOG(dialog));

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

void about_menu_item_activate_cb(GtkMenuItem *menuitem, gpointer user_data) {
  (void)menuitem;

  GtkWidget *dialog = GTK_WIDGET(user_data);

  if (dialog) {
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_hide(dialog);
  } else {
    g_printerr("Error: Can't get about dialog box\n");
  }
}

void options_menu_configure_activate_cb(GtkMenuItem *menuitem,
                                        gpointer user_data) {
  (void)menuitem;

  GtkWidget *dialog = GTK_WIDGET(user_data);

  if (dialog) {
    refresh_options_win(dialog);

    gint res = gtk_dialog_run(GTK_DIALOG(dialog));

    if (res == GTK_RESPONSE_ACCEPT) {
    }

    gtk_widget_hide(dialog);
  } else {
    g_printerr("Error: Can't get options dialog box\n");
  }
}
