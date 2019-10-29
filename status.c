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

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
/* #include <gdk_imlib.h> */

#include "globals.h"
#include "status.h"

static GtkWidget *mode_display;
static GtkWidget *progress_bar;

void set_progress_indicator(double val) {
  static int last_percent = -1;

  /* don't bother updating if we haven't changed by al least 1 percent */
  if (last_percent != (int)(100.0 * val)) {
    last_percent = (int)(100.0 * val);

    /* update the graph */
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), val);

    /* update the text */
    if (val == 1) {
      gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress_bar), "Ready");
    } else {
      char text[4];
      sprintf(text, "%d%%", (int)(100.0 * val));
      gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress_bar), text);
    }

    /* force an update NOW */
    gdk_window_invalidate_rect(gtk_widget_get_window(GTK_WIDGET(progress_bar)), NULL, TRUE);
    //gtk_widget_draw(GTK_WIDGET(progress_bar), NULL);
    gdk_flush();
  }
}

void refresh_mode_display() {
  char buf[256];

  if (globals.cur_opt.opt_alg == PIX_SIZE) {
    sprintf(buf, " Size of images: %ux%u", globals.cur_opt.pixW,
            globals.cur_opt.pixH);
  } else {
    sprintf(buf, " Number of Images: %ux%u", globals.cur_opt.nPixW,
            globals.cur_opt.nPixH);
  }

  gtk_label_set_text(GTK_LABEL(mode_display), buf);
}

GtkWidget *setup_status(GtkWidget *parent) {
  GtkWidget *hbox;

  /* create the hbox that will hold our widgets */
  hbox = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_end(GTK_BOX(parent), hbox, FALSE, FALSE, 0);
  gtk_widget_show(hbox);

  /* first is a progressbar */
  progress_bar = gtk_progress_bar_new();

  /* Set the progress bar format */
  // gtk_progress_bar_set_bar_style(GTK_PROGRESS_BAR(progress_bar),
  // GTK_PROGRESS_CONTINUOUS);
  gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(progress_bar),
                                   GTK_PROGRESS_LEFT_TO_RIGHT);
  // gtk_progress_set_activity_mode(GTK_PROGRESS(progress_bar), FALSE);
  // gtk_progress_set_show_text(GTK_PROGRESS(progress_bar), TRUE);
  // toggle_progress_indicator(ST_READY);

  gtk_box_pack_start(GTK_BOX(hbox), progress_bar, FALSE, FALSE, 0);
  gtk_widget_show(progress_bar);

  /* now a label to display stuff */
  mode_display = gtk_label_new(" ");
  gtk_box_pack_start(GTK_BOX(hbox), mode_display, FALSE, FALSE, 0);
  gtk_widget_show(mode_display);

  /* set the string */
  refresh_mode_display();

  return hbox;
}
