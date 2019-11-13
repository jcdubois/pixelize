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

static GtkWidget *mode_display;
static GtkWidget *progress_bar;

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
      /* update the graph */
      gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), val);

      /* update the text */
      if (val == 1) {
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress_bar), "Ready");
      } else if (val < 1 && val >= 0) {
        char text[4];
        snprintf(text, sizeof(text), "%d%%", (int)(100.0 * val));
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress_bar), text);
      }

      /* force an update NOW */
      /* We ask the main thread to update the progress bar */
      g_idle_add(update_gui_callback, progress_bar);
    }
  }
}

void refresh_mode_display() {
  if (mode_display) {
    char buf[256];

    if (globals.cur_opt.opt_alg == PIX_SIZE) {
      snprintf(buf, sizeof(buf), " Size of images: %ux%u", globals.cur_opt.pixW,
               globals.cur_opt.pixH);
    } else {
      snprintf(buf, sizeof(buf), " Number of Images: %ux%u",
               globals.cur_opt.nPixW, globals.cur_opt.nPixH);
    }

    gtk_label_set_text(GTK_LABEL(mode_display), buf);

    /* We ask the main thread to update the mode information */
    g_idle_add(update_gui_callback, mode_display);
  }
}

void setup_status(GtkWidget *parent) {

  /* first is a progressbar */
  progress_bar = gtk_progress_bar_new();

  if (progress_bar) {
    /* Set the progress bar format */
    gtk_orientable_set_orientation(GTK_ORIENTABLE(progress_bar),
                                   GTK_ORIENTATION_HORIZONTAL);
    gtk_progress_bar_set_inverted(GTK_PROGRESS_BAR(progress_bar), FALSE);
    gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(progress_bar), TRUE);

    gtk_grid_attach(GTK_GRID(parent), progress_bar, 0, 2, 1, 1);
    gtk_widget_show(progress_bar);

    set_progress_indicator(0.0);
  }

  /* now a label to display stuff */
  mode_display = gtk_label_new(" ");

  if (mode_display) {
    gtk_grid_attach(GTK_GRID(parent), mode_display, 1, 2, 1, 1);
    gtk_widget_show(mode_display);

    /* set the string */
    refresh_mode_display();
  }
}
