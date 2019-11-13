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

int main(int argc, char *argv[]) {
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

  /* initialize gtk with args */
  if (gtk_init_with_args(&argc, &argv, "toto", entries, NULL, NULL)) {
    GtkBuilder *builder;
    GError *error = NULL;

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

      globals.topwin =
          GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));

      if (globals.topwin) {

        /* handle window manager close */
        g_signal_connect(globals.topwin, "destroy", G_CALLBACK(gtk_main_quit),
                         NULL);

        /* set up the menu bar */
        setup_menu(builder);

#if 0
        /* create the varrious subsystems */
        setup_display(builder);

        setup_status(builder);
#endif

        gtk_widget_show(globals.topwin);

        cursor_normal();

        // gtk_window_maximize(GTK_WINDOW(globals.topwin));

        if (globals.in_im_scaled) {
          g_thread_new("render", render_compute_thread, NULL);
        }

        gtk_main();
      } else {
        g_printerr("%s: Error: failed to create main window\n", argv[0]);
        exit(1);
      }

      return 0;
    }
  } else {
    g_printerr("%s: Error: in parameters\n", argv[0]);
    exit(1);
  }
}
