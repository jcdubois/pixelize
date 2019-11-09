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
/* options.c by Paul Wilkins 1/2/2000 */

#include "options.h"
#include "draw_image.h"
#include "globals.h"
#include <ctype.h>

static GtkWidget *optWindow = NULL;
static GtkWidget *size_x_entry = NULL;
static GtkWidget *size_y_entry = NULL;
static GtkWidget *count_x_entry = NULL;
static GtkWidget *count_y_entry = NULL;
static GtkWidget *size_button = NULL;
static GtkWidget *count_button = NULL;

void refresh_options_win(struct _ImageOptions *im_opt) {
  char buf[8];

  if (optWindow) {
    snprintf(buf, sizeof(buf), "%u", im_opt->pixW);
    gtk_entry_set_text(GTK_ENTRY(size_x_entry), buf);
    snprintf(buf, sizeof(buf), "%u", im_opt->pixH);
    gtk_entry_set_text(GTK_ENTRY(size_y_entry), buf);
    snprintf(buf, sizeof(buf), "%u", im_opt->nPixW);
    gtk_entry_set_text(GTK_ENTRY(count_x_entry), buf);
    snprintf(buf, sizeof(buf), "%u", im_opt->nPixH);
    gtk_entry_set_text(GTK_ENTRY(count_y_entry), buf);
    if (globals.new_opt.opt_alg == PIX_SIZE) {
      gtk_editable_set_editable(GTK_EDITABLE(size_x_entry), TRUE);
      gtk_editable_set_editable(GTK_EDITABLE(size_y_entry), TRUE);
      gtk_editable_set_editable(GTK_EDITABLE(count_x_entry), FALSE);
      gtk_editable_set_editable(GTK_EDITABLE(count_y_entry), FALSE);
      gtk_widget_set_sensitive(size_x_entry, TRUE);
      gtk_widget_set_sensitive(size_y_entry, TRUE);
      gtk_widget_set_sensitive(count_x_entry, FALSE);
      gtk_widget_set_sensitive(count_y_entry, FALSE);
    } else {
      gtk_editable_set_editable(GTK_EDITABLE(size_x_entry), FALSE);
      gtk_editable_set_editable(GTK_EDITABLE(size_y_entry), FALSE);
      gtk_editable_set_editable(GTK_EDITABLE(count_x_entry), TRUE);
      gtk_editable_set_editable(GTK_EDITABLE(count_y_entry), TRUE);
      gtk_widget_set_sensitive(size_x_entry, FALSE);
      gtk_widget_set_sensitive(size_y_entry, FALSE);
      gtk_widget_set_sensitive(count_x_entry, TRUE);
      gtk_widget_set_sensitive(count_y_entry, TRUE);
    }
  }
}

static void pix_opt_alg_CB(GtkWidget *widget, gpointer data) {
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
    globals.new_opt.opt_alg = (unsigned long long)data;
  }
  refresh_options_win(&(globals.new_opt));
}

static gboolean pix_size_x_CB(GtkWidget *widget, GdkEvent *event,
                              gpointer user_data) {
  const gchar *txt = gtk_entry_get_text(GTK_ENTRY(widget));
  (void)user_data;
  (void)event;

  if (sscanf(txt, "%u", &globals.new_opt.pixW) == 1) {
    if (0 == calc_dimensions(&(globals.new_opt))) {
      fprintf(stderr, "error: %s can't compute dimension\n", __func__);
      return FALSE;
    }
  }
  refresh_options_win(&(globals.new_opt));
  return FALSE;
}

static gboolean pix_size_y_CB(GtkWidget *widget, GdkEvent *event,
                              gpointer user_data) {
  const gchar *txt = gtk_entry_get_text(GTK_ENTRY(widget));
  (void)user_data;
  (void)event;

  if (sscanf(txt, "%u", &globals.new_opt.pixH) == 1) {
    if (0 == calc_dimensions(&(globals.new_opt))) {
      fprintf(stderr, "error: %s can't compute dimension\n", __func__);
      return FALSE;
    }
  }
  refresh_options_win(&(globals.new_opt));
  return FALSE;
}

