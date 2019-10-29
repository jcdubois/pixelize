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

#include <gtk/gtk.h>
#include <stdio.h>
/* #include <gdk_imlib.h> */
/* #include <gdk/gdk.h> */

#include "draw_image.h"
#include "globals.h"
#include "highlight.h"
#include "render_image.h"

static int timeout_exists = 0;

static int highlight_timeout(gpointer data) {
#if 0
  static int state = 0;
#endif
  unsigned int xx, yy;
  (void)data;

  if (globals.do_highlight) {

    /* search for the images to highlight and draw the highlight here */
    for (yy = 0; yy < globals.cur_opt.nPixH; yy++) {
      for (xx = 0; xx < globals.cur_opt.nPixW; xx++) {
        struct IMAGE_INFO *inf = &(globals.image[yy][xx]);

        if (inf->do_highlight == true) {
#if 0
          GdkGC *gc = (state == 1) ? gtk_widget_get_style(GTK_WIDGET(globals.picDA))->white_gc
                                   : gtk_widget_get_style(GTK_WIDGET(globals.picDA))->black_gc;

          gdk_draw_rectangle(
              gtk_widget_get_window(GTK_WIDGET(globals.picDA)), gc, FALSE,
              xx * globals.cur_opt.pixW, yy * globals.cur_opt.pixH,
              globals.cur_opt.pixW - 1, globals.cur_opt.pixH - 1);
#else
          cairo_surface_t *surface = NULL;

          if (surface) {
            cairo_t *cr = gdk_cairo_create(
                gtk_widget_get_window(GTK_WIDGET(globals.picDA)));

            if (cr) {
              cairo_set_source_surface(cr, surface, 0, 0);
              cairo_pattern_set_extend(cairo_get_source(cr),
                                       CAIRO_EXTEND_REPEAT);
              cairo_rectangle(
                  cr, xx * globals.cur_opt.pixW, yy * globals.cur_opt.pixH,
                  globals.cur_opt.pixW - 1, globals.cur_opt.pixH - 1);
              cairo_fill(cr);
              cairo_destroy(cr);
            }
          }
#endif
        }
      }
    }

#if 0
    state = !state;
#endif

    /* make sure our drawing happens now */
    gdk_flush();
  }

  return TRUE;
}

int highlight_changed() {

  /* redraw the screen */
  redraw_screen(0, 0, globals.cur_opt.width, globals.cur_opt.height);

  /* redraw the highlights */
  highlight_timeout(NULL);

  return 1;
}

int start_highlight_timer() {

  /* start a timer if one does not already exist */
  if (!timeout_exists) {
    globals.timer = g_timeout_add(200, highlight_timeout, NULL);
    timeout_exists = 1;
  }

  return 1;
}

int stop_highlight_timer() {

  if (timeout_exists) {
    g_source_remove(globals.timer);
    timeout_exists = 0;
  }

  /* redraw the screen */
  redraw_screen(0, 0, globals.cur_opt.width, globals.cur_opt.height);

  return 1;
}
