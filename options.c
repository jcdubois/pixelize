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

#include <ctype.h>
#include <gtk/gtk.h>
#include <stdio.h>
/* #include <gdk_imlib.h> */

#include "draw_image.h"
#include "globals.h"
#include "options.h"

static GtkWidget *optWindow = NULL;
static GtkWidget *size_x_entry = NULL;
static GtkWidget *size_y_entry = NULL;
static GtkWidget *count_x_entry = NULL;
static GtkWidget *count_y_entry = NULL;
static GtkWidget *size_button = NULL;
static GtkWidget *count_button = NULL;

unsigned int is_valid_int(const char *str) {
  const char *p1 = str;
  unsigned int val;

  while (*p1 != 0) {
    if (!isdigit(*p1))
      return -1;
    p1++;
  }

  if (1 != sscanf(str, "%u", &val))
    return -1;

  return val;
}

void refresh_options_win(struct IMAGE_OPTIONS *im_opt) {
  char buf[8];

  if (optWindow) {

    sprintf(buf, "%u", im_opt->pixW);
    gtk_entry_set_text(GTK_ENTRY(size_x_entry), buf);
    sprintf(buf, "%u", im_opt->pixH);
    gtk_entry_set_text(GTK_ENTRY(size_y_entry), buf);
    sprintf(buf, "%u", im_opt->nPixW);
    gtk_entry_set_text(GTK_ENTRY(count_x_entry), buf);
    sprintf(buf, "%u", im_opt->nPixH);
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

void apply_CB(GtkWidget *widget, GtkWidget *dialog) {
  (void)widget;

  if (0 == calc_dimensions(&(globals.new_opt))) {
    fprintf(stderr, "error: %s can't compute dimension\n", __func__);
    return;
  }
  refresh_options_win(&(globals.new_opt));
  gtk_widget_destroy(dialog);
}

void pix_opt_alg_CB(GtkWidget *widget, gpointer data) {
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
    globals.new_opt.opt_alg = (unsigned long long)data;
  }
  refresh_options_win(&(globals.new_opt));
}

void pix_size_x_CB(GtkWidget *widget, GtkWidget *entry) {
  const gchar *txt = gtk_entry_get_text(GTK_ENTRY(entry));
  (void)widget;

  globals.new_opt.pixW = is_valid_int(txt);
  if (0 == calc_dimensions(&(globals.new_opt))) {
    fprintf(stderr, "error: %s can't compute dimension\n", __func__);
    return;
  }
  refresh_options_win(&(globals.new_opt));
}

void pix_size_y_CB(GtkWidget *widget, GtkWidget *entry) {
  const gchar *txt = gtk_entry_get_text(GTK_ENTRY(entry));
  (void)widget;

  globals.new_opt.pixH = is_valid_int(txt);
  if (0 == calc_dimensions(&(globals.new_opt))) {
    fprintf(stderr, "error: %s can't compute dimension\n", __func__);
    return;
  }
  refresh_options_win(&(globals.new_opt));
}

static void pix_proximity_CB(GtkWidget *widget, GtkWidget *entry) {
  const gchar *txt = gtk_entry_get_text(GTK_ENTRY(entry));
  (void)widget;

  globals.new_opt.proximity = is_valid_int(txt);
}

void pix_count_x_CB(GtkWidget *widget, GtkWidget *entry) {
  const gchar *txt = gtk_entry_get_text(GTK_ENTRY(entry));
  (void)widget;

  globals.new_opt.nPixW = is_valid_int(txt);
  if (0 == calc_dimensions(&(globals.new_opt))) {
    fprintf(stderr, "error: %s can't compute dimension\n", __func__);
    return;
  }
  refresh_options_win(&(globals.new_opt));
}

void pix_count_y_CB(GtkWidget *widget, GtkWidget *entry) {
  const gchar *txt = gtk_entry_get_text(GTK_ENTRY(entry));
  (void)widget;

  globals.new_opt.nPixH = is_valid_int(txt);
  if (0 == calc_dimensions(&(globals.new_opt))) {
    fprintf(stderr, "error: %s can't compute dimension\n", __func__);
    return;
  }
  refresh_options_win(&(globals.new_opt));
}

/* pops up a new window with all the options in it */
void optionsCB(gpointer data) {
  (void)data;

  fprintf(stderr, "%s: enterring\n", __func__);

  if (!optWindow) {
    optWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    if (optWindow) {
      GtkWidget *vbox;

      gtk_window_set_title(GTK_WINDOW(optWindow), "Options");

      g_signal_connect(optWindow, "destroy", G_CALLBACK(gtk_widget_destroyed),
                       &optWindow);

      gtk_container_set_border_width(GTK_CONTAINER(optWindow), 4);

      /* all the sections go in the vbox */
      vbox = gtk_vbox_new(FALSE, 0);

      if (vbox) {
        GtkWidget *frame;

        gtk_container_add(GTK_CONTAINER(optWindow), vbox);
        gtk_widget_show(vbox);

        /***** IMAGES *******************************************************/

        frame = gtk_frame_new("Images");

        if (frame) {
          GtkWidget *vbox2;

          gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 0);
          gtk_container_set_border_width(GTK_CONTAINER(frame), 4);
          gtk_widget_show(frame);

          vbox2 = gtk_vbox_new(FALSE, 4);

          if (vbox2) {
            char buf[8];
            GSList *group;
            GtkWidget *button;
            GtkWidget *hbox;
            GtkWidget *label;
            GtkWidget *entry;

            fprintf(stderr, "%s: creating option window\n", __func__);

            gtk_container_set_border_width(GTK_CONTAINER(vbox2), 4);
            gtk_container_add(GTK_CONTAINER(frame), vbox2);
            gtk_widget_show(vbox2);

            /***** size *****/
            hbox = gtk_hbox_new(FALSE, 2);
            gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 0);
            gtk_widget_show(hbox);

            size_button = button = gtk_radio_button_new_with_label(NULL, "");
            gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
            g_signal_connect(button, "toggled", G_CALLBACK(pix_opt_alg_CB),
                             (gpointer)PIX_SIZE);
            gtk_widget_show(button);

            label = gtk_label_new("Image Sizes: ");
            gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
            gtk_widget_show(label);

            size_x_entry = entry = gtk_entry_new();
            gtk_entry_set_max_length(GTK_ENTRY(entry), 5);
            gtk_widget_set_size_request(entry, 50, -1);
            g_signal_connect(entry, "activate", G_CALLBACK(pix_size_x_CB),
                             entry);
            gtk_box_pack_start(GTK_BOX(hbox), entry, FALSE, FALSE, 0);
            gtk_widget_show(entry);

            label = gtk_label_new("X");
            gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
            gtk_widget_show(label);

            size_y_entry = entry = gtk_entry_new();
            gtk_entry_set_max_length(GTK_ENTRY(entry), 5);
            gtk_widget_set_size_request(entry, 50, -1);
            g_signal_connect(entry, "activate", G_CALLBACK(pix_size_y_CB),
                             entry);
            gtk_box_pack_start(GTK_BOX(hbox), entry, FALSE, FALSE, 0);
            gtk_widget_show(entry);

            label = gtk_label_new(" Pixels ");
            gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
            gtk_widget_show(label);

            /***** number *****/
            hbox = gtk_hbox_new(FALSE, 2);
            gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 0);
            gtk_widget_show(hbox);

            group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(button));
            count_button = button = gtk_radio_button_new_with_label(group, "");
            gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
            g_signal_connect(button, "toggled", G_CALLBACK(pix_opt_alg_CB),
                             (gpointer)PIX_COUNT);
            gtk_widget_show(button);

            label = gtk_label_new("Number of Images: ");
            gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
            gtk_widget_show(label);

            count_x_entry = entry = gtk_entry_new();
            gtk_entry_set_max_length(GTK_ENTRY(entry), 5);
            gtk_widget_set_size_request(entry, 50, -1);
            g_signal_connect(entry, "activate", G_CALLBACK(pix_count_x_CB),
                             entry);
            gtk_box_pack_start(GTK_BOX(hbox), entry, FALSE, FALSE, 0);
            gtk_widget_show(entry);

            label = gtk_label_new("X");
            gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
            gtk_widget_show(label);

            count_y_entry = entry = gtk_entry_new();
            gtk_entry_set_max_length(GTK_ENTRY(entry), 5);
            gtk_widget_set_size_request(entry, 50, -1);
            g_signal_connect(entry, "activate", G_CALLBACK(pix_count_y_CB),
                             entry);
            gtk_box_pack_start(GTK_BOX(hbox), entry, FALSE, FALSE, 0);
            gtk_widget_show(entry);

            /***** proximity *****/

            hbox = gtk_hbox_new(FALSE, 2);
            gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 0);
            gtk_widget_show(hbox);

            label = gtk_label_new("Proximity of duplicates: ");
            gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
            gtk_widget_show(label);

            entry = gtk_entry_new();
            gtk_entry_set_max_length(GTK_ENTRY(entry), 5);
            gtk_widget_set_size_request(entry, 50, -1);
            g_signal_connect(entry, "activate", G_CALLBACK(pix_proximity_CB),
                             entry);
            sprintf(buf, "%u", globals.new_opt.proximity);
            gtk_entry_set_text(GTK_ENTRY(entry), buf);
            gtk_box_pack_start(GTK_BOX(hbox), entry, FALSE, FALSE, 0);
            gtk_widget_show(entry);

            /**************** Dismiss **************************/

            // hbox = gtk_hbox_new(FALSE, 2);
            // gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);
            // gtk_widget_show(hbox);

            // button = gtk_button_new_with_label("Apply");
            // gtk_signal_connect_object(GTK_OBJECT(button), "clicked",
            //                            GTK_SIGNAL_FUNC(apply_CB),
            //                            GTK_OBJECT(optWindow));
            // gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
            // GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);
            // gtk_widget_show(button);

            button = gtk_button_new_with_label("Apply");
            g_signal_connect_swapped(button, "clicked",
                                     G_CALLBACK(gtk_widget_destroy),
                                     GTK_OBJECT(optWindow));
            // gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
            gtk_box_pack_start(GTK_BOX(vbox), button, TRUE, TRUE, 0);
            gtk_widget_set_can_default(button, TRUE);
            gtk_widget_show(button);

            fprintf(stderr, "%s: option window created\n", __func__);
          } else {
            fprintf(stderr, "%s: failed to create vbox2\n", __func__);
            return;
          }
        } else {
          fprintf(stderr, "%s: failed to create frame\n", __func__);
          return;
        }
      } else {
        fprintf(stderr, "%s: failed to create vbox\n", __func__);
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

#if 0
  if (GTK_IS_INVISIBLE(optWindow)) {
    gtk_widget_show(optWindow);
  } else {
    gtk_widget_destroy(optWindow);
  }
#else
  gtk_widget_show(optWindow);
#endif
}
