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

#ifdef USE_GNOME
#include <gnome.h>
#endif
#include <gtk/gtk.h>

#include "globals.h"
#include "icon_bitmap"
#include "display.h"
#include "status.h"
#include "menu.h"
#include "cursor.h"

void usage(char *str){
   fprintf(stderr, "Error: %s\n", str);
   fprintf(stderr, "Usage: pixelize\n");
}


int main(int argc, char *argv[])
{
   GtkWidget *main_w;
   GtkWidget *vbox;
   GtkWidget *menu;
   GdkBitmap *icon_bitmap;
#ifdef USE_GNOME
   GnomeAppBar *appbar;
#endif

#ifdef USE_GNOME
   gnome_init("pixelize", "1.0", argc, argv);
   gnome_app_new("pixelize", "pixelize");
#else
   /* initialize gtk */
   gtk_init(&argc, &argv);
#endif


   if(argc > 2){
      usage("Unknown Argument.");
      exit(1);
   }

   init_globals();

   if(argc == 2){
      globals.start_fname = argv[1];
   }

#ifdef USE_GNOME
   main_w = gnome_app_new("pixelize", "pixelize");
   globals.topwin = main_w;
   setup_menu(main_w);

   appbar = GNOME_APPBAR(gnome_appbar_new(FALSE, FALSE, FALSE));
   gnome_app_set_statusbar(GNOME_APP(main_w), GTK_WIDGET(appbar));

   /*create the box that everyone goes in */
   vbox = gtk_vbox_new(FALSE, 0);
   gnome_app_set_contents(GNOME_APP(main_w), vbox);
   gtk_widget_show(vbox);

#else
   /* the main window contains the work area and the menubar */
   main_w = gtk_window_new(GTK_WINDOW_TOPLEVEL);
   globals.topwin = main_w;
   gtk_widget_set_name(main_w, "pixelize");

   /*create the box that everyone goes in */
   vbox = gtk_vbox_new(FALSE, 0);
   gtk_container_add(GTK_CONTAINER(main_w), vbox);
   gtk_widget_show(vbox);

   /* set up the menu bar */
   menu = setup_menu(vbox);
#endif

   /* handle window manager close */
   gtk_signal_connect(GTK_OBJECT(main_w), "delete_event",
      GTK_SIGNAL_FUNC(delete_event), NULL);
   gtk_signal_connect(GTK_OBJECT(main_w), "destroy",
      GTK_SIGNAL_FUNC(destroy), NULL);

   /* create the varrious subsystems */
   setup_display(vbox);
   setup_status(vbox);

   gtk_widget_show(main_w);

   gdk_window_move(GTK_WIDGET(main_w)->window, 100, 100);
   gdk_window_set_hints(GTK_WIDGET(main_w)->window,
      100, 100, 300, 300, 0, 0, 
      GDK_HINT_POS|GDK_HINT_MIN_SIZE);


   /* Create pixmap of depth 1 (bitmap) for icon */
   icon_bitmap = gdk_bitmap_create_from_data(main_w->window,
      icon_bitmap_bits, icon_bitmap_width, icon_bitmap_height);
   gdk_window_set_icon(main_w->window, NULL, icon_bitmap, NULL);

   cursor_normal();

   gtk_main();

   return 0;
}
