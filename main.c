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
/* main.c by Paul Wilkins 1/2/2000 */

/* main.c by Paul Wilkins.
 * This file contains the main program.
 */

#include "cursor.h"
#include "display.h"
#include "globals.h"
#include "menu.h"
#include "pixelize_model.h"
#include "status.h"

#if 1
static gboolean delete_event_callback(GtkWidget *widget, GdkEvent *event,
                                      gpointer user_data) {
  (void)widget;
  (void)event;
  (void)user_data;

  return FALSE; /* will call our destroy function */
}

static void destroy_callback(GtkWidget *widget, gpointer data) {
  (void)widget;
  (void)data;

  gtk_main_quit();
}
#endif

int main(int argc, char *argv[]) {
  GtkBuilder *builder;
  // GObject *window;
  // GObject *button;
  GError *error = NULL;
  GOptionEntry entries[] = {
      {"in", 'i', 0, G_OPTION_ARG_STRING, &globals.in_fname, "Input file name",
       "file_name"},
      {"out", 'o', 0, G_OPTION_ARG_STRING, &globals.out_fname,
       "Output file name", "file_name"},
      {"mode", 'm', 0, G_OPTION_ARG_NONE, &globals.command_mode,
       "Command line mode", NULL},
      {NULL},
  };

  init_globals();

  /* initialize gtk */
  if (gtk_init_with_args(&argc, &argv, "toto", entries, NULL, NULL)) {

    if (argc > 2) {
      g_printerr("%s: Error: too many arguments\n", argv[0]);
      exit(1);
    }

    /*
     * if a file name was provided, save it the global struct
     */
    if (argc == 2) {
      if (globals.in_fname == NULL) {
        globals.in_fname = g_strdup(argv[1]);
      } else {
        g_printerr("%s: Error: you provided 2 input files\n", argv[0]);
        exit(1);
      }
    }

    /*
     * check that we got all required argument if non interactive mode was
     * selected
     */
    if (globals.command_mode &&
        (globals.out_fname == NULL || globals.in_fname == NULL)) {
      g_printerr("%s: Error: Invalid arguments\n", argv[0]);
      g_printerr(
          "%s: you need to define an input file and and an ouput file in "
          "non interactive mode\n",
          argv[0]);
      exit(1);
    }

    builder = gtk_builder_new();

    if (builder) {

      if (gtk_builder_add_from_string(builder, gladestring, -1, &error) == 0) {
        g_printerr("%s: Error loading file: %s\n", argv[0], error->message);
        g_clear_error(&error);
        exit(1);
      }

      globals.topwin = gtk_builder_get_object(builder, "main_window");

      g_print("OK\n");
#if 0
      /* the main window contains the work area and the menubar */
      globals.topwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
#endif

      if (globals.topwin) {
#if 0
        GtkWidget *grid;

        gtk_widget_set_name(globals.topwin, "pixelize");
        gtk_window_set_default_size(GTK_WINDOW(globals.topwin), 300, 300);
        gtk_window_set_position(GTK_WINDOW(globals.topwin), GTK_WIN_POS_CENTER);
        gtk_container_set_border_width(GTK_CONTAINER(globals.topwin), 15);
#endif

        /* handle window manager close */
        g_signal_connect(globals.topwin, "delete_event",
                         G_CALLBACK(delete_event_callback), NULL);
        g_signal_connect(globals.topwin, "destroy",
                         G_CALLBACK(destroy_callback), NULL);

#if 0
        /*create the grid that everyone goes in */
        grid = gtk_grid_new();

        if (grid) {

          /* add the grid to the top window */
          gtk_container_add(GTK_CONTAINER(globals.topwin), grid);
          gtk_widget_show(grid);

          /* set up the menu bar */
          setup_menu(grid);

          /* create the varrious subsystems */
          setup_display(grid);

          setup_status(grid);

        } else {
          fprintf(stderr, "Error: failed to create the main box\n");
          exit(1);
        }

        gtk_widget_show(globals.topwin);

        cursor_normal();

        // gtk_window_maximize(GTK_WINDOW(globals.topwin));
        if (globals.in_im_scaled) {
          g_thread_new("render", render_compute_thread, NULL);
        }
#endif

        gtk_main();
      } else {
        fprintf(stderr, "Error: failed to create main window\n");
        exit(1);
      }

      return 0;
    }
  } else {
    fprintf(stderr, "Error: in parameters\n");
    return -1;
  }
}