static gboolean pix_proximity_CB(GtkWidget *widget, GdkEvent *event,
                                 gpointer user_data) {
  const gchar *txt = gtk_entry_get_text(GTK_ENTRY(widget));
  (void)user_data;
  (void)event;

  if (sscanf(txt, "%u", &globals.new_opt.proximity) == 1) {
  }
  refresh_options_win(&(globals.new_opt));
  return FALSE;
}

static gboolean pix_count_x_CB(GtkWidget *widget, GdkEvent *event,
                               gpointer user_data) {
  const gchar *txt = gtk_entry_get_text(GTK_ENTRY(widget));
  (void)user_data;
  (void)event;

  if (sscanf(txt, "%u", &globals.new_opt.nPixW) == 1) {
    if (0 == calc_dimensions(&(globals.new_opt))) {
      fprintf(stderr, "error: %s can't compute dimension\n", __func__);
      return FALSE;
    }
  }
  refresh_options_win(&(globals.new_opt));
  return FALSE;
}

static gboolean pix_count_y_CB(GtkWidget *widget, GdkEvent *event,
                               gpointer user_data) {
  const gchar *txt = gtk_entry_get_text(GTK_ENTRY(widget));
  (void)user_data;
  (void)event;

  if (sscanf(txt, "%u", &globals.new_opt.nPixH) == 1) {
    if (0 == calc_dimensions(&(globals.new_opt))) {
      fprintf(stderr, "error: %s can't compute dimension\n", __func__);
      return FALSE;
    }
  }
  refresh_options_win(&(globals.new_opt));
  return FALSE;
}

