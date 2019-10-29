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

#include <stdio.h>
#include <stdlib.h>

#include <gtk/gtk.h>

#include "cursor.h"
#include "display.h"
#include "globals.h"
//#include "icon_bitmap"
#include "menu.h"
#include "status.h"

int main(int argc, char *argv[]) {
  GtkWidget *main_w;
  GtkWidget *vbox;
  //GdkBitmap *icon_bitmap;

  /* initialize gtk */
  gtk_init(&argc, &argv);

  if (argc > 2) {
    fprintf(stderr, "Error: too many arguments\n");
    fprintf(stderr, "Usage: %s [image_file_name]\n", argv[0]);
    exit(1);
  }

  init_globals();

  if (argc == 2) {
    globals.start_fname = argv[1];
  }

  /* the main window contains the work area and the menubar */
  main_w = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  if (main_w == NULL) {
    fprintf(stderr, "Error: failed to create main window\n");
    exit(1);
  }

  globals.topwin = main_w;
  gtk_widget_set_name(main_w, "pixelize");

  /*create the box that everyone goes in */
  vbox = gtk_vbox_new(FALSE, 0);

  if (vbox == NULL) {
    fprintf(stderr, "Error: failed to create the main box\n");
    exit(1);
  }

  gtk_container_add(GTK_CONTAINER(main_w), vbox);
  gtk_widget_show(vbox);

  /* set up the menu bar */
  setup_menu(vbox);

  /* handle window manager close */
  g_signal_connect(main_w, "delete_event", G_CALLBACK(delete_event), NULL);
  g_signal_connect(main_w, "destroy", G_CALLBACK(destroy), NULL);

  /* create the varrious subsystems */
  setup_display(vbox);
  setup_status(vbox);

  gtk_widget_show(main_w);

  //gdk_window_move(GGTK_WIDGET(main_w)->window, 100, 100);
  gdk_window_move(gtk_widget_get_window(GTK_WIDGET(main_w)), 100, 100);
  //gdk_window_set_hints(GTK_WIDGET(main_w)->window, 100, 100, 300, 300, 0, 0,
  //                     GDK_HINT_POS | GDK_HINT_MIN_SIZE);

#if 0
  /* Create pixmap of depth 1 (bitmap) for icon */
  icon_bitmap = gdk_bitmap_create_from_data(
      main_w->window, icon_bitmap_bits, icon_bitmap_width, icon_bitmap_height);

  if (icon_bitmap == NULL) {
    fprintf(stderr, "Error: failed to create icon pixmap\n");
    exit(1);
  }

  gdk_window_set_icon(main_w->window, NULL, icon_bitmap, NULL);
  //gdk_window_set_icon(main_w, NULL, icon_bitmap, NULL);
#endif

  cursor_normal();

  gtk_window_maximize(GTK_WINDOW(main_w));

  gtk_main();

  return 0;
}
