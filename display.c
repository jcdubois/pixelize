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

// static GdkRectangle workarea = {0};

/* some callbacks */
static gboolean draw_callback(GtkWidget *widget, cairo_t *cr, gpointer data) {
  (void)data;
  (void)widget;
  (void)cr;
  GdkPixbuf *ptr = NULL;

  fprintf(stderr, "%s: enter\n", __func__);

  if (globals.show_rendered && globals.image) {
    ptr = globals.out_im;
  } else {
    ptr = globals.in_im;
  }

  if (ptr) {
    // define the selected pixbuf as draw_area source
    gdk_cairo_set_source_pixbuf(cr, ptr, 0, 0);
    // adjust the scrollbar as required
    gtk_widget_set_size_request(widget, gdk_pixbuf_get_width(ptr),
                                gdk_pixbuf_get_height(ptr));
  }

  cairo_paint(cr);

  fprintf(stderr, "%s: exit\n", __func__);

  return FALSE;
}

static gint resize_callback(GtkWidget *widget, GdkEventConfigure *event,
                            gpointer func_data) {
  (void)func_data;
  (void)widget;
  (void)event;

  fprintf(stderr, "%s: enter\n", __func__);

  // Limit the minimal window size
  GdkGeometry hints;
  hints.min_width = 300;
  hints.min_height = 300;

  gdk_window_set_geometry_hints(
      gtk_widget_get_window(GTK_WIDGET(globals.topwin)), &hints,
      GDK_HINT_MIN_SIZE);

  fprintf(stderr, "%s: exit\n", __func__);

  return TRUE;
}

static gboolean key_press_callback(GtkWidget *widget, GdkEventKey *event) {
  (void)widget;
  (void)event;

  fprintf(stderr, "%s: enter\n", __func__);

  fprintf(stderr, "%s: exit\n", __func__);

  return TRUE;
}

static gboolean key_release_callback(GtkWidget *widget, GdkEventKey *event) {
  (void)widget;

  fprintf(stderr, "%s: enter\n", __func__);

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

  fprintf(stderr, "%s: exit\n", __func__);

  return TRUE;
}

static gboolean button_press_callback(GtkWidget *widget,
                                      GdkEventButton *event) {
  (void)widget;

  fprintf(stderr, "%s: enter\n", __func__);

  if (event->x < 0 || event->x >= globals.cur_opt.width) {
  } else if (event->y < 0 || event->y >= globals.cur_opt.height) {
  } else if (globals.image != NULL) {
    guint xx, yy;

    xx = event->x / globals.cur_opt.pixW;
    yy = event->y / globals.cur_opt.pixH;

    if (event->button == 2) {
      info_popup(xx, yy);
      info_prev();
    } else if (event->button == 3) {
      info_popup(xx, yy);
      info_next();
    } else if (event->button == 1) {
      info_popup(xx, yy);
    }
  }

  fprintf(stderr, "%s: exit\n", __func__);

  return TRUE;
}

static gboolean button_release_callback(GtkWidget *widget,
                                        GdkEventButton *event) {
  (void)widget;
  (void)event;

  fprintf(stderr, "%s: enter\n", __func__);

  fprintf(stderr, "%s: exit\n", __func__);

  return TRUE;
}

GtkWidget *setup_display(GtkWidget *parent) {

  fprintf(stderr, "%s: enter\n", __func__);

  /***** create an event box *****/
  globals.event_box = gtk_event_box_new();

  if (globals.event_box) {
    gtk_grid_attach(GTK_GRID(parent), globals.event_box, 0, 1, 2, 1);

    gtk_widget_show(globals.event_box);

    /***** create a new scrolled window. *****/
    globals.scroll_area = gtk_scrolled_window_new(NULL, NULL);

    if (globals.scroll_area) {
      gtk_widget_set_size_request(globals.scroll_area, 300, 300);
      gtk_container_set_border_width(GTK_CONTAINER(globals.scroll_area), 0);
      gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(globals.scroll_area),
                                     GTK_POLICY_AUTOMATIC,
                                     GTK_POLICY_AUTOMATIC);
      gtk_widget_set_hexpand(GTK_WIDGET(globals.scroll_area), TRUE);
      gtk_widget_set_vexpand(GTK_WIDGET(globals.scroll_area), TRUE);
      gtk_container_add(GTK_CONTAINER(globals.event_box), globals.scroll_area);

      gtk_widget_show(globals.scroll_area);

      /***** create the drawing area *****/
      globals.draw_area = gtk_drawing_area_new();

      if (globals.draw_area) {
        // gtk_drawing_area_size(GTK_DRAWING_AREA(globals.draw_area), wid,
        // hgt);
        gtk_container_add(GTK_CONTAINER(globals.scroll_area),
                          globals.draw_area);
        gtk_widget_show(globals.draw_area);

        /* Signals used to handle window ops */
        // g_signal_connect(globals.draw_area, "expose_event",
        // G_CALLBACK(ExposeCB), NULL);
        g_signal_connect(globals.draw_area, "draw", G_CALLBACK(draw_callback),
                         NULL);
        g_signal_connect(globals.draw_area, "configure_event",
                         G_CALLBACK(resize_callback), NULL);

        /* Event signals (Input) */
        g_signal_connect(globals.draw_area, "button_press_event",
                         G_CALLBACK(button_press_callback), NULL);
        g_signal_connect(globals.draw_area, "button_release_event",
                         G_CALLBACK(button_release_callback), NULL);
        g_signal_connect_after(globals.draw_area, "key_press_event",
                               G_CALLBACK(key_press_callback), NULL);
        g_signal_connect_after(globals.draw_area, "key_release_event",
                               G_CALLBACK(key_release_callback), NULL);

        gtk_widget_set_events(globals.draw_area,
                              GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK |
                                  GDK_BUTTON_PRESS_MASK |
                                  GDK_BUTTON_RELEASE_MASK | GDK_KEY_PRESS_MASK |
                                  GDK_KEY_RELEASE_MASK);

        gtk_widget_set_can_focus(globals.draw_area, TRUE);
        gtk_widget_grab_focus(globals.draw_area);

        if (globals.in_fname) {
          open_image();
        }
      }
    }
  }

  fprintf(stderr, "%s: exit\n", __func__);

  return globals.scroll_area;
}

void resize_window() {

  fprintf(stderr, "%s: enter\n", __func__);

#if 0
  gint w, h;

  /* compute window's width */
  if (globals.cur_opt.width > (guint)workarea.width) {
    w = workarea.width;
  } else {
    w = globals.cur_opt.width;
  }

  /* compute window's heigth */
  if (globals.cur_opt.height > (guint)workarea.height) {
    h = workarea.height;
  } else {
    h = globals.cur_opt.height;
  }

  // gtk_drawing_area_size(GTK_DRAWING_AREA(globals.draw_area),
  // globals.cur_opt.width, globals.cur_opt.height);

  gdk_window_resize(gtk_widget_get_window(GTK_WIDGET(globals.topwin)), w, h);

  /* set the size of the window, and allow user resizing */
  gtk_widget_set_size_request(GTK_WIDGET(globals.scroll_area), w, h);
  // gtk_window_set_policy(GTK_WINDOW(globals.topwin), TRUE, TRUE, TRUE);
  gtk_window_set_resizable(GTK_WINDOW(globals.topwin), TRUE);
#endif

  fprintf(stderr, "%s: exit\n", __func__);
}
