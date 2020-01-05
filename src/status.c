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
/* status.c by Paul Wilkins 1/2/2000 */

#include "status.h"
#include "display.h"
#include "globals.h"

static GtkWidget *progress_bar = NULL;

void set_progress_indicator(double val) {
  static int last_percent = -1;

  /* make sure val is in the correct range */
  if (val > 1) {
    val = 1;
  } else if (val < 0) {
    val = 0;
  }

  /* don't bother updating if we haven't changed by al least 1 percent */
  if (last_percent != (int)(100.0 * val)) {
    last_percent = (int)(100.0 * val);

    /* we can update the progress bar if its is created */
    if (progress_bar) {
      if (val == 1) {
        /* update the text */
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress_bar), "Ready");
        /* Reset the bar to 0 */
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), 0);
      } else if (val < 1 && val >= 0) {
        char text[4];
        /* update the text */
        snprintf(text, sizeof(text), "%d%%", (int)(100.0 * val));
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress_bar), text);
        /* update the graph */
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), val);
      }

      /* force an update NOW */
      /* We ask the main thread to update the progress bar */
      gdk_threads_add_idle(update_gui_callback, progress_bar);
    }
  }
}

void refresh_mode_display() {
  if (globals.topwin) {
    char buf[256];

    if (globals.cur_opt.opt_alg == PIX_SIZE) {
      snprintf(buf, sizeof(buf), "Pixelize - Size of images: %ux%u",
               globals.cur_opt.pixW, globals.cur_opt.pixH);
    } else {
      snprintf(buf, sizeof(buf), "Pixelize - Number of Images: %ux%u",
               globals.cur_opt.nPixW, globals.cur_opt.nPixH);
    }

    gtk_window_set_title(GTK_WINDOW(globals.topwin), buf);

    /* We ask the main thread to update the mode information */
    gdk_threads_add_idle(update_gui_callback, globals.topwin);
  }
}

void setup_status(GtkWidget *dialog) {

  progress_bar = find_child(dialog, "progress_bar");

  if (progress_bar) {
    set_progress_indicator(0.0);
  }
}
