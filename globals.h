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

#include <stdbool.h>
#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#define PIX_SIZE 0x1
#define PIX_COUNT 0x2

struct IMAGE_OPTIONS {
  unsigned int width, height; /* width and height of final image */
  unsigned int pixW, pixH;    /* size to scale small images to */
  unsigned int nPixW, nPixH;
  unsigned int proximity;
  unsigned long long opt_alg;
};

/* globals we use alot */
struct GLOBALS {

  GtkWidget *topwin;         /* the main window */
  GtkWidget *ebox;           /* an event box around the scrolled window */

  GtkWidget *picScroll;      /* drawing area scrollbar widget */
  GtkWidget *picDA;          /* drawing area widget */
  //GdkPixmap *pixmap;         /* pixmap of the image */

  struct PIC_DB *head;       /* the image database */
  struct IMAGE_INFO **image; /* the constructed image */

  unsigned int max_order;    /* maximum order when fitting images */

  struct IMAGE_OPTIONS cur_opt;
  struct IMAGE_OPTIONS new_opt;

  int timer;
  unsigned int do_highlight;

  char *start_fname;

  bool show_rendered;

  unsigned int disp_w, disp_h; /* size of display */

  char *in_fname; /* input file */
  GdkPixbuf *in_im_scaled;
  GdkPixbuf *in_im;

  char *out_fname; /* output file */
  GdkPixbuf *out_im;
};

extern struct GLOBALS globals;

int init_globals();

#endif
