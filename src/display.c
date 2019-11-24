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
/* display.c by Paul Wilkins 1/2/2000 */

#include "display.h"
#include "draw_image.h"
#include "file_dialog.h"
#include "globals.h"
#include "info_popup.h"

gboolean draw_area_draw_cb(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
  (void)user_data;

  GdkPixbuf *ptr = NULL;

  if (globals.show_rendered && globals.image) {
    ptr = globals.out_im;
  } else {
    ptr = globals.in_im_scaled;
  }

  if (ptr) {
    GdkPixbuf *ptr2;
    GtkAllocation alloc;
    gint pix_width = gdk_pixbuf_get_width(ptr);
    gint pix_height = gdk_pixbuf_get_height(ptr);
    gint area_height, area_width;

    gtk_widget_get_allocation(gtk_widget_get_parent(widget), &alloc);

    if (pix_height > pix_width) {
      area_height = (pix_height > alloc.height) ? alloc.height : pix_height;
      area_width = area_height * pix_width / pix_height;
      if (area_width > alloc.width) {
        area_width = alloc.width;
        area_height = area_width * pix_height / pix_width;
      }
    } else {
      area_width = (pix_width > alloc.width) ? alloc.width : pix_width;
      area_height = area_width * pix_height / pix_width;
      if (area_height > alloc.height) {
        area_height = alloc.height;
        area_width = area_height * pix_width / pix_height;
      }
    }

    ptr2 = gdk_pixbuf_scale_simple(ptr, area_width, area_height,
                                   GDK_INTERP_BILINEAR);

    if (ptr2) {
      // define the selected pixbuf as draw_area source
      gdk_cairo_set_source_pixbuf(cr, ptr2, 0, 0);
      // adjust the scrollbar as required
      gtk_widget_set_size_request(widget, area_width, area_height);
      g_object_unref(ptr2);
    }
  }

  cairo_paint(cr);

  return FALSE;
}

gboolean draw_area_configure_event_cb(GtkWidget *widget,
                                      GdkEventConfigure *event,
                                      gpointer user_data) {
  (void)user_data;
  (void)widget;
  (void)event;

  g_printerr("%s: Enter\n", __func__);

  // Limit the minimal window size
  GdkGeometry hints;
  hints.min_width = 300;
  hints.min_height = 300;

  gdk_window_set_geometry_hints(
      gtk_widget_get_window(GTK_WIDGET(globals.topwin)), &hints,
      GDK_HINT_MIN_SIZE);

  gtk_widget_queue_draw(GTK_WIDGET(widget));

  g_printerr("%s: Exit\n", __func__);

  return TRUE;
}

gboolean draw_area_key_release_event_cb(GtkWidget *widget, GdkEventKey *event,
                                        gpointer user_data) {
  (void)widget;
  (void)user_data;

  g_printerr("%s: Enter\n", __func__);

  switch (event->keyval) {
  case GDK_KEY_space:
  case GDK_KEY_KP_Space:
    // toggle the image to display
    globals.show_rendered = !globals.show_rendered;
    // redraw the draw area
    gtk_widget_queue_draw(widget);
    break;
  default:
    break;
  }

  g_printerr("%s: Exit\n", __func__);

  return TRUE;
}

gboolean draw_area_button_press_event_cb(GtkWidget *widget,
                                         GdkEventButton *event,
                                         gpointer user_data) {
  (void)widget;

  GtkWidget *dialog = GTK_WIDGET(user_data);

  g_printerr("%s: Enter\n", __func__);

  if (dialog && (event->x > 0) && (event->y > 0) &&
      (event->x < gtk_widget_get_allocated_width(widget)) &&
      (event->y < gtk_widget_get_allocated_height(widget)) &&
      (globals.image != NULL)) {
    guint xx, yy;

    xx = (event->x * globals.cur_opt.width) /
         (gtk_widget_get_allocated_width(widget) * globals.cur_opt.pixW);
    yy = (event->y * globals.cur_opt.height) /
         (gtk_widget_get_allocated_height(widget) * globals.cur_opt.pixH);

    info_popup(dialog, xx, yy);

#if 0
    switch (event->button) {
    case (2): /* middle mouse button */
      info_prev();
      break;
    case (3): /* right mouse button */
      info_next();
      break;
    case (1): /* left mouse button */
    default:
      break;
    }
#endif
  }

  g_printerr("%s: Exit\n", __func__);

  return TRUE;
}

gboolean update_gui_callback(gpointer data) {

  gtk_widget_queue_draw(GTK_WIDGET(data));

  return FALSE;
}
