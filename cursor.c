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
/* cursor.c by Paul Wilkins 1/2/2000 */

#include "cursor.h"
#include "globals.h"

void cursor_busy() {
  static GdkCursor *cur = NULL;

  if (cur == NULL) {
    cur = gdk_cursor_new_for_display(gdk_display_get_default(), GDK_WATCH);
  }

  gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(globals.topwin)), cur);
}

void cursor_normal() {
  static GdkCursor *cur = NULL;

  if (cur == NULL) {
    cur = gdk_cursor_new_for_display(gdk_display_get_default(),
                                     GDK_TOP_LEFT_ARROW);
  }

  gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(globals.topwin)), cur);
}
