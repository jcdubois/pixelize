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

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <stdio.h>

#include "display.h"
#include "draw_image.h"
#include "file_dialog.h"
#include "find_match.h"
#include "globals.h"
#include "info_popup.h"
#include "read_db.h"
#include "render_image.h"

/* some callbacks */
static gint ExposeCB(GtkWidget *widget, GdkEventExpose *event,
                     gpointer func_data) {
  static int picInitalized = 0;

  (void)func_data;
  (void)widget;

  if (picInitalized == 0) {

    /* save the size of the display */
    globals.disp_w = gdk_screen_width();
    globals.disp_h = gdk_screen_height();

    picInitalized = 1;

    if (globals.start_fname) {
      open_image(globals.start_fname);
    }
  }

  redraw_screen(event->area.x, event->area.y, event->area.width,
                event->area.height);

  return FALSE;
}

static gint ResizeCB(GtkWidget *widget, GdkEventConfigure *event,
                     gpointer func_data) {
  unsigned int w, h;

  (void)func_data;
  (void)widget;
  (void)event;

  w = gdk_window_get_width(gtk_widget_get_window(GTK_WIDGET(globals.ebox)));
  h = gdk_window_get_height(gtk_widget_get_window(GTK_WIDGET(globals.ebox)));

  /* we need to tell Gtk's brain dead scrollbar when not to appear */
  if (w >= globals.cur_opt.width && h >= globals.cur_opt.height) {
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(globals.picScroll),
                                   GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  } else {
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(globals.picScroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  }

  /* dammit: let the user resize the window!
     I think gtk_widget_set_usize() messes with this. */
  /*
     gdk_window_set_hints(GTK_WIDGET(globals.topwin)->window,
     0, 0, 300, 300, 0, 0, GDK_HINT_MIN_SIZE);
   */

  return TRUE;
}

static gint KeyPressEvnt(GtkWidget *widget, GdkEventKey *event,
                         gpointer func_data) {
  (void)func_data;
  (void)widget;
  (void)event;

  return TRUE;
}

static gint KeyReleaseEvnt(GtkWidget *widget, GdkEventKey *event,
                           gpointer func_data) {
  (void)func_data;
  (void)widget;

  switch (event->keyval) {
  case GDK_space:
  case GDK_KP_Space:
    globals.show_rendered = !globals.show_rendered;
    redraw_screen(0, 0, globals.cur_opt.width, globals.cur_opt.height);
    break;
  default:
    break;
  }
  return TRUE;
}

static gint ButtonPressEvnt(GtkWidget *widget, GdkEventButton *event,
                            gpointer func_data) {
  (void)func_data;
  (void)widget;

  if (event->x < 0 || event->x >= globals.cur_opt.width)
    return TRUE;

  if (event->y < 0 || event->y >= globals.cur_opt.height)
    return TRUE;

  if (globals.image != NULL) {
    unsigned int xx, yy;

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

  return TRUE;
}

static gint ButtonReleaseEvnt(GtkWidget *widget, GdkEventButton *event,
                              gpointer func_data) {
  (void)func_data;
  (void)widget;
  (void)event;

  return TRUE;
}

GtkWidget *setup_display(GtkWidget *parent) {
  int wid, hgt;

  wid = 300;
  hgt = 300;

  /***** create an event box *****/
  globals.ebox = gtk_event_box_new();
  gtk_box_pack_start(GTK_BOX(parent), globals.ebox, TRUE, TRUE, 0);
  gtk_widget_show(globals.ebox);

  /***** create a new scrolled window. *****/
  globals.picScroll = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_set_size_request(globals.picScroll, wid, hgt);
  gtk_container_set_border_width(GTK_CONTAINER(globals.picScroll), 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(globals.picScroll),
                                 GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_container_add(GTK_CONTAINER(globals.ebox), globals.picScroll);

  /* gtk_box_pack_start(GTK_BOX(parent), globals.picScroll, TRUE, TRUE, 0); */
  gtk_widget_show(globals.picScroll);

  /***** create the drawing area *****/
  globals.picDA = gtk_drawing_area_new();
  // gtk_drawing_area_size(GTK_DRAWING_AREA(globals.picDA), wid, hgt);

  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(globals.picScroll),
                                        globals.picDA);
  gtk_widget_show(globals.picDA);

  /* Signals used to handle window ops */
  g_signal_connect(GTK_OBJECT(globals.picDA), "expose_event",
                   G_CALLBACK(ExposeCB), NULL);
  g_signal_connect(GTK_OBJECT(globals.picDA), "configure_event",
                   G_CALLBACK(ResizeCB), NULL);

  /* Event signals (Input) */
  g_signal_connect(GTK_OBJECT(globals.picDA), "button_press_event",
                   G_CALLBACK(ButtonPressEvnt), NULL);
  g_signal_connect(GTK_OBJECT(globals.picDA), "button_release_event",
                   G_CALLBACK(ButtonReleaseEvnt), NULL);
  g_signal_connect_after(GTK_OBJECT(globals.picDA), "key_press_event",
                         G_CALLBACK(KeyPressEvnt), NULL);
  g_signal_connect_after(GTK_OBJECT(globals.picDA), "key_release_event",
                         G_CALLBACK(KeyReleaseEvnt), NULL);

  gtk_widget_set_events(globals.picDA,
                        GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK |
                            GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK |
                            GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK);

  gtk_widget_set_can_focus(globals.picDA, TRUE);
  gtk_widget_grab_focus(globals.picDA);

  return globals.picScroll;
}

void resize_window() {
  int w, h;

  /* compute window's width */
  if (globals.cur_opt.width > globals.disp_w) {
    w = globals.disp_w;
  } else {
    w = globals.cur_opt.width;
  }

  /* compute window's heigth */
  if (globals.cur_opt.height > globals.disp_h) {
    h = globals.disp_h;
  } else {
    h = globals.cur_opt.height;
  }

  // gtk_drawing_area_size(GTK_DRAWING_AREA(globals.picDA),
  // globals.cur_opt.width, globals.cur_opt.height);

  gdk_window_resize(gtk_widget_get_window(GTK_WIDGET(globals.topwin)), w, h);
  /*
     gtk_window_set_default_size(GTK_WINDOW(globals.mainwin), w, h);
  */

  /* set the size of the window, and allow user resizing */
  gtk_widget_set_size_request(GTK_WIDGET(globals.picScroll), w, h);
  // gtk_window_set_policy(GTK_WINDOW(globals.topwin), TRUE, TRUE, TRUE);
}
