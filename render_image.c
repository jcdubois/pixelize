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
/* render_image.c by Paul Wilkins 1/2/2000 */

#include <gtk/gtk.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
/* #include <gdk_imlib.h> */

#include "draw_image.h"
#include "globals.h"
#include "read_db.h"
#include "render_image.h"
#include "status.h"

static void find_next(struct IMAGE_INFO **image, struct PIC_DB *db,
                      unsigned int *col, unsigned int *row, unsigned int nPixW,
                      unsigned int nPixH) {
  unsigned int r, c, cl;
  cl = (*col) + 1;
  for (r = *row; r < nPixH; r++) {
    for (c = cl; c < nPixW; c++) {
      if (db == image[r][c].db) {
        *row = r;
        *col = c;
        return;
      }
    }
    cl = 0;
  }
  fprintf(stderr, "Error: Can't find next match\n");
  exit(1);
}

void copy_image_RGB(GdkPixbuf *src, GdkPixbuf *dst, unsigned int col,
                    unsigned int row) {
  gdk_pixbuf_copy_area(src, 0,                          // src_x
                       0,                               // src_y
                       globals.cur_opt.pixW,            // width
                       globals.cur_opt.pixH,            // height
                       dst, col * globals.cur_opt.pixW, // dest_x
                       row * globals.cur_opt.pixH       // dest_y
  );
}

/*
void copy_image_RGB(
   GdkPixbuf *src,
   GdkPixbuf *dst,
   int col,
   int row)
{
   int i;
   int src_x, src_y;
   int dst_x, dst_y;
   int src_rowstride, src_n_channels;
   int dst_rowstride, dst_n_channels;
   guchar *src_pixels, *src_p;
   guchar *dst_pixels, *dst_p;

   src_n_channels = gdk_pixbuf_get_n_channels(src);
   dst_n_channels = gdk_pixbuf_get_n_channels(dst);

   src_rowstride = gdk_pixbuf_get_rowstride(src);
   dst_rowstride = gdk_pixbuf_get_rowstride(dst);

   src_pixels = gdk_pixbuf_get_pixels(src);
   dst_pixels = gdk_pixbuf_get_pixels(dst);

   for(src_y=0; src_y<globals.cur_opt.pixH; src_y++){
      for(src_x=0; src_x<globals.cur_opt.pixW; src_x++){

         dst_x = src_x + col * globals.cur_opt.pixW;
         dst_y = src_y + row * globals.cur_opt.pixH;

         src_p = src_pixels + src_y * src_rowstride + src_x * src_n_channels;
         dst_p = dst_pixels + dst_y * dst_rowstride + dst_x * dst_n_channels;

         for(i=0; i<src_n_channels && i<dst_n_channels; i++){
                                dst_p[i] = src_p[i];
         }

      }
   }
}
*/

GdkPixbuf *render_image(struct IMAGE_INFO **image, unsigned int nPixW,
                        unsigned int nPixH, unsigned int pixW,
                        unsigned int pixH) {
  unsigned int i;
  GError *gerror;
  unsigned int row, col;
  unsigned int width, height;
  unsigned int ww, hh;
  GdkPixbuf *im;
  GdkPixbuf *scale_im;
  GdkPixbuf *scale_im_alpha;
  GdkPixbuf *dest;

  width = nPixW * pixW;
  height = nPixH * pixH;

  dest = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, width, height);

  /* update the progress bar */
  set_progress_indicator(0.0);

  for (hh = 0; hh < nPixH; hh++) {
    for (ww = 0; ww < nPixW; ww++) {

      /* skip this image if it has already been rendered; */
      if (image[hh][ww].db->done == true)
        continue;
      image[hh][ww].db->done = true;

      /* load the image */
      gerror = NULL;
      if (NULL ==
          (im = gdk_pixbuf_new_from_file(image[hh][ww].db->fname, &gerror))) {
        fprintf(stderr, "Error: Unable to open %s: %s\n",
                image[hh][ww].db->fname, gerror->message);
        continue;
      }

      /* scale the image */
      if (NULL == (scale_im = gdk_pixbuf_scale_simple(im, pixW, pixH,
                                                      GDK_INTERP_BILINEAR))) {
        fprintf(stderr, "Error: Unable to scale image %s\n",
                image[hh][ww].db->fname);
        continue;
      }

      /* add an alpha channel */
      scale_im_alpha = gdk_pixbuf_add_alpha(scale_im, 0, 0, 0, 0);

      /* copy the image data into the output data */
      for (i = 0; i < image[hh][ww].db->refcnt; i++) {
        if (i == 0) {
          row = hh;
          col = ww;
        } else {
          find_next(image, image[hh][ww].db, &col, &row, nPixW, nPixH);
        }

        draw_small_image(scale_im_alpha, col, row);

        copy_image_RGB(scale_im_alpha, dest, col, row);
      }

      g_object_unref(im);
      g_object_unref(scale_im);
      g_object_unref(scale_im_alpha);

      /* update the progress bar */
      set_progress_indicator((double)(hh * nPixW + ww + 1) /
                             (double)(nPixH * nPixW));
    }
  }

  return dest;
}
