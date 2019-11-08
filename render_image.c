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

#include "render_image.h"
#include "globals.h"
#include "status.h"

static void find_next(struct IMAGE_INFO **image, struct PIC_DB *db, guint *col,
                      guint *row, guint nPixW, guint nPixH) {
  guint r, c, cl;

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

void copy_image_RGB(GdkPixbuf *src, GdkPixbuf *dst, guint col, guint row) {
  gdk_pixbuf_copy_area(src, 0,                          // src_x
                       0,                               // src_y
                       globals.cur_opt.pixW,            // width
                       globals.cur_opt.pixH,            // height
                       dst, col * globals.cur_opt.pixW, // dest_x
                       row * globals.cur_opt.pixH       // dest_y
  );
}

GdkPixbuf *render_image(struct IMAGE_INFO **image, guint nPixW, guint nPixH,
                        guint pixW, guint pixH) {
  guint width = nPixW * pixW;
  guint height = nPixH * pixH;
  GdkPixbuf *dest = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, width, height);

  if (dest) {
    guint ww, hh;

    if (globals.out_im) {
      g_object_unref(globals.out_im);
    }
    globals.out_im = dest;

    gdk_pixbuf_copy_area(globals.in_im_scaled, 0, 0, width, height, dest, 0, 0);

    /* init the progress bar 0 */
    set_progress_indicator(0.0);

    for (hh = 0; hh < nPixH; hh++) {
      for (ww = 0; ww < nPixW; ww++) {
        GdkPixbuf *im;
        GError *gerror;

        if (image[hh][ww].db) {
          /* skip this image if it has already been rendered */
          if (image[hh][ww].db->done == TRUE) {
            continue;
          }

          image[hh][ww].db->done = TRUE;

          /* load the image */
          gerror = NULL;
          im = gdk_pixbuf_new_from_file(image[hh][ww].db->fname, &gerror);
          if (im) {

            /* scale the image */
            GdkPixbuf *scale_im =
                gdk_pixbuf_scale_simple(im, pixW, pixH, GDK_INTERP_BILINEAR);
            if (scale_im) {

              /* add an alpha channel */
              GdkPixbuf *scale_im_alpha =
                  gdk_pixbuf_add_alpha(scale_im, 0, 0, 0, 0);
              if (scale_im_alpha) {
                guint i;
                guint row, col;

                /* copy the image data into the output data */
                for (i = 0; i < image[hh][ww].db->refcnt; i++) {
                  if (i == 0) {
                    row = hh;
                    col = ww;
                  } else {
                    find_next(image, image[hh][ww].db, &col, &row, nPixW,
                              nPixH);
                  }

                  copy_image_RGB(scale_im_alpha, dest, col, row);
                }

                g_object_unref(scale_im_alpha);
              } else {
                fprintf(stderr, "Error: Unable to add alpha to image %s\n",
                        image[hh][ww].db->fname);
              }

              g_object_unref(scale_im);
            } else {
              fprintf(stderr, "Error: Unable to scale image %s\n",
                      image[hh][ww].db->fname);
            }

            g_object_unref(im);
          } else {
            fprintf(stderr, "Error: Unable to open %s: %s\n",
                    image[hh][ww].db->fname, gerror->message);
          }
        } else {
          fprintf(stderr, "Error: No DB entry for x(%d) y(%d)\n", ww, hh);
        }

        /* update the progress bar */
        set_progress_indicator((double)(hh * nPixW + ww + 1) /
                               (double)(nPixH * nPixW));

        gtk_widget_queue_draw(globals.draw_area);

        gdk_display_flush(gdk_display_get_default());
      }
    }
  }

  return dest;
}
