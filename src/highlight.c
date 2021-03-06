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

  return timeout_exists;
}

void highlight_changed() {

  /* redraw the screen */
  if (globals.draw_area) {
    gtk_widget_queue_draw(globals.draw_area);
  }
}

void start_highlight_timer() {

  /* start a timer if one does not already exist */
  if (!timeout_exists) {
    guint local_timer;
    timeout_exists = TRUE;
    globals.do_highlight |= DO_HIGHLIGHT_ON;
    local_timer = gdk_threads_add_timeout(200, highlight_timeout, NULL);

    if (local_timer == 0) {
      timeout_exists = FALSE;
      globals.do_highlight &= ~DO_HIGHLIGHT_ON;
      g_printerr("%s: failed to create timer\n", __func__);
    }
  }

  /* redraw the screen */
  if (globals.draw_area) {
    gtk_widget_queue_draw(globals.draw_area);
  }
}

void stop_highlight_timer() {

  if (timeout_exists) {
    timeout_exists = FALSE;
    globals.do_highlight &= ~DO_HIGHLIGHT_ON;
  }

  /* redraw the screen */
  if (globals.draw_area) {
    gtk_widget_queue_draw(globals.draw_area);
  }
}