/* pops up a new window with all the options in it */
void optionsCB(gpointer data) {
  (void)data;

  if (!optWindow) {
    optWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    if (optWindow) {
      GtkWidget *grid;

      gtk_window_set_title(GTK_WINDOW(optWindow), "Options");

      g_signal_connect(optWindow, "destroy", G_CALLBACK(gtk_widget_destroyed),
                       &optWindow);

      gtk_container_set_border_width(GTK_CONTAINER(optWindow), 4);

      /* all the sections go in the grid */
      grid = gtk_grid_new();

      if (grid) {
        GtkWidget *frame;

        gtk_container_add(GTK_CONTAINER(optWindow), grid);
        gtk_widget_show(grid);

        /***** IMAGES *******************************************************/

        frame = gtk_frame_new("Images");

        if (frame) {
          GtkWidget *grid2;

          gtk_grid_attach(GTK_GRID(grid), frame, 0, 0, 1, 1);
          gtk_container_set_border_width(GTK_CONTAINER(frame), 4);
          gtk_widget_show(frame);

          grid2 = gtk_grid_new();

          if (grid2) {
            char buf[8];
            GSList *group;
            GtkWidget *button;
            GtkWidget *label;
            GtkWidget *entry;

            gtk_container_set_border_width(GTK_CONTAINER(grid2), 4);
            gtk_container_add(GTK_CONTAINER(frame), grid2);
            gtk_widget_show(grid2);

            /***** size *****/
            size_button = button = gtk_radio_button_new_with_label(NULL, "");
            gtk_grid_attach(GTK_GRID(grid2), button, 0, 0, 1, 1);
            g_signal_connect(button, "toggled", G_CALLBACK(pix_opt_alg_CB),
                             (gpointer)PIX_SIZE);
            gtk_widget_show(button);

            label = gtk_label_new("Image Sizes: ");
            gtk_grid_attach(GTK_GRID(grid2), label, 1, 0, 1, 1);
            gtk_widget_show(label);

            size_x_entry = entry = gtk_entry_new();
            gtk_entry_set_max_length(GTK_ENTRY(entry), 5);
            gtk_widget_set_size_request(entry, 50, -1);
            g_signal_connect(entry, "focus-out-event",
                             G_CALLBACK(pix_size_x_CB), NULL);
            gtk_grid_attach(GTK_GRID(grid2), entry, 2, 0, 1, 1);
            gtk_widget_show(entry);

            label = gtk_label_new(" X ");
            gtk_grid_attach(GTK_GRID(grid2), label, 4, 0, 1, 1);
            gtk_widget_show(label);

            size_y_entry = entry = gtk_entry_new();
            gtk_entry_set_max_length(GTK_ENTRY(entry), 5);
            gtk_widget_set_size_request(entry, 50, -1);
            g_signal_connect(entry, "focus-out-event",
                             G_CALLBACK(pix_size_y_CB), NULL);
            gtk_grid_attach(GTK_GRID(grid2), entry, 6, 0, 1, 1);
            gtk_widget_show(entry);

            label = gtk_label_new(" Pixels ");
            gtk_grid_attach(GTK_GRID(grid2), label, 8, 0, 1, 1);
            gtk_widget_show(label);

            /***** number *****/
            group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(button));
            count_button = button = gtk_radio_button_new_with_label(group, "");
            gtk_grid_attach(GTK_GRID(grid2), button, 0, 1, 1, 1);
            g_signal_connect(button, "toggled", G_CALLBACK(pix_opt_alg_CB),
                             (gpointer)PIX_COUNT);
            gtk_widget_show(button);

            label = gtk_label_new("Number of Images: ");
            gtk_grid_attach(GTK_GRID(grid2), label, 1, 1, 1, 1);
            gtk_widget_show(label);

            count_x_entry = entry = gtk_entry_new();
            gtk_entry_set_max_length(GTK_ENTRY(entry), 5);
            gtk_widget_set_size_request(entry, 50, -1);
            g_signal_connect(entry, "focus-out-event",
                             G_CALLBACK(pix_count_x_CB), NULL);
            gtk_grid_attach(GTK_GRID(grid2), entry, 2, 1, 1, 1);
            gtk_widget_show(entry);

            label = gtk_label_new(" X ");
            gtk_grid_attach(GTK_GRID(grid2), label, 4, 1, 1, 1);
            gtk_widget_show(label);

            count_y_entry = entry = gtk_entry_new();
            gtk_entry_set_max_length(GTK_ENTRY(entry), 5);
            gtk_widget_set_size_request(entry, 50, -1);
            g_signal_connect(entry, "focus-out-event",
                             G_CALLBACK(pix_count_y_CB), NULL);
            gtk_grid_attach(GTK_GRID(grid2), entry, 6, 1, 1, 1);
            gtk_widget_show(entry);

            label = gtk_label_new(" Images ");
            gtk_grid_attach(GTK_GRID(grid2), label, 8, 1, 1, 1);
            gtk_widget_show(label);

            /***** proximity *****/

            label = gtk_label_new("Proximity of duplicates: ");
            gtk_grid_attach(GTK_GRID(grid2), label, 1, 2, 1, 1);
            gtk_widget_show(label);

            entry = gtk_entry_new();
            gtk_entry_set_max_length(GTK_ENTRY(entry), 5);
            gtk_widget_set_size_request(entry, 50, -1);
            g_signal_connect(entry, "focus-out-event",
                             G_CALLBACK(pix_proximity_CB), NULL);
            snprintf(buf, sizeof(buf), "%u", globals.new_opt.proximity);
            gtk_entry_set_text(GTK_ENTRY(entry), buf);
            gtk_grid_attach(GTK_GRID(grid2), entry, 2, 2, 1, 1);
            gtk_widget_show(entry);

            /**************** Dismiss **************************/

            button = gtk_button_new_with_label("Apply");
            g_signal_connect_swapped(button, "clicked",
                                     G_CALLBACK(gtk_widget_destroy), optWindow);
            gtk_grid_attach(GTK_GRID(grid), button, 0, 1, 1, 1);
            gtk_widget_set_can_default(button, TRUE);
            gtk_widget_show(button);
          } else {
            fprintf(stderr, "%s: failed to create grid2\n", __func__);
            return;
          }
        } else {
          fprintf(stderr, "%s: failed to create frame\n", __func__);
          return;
        }
      } else {
        fprintf(stderr, "%s: failed to create grid\n", __func__);
        return;
      }
    } else {
      fprintf(stderr, "%s: failed to create option window\n", __func__);
      return;
    }
  }

  /* set the toggle buttons to the right state */
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(size_button),
                               globals.new_opt.opt_alg == PIX_SIZE ? TRUE
                                                                   : FALSE);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(count_button),
                               globals.new_opt.opt_alg == PIX_COUNT ? TRUE
                                                                    : FALSE);

  /* set the test entry boxes to the right state */
  refresh_options_win(&(globals.new_opt));

  gtk_widget_show(optWindow);
}
