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

/* globals.h by Paul Wilkins 1/2/2000 */

#ifndef __GLOBALS_H
#define __GLOBALS_H

#include "render_image.h"
#include <gtk/gtk.h>

#define PIX_SIZE 0x1
#define PIX_COUNT 0x2

struct _ImageOptions {
  guint width, height; /* width and height of final image */
  guint pixW, pixH;    /* size to scale small images to */
  guint nPixW, nPixH;
  guint proximity;
  unsigned long long opt_alg;
};

struct _Globals {

  GtkWidget *topwin;    /* The main window. It encompass everything */
  GtkWidget *draw_area; /* The widget where the picture is drawn */

  struct PIC_DB *head;       /* the image database */
  struct IMAGE_INFO **image; /* the constructed image */

  guint max_order; /* maximum order when fitting images */

  struct _ImageOptions cur_opt;
  struct _ImageOptions new_opt;

  guint do_highlight;

  gboolean command_mode;

  gboolean show_rendered;

  char *in_fname;          /* Input file name */
  GdkPixbuf *in_im;        /* The original images */
  GdkPixbuf *in_im_scaled; /* A scaled version ot the original image.
                              It is scalled to fit exactly the col x line
                              defined in the options */

  char *out_fname;   /* Output file name */
  GdkPixbuf *out_im; /* The processed image */
};

extern struct _Globals globals;

void init_globals();

GtkWidget *find_child(GtkWidget *parent, const gchar *name);

#endif //__GLOBALS_H
