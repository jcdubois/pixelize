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
/* highlight.c by Paul Wilkins 1/2/2000 */

#include "highlight.h"
#include "globals.h"
#include "render_image.h"

static gboolean timeout_exists = FALSE;

static gboolean highlight_timeout(gpointer data) {
  (void)data;

  globals.do_highlight ^= DO_HIGHLIGHT_ON;

  /* redraw the screen */
  if (globals.draw_area) {
    gtk_widget_queue_draw(globals.draw_area);
  }

  return TRUE;
}

void highlight_changed() {

  /* redraw the screen */
  if (globals.draw_area) {
    gtk_widget_queue_draw(globals.draw_area);
  }
}

static guint local_timer = 0;

void start_highlight_timer() {

  /* start a timer if one does not already exist */
  if (!timeout_exists) {
    timeout_exists = TRUE;
    globals.do_highlight |= DO_HIGHLIGHT_ON;
    local_timer = g_timeout_add(200, highlight_timeout, NULL);
  }

  /* redraw the screen */
  if (globals.draw_area) {
    gtk_widget_queue_draw(globals.draw_area);
  }
}

void stop_highlight_timer() {

  if (timeout_exists) {
    if (local_timer) {
      g_source_remove(local_timer);
    }
    timeout_exists = FALSE;
    globals.do_highlight &= ~DO_HIGHLIGHT_ON;
  }

  /* redraw the screen */
  if (globals.draw_area) {
    gtk_widget_queue_draw(globals.draw_area);
  }
}
