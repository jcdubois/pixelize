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
/* draw_image.c by Paul Wilkins 1/2/2000 */

#include "draw_image.h"
#include "globals.h"
#include "read_db.h"
#include "render_image.h"

#include <math.h>

static gboolean check_options(struct _ImageOptions *im_opt) {
  gboolean ret = TRUE;

  if (im_opt->opt_alg == PIX_SIZE) {
    if (im_opt->pixW < 1) {
      fprintf(stderr, "Image size X: invalid number\n");
      ret = FALSE;
    }
    if (im_opt->pixH < 1) {
      fprintf(stderr, "Image size Y: invalid number\n");
      ret = FALSE;
    }
  } else {
    if (im_opt->nPixW < 1) {
      fprintf(stderr, "Image count X: invalid number\n");
      ret = FALSE;
    }
    if (im_opt->nPixH < 1) {
      fprintf(stderr, "Image count Y: invalid number\n");
      ret = FALSE;
    }
  }
  if (im_opt->proximity < 1) {
    fprintf(stderr, "Image proximity: invalid number\n");
    ret = FALSE;
  }

  return ret;
}

gboolean calc_dimensions(struct _ImageOptions *im_opt) {
  gboolean ret = FALSE;

  if (check_options(im_opt)) {
    if (im_opt->opt_alg == PIX_SIZE) {
      if (globals.in_im == NULL) {
        im_opt->nPixW = 0;
        im_opt->nPixH = 0;
      } else {
        im_opt->nPixW = ceil((double)gdk_pixbuf_get_width(globals.in_im) /
                             (double)im_opt->pixW);
        im_opt->nPixH = ceil((double)gdk_pixbuf_get_height(globals.in_im) /
                             (double)im_opt->pixH);
      }

      im_opt->width = im_opt->nPixW * im_opt->pixW;
      im_opt->height = im_opt->nPixH * im_opt->pixH;
      ret = TRUE;
    } else if (im_opt->opt_alg == PIX_COUNT) {
      if (globals.in_im == NULL) {
        im_opt->pixW = 0;
        im_opt->pixH = 0;
      } else {
        im_opt->pixW = ceil((double)gdk_pixbuf_get_width(globals.in_im) /
                            (double)im_opt->nPixW);
        im_opt->pixH = ceil((double)gdk_pixbuf_get_height(globals.in_im) /
                            (double)im_opt->nPixH);
      }

      im_opt->width = im_opt->nPixW * im_opt->pixW;
      im_opt->height = im_opt->nPixH * im_opt->pixH;
      ret = TRUE;
    } else {
      fprintf(stderr, "calc_dimensions: I shouldn't get here.\n");
    }
  } else {
    fprintf(stderr, "error with options\n");
  }

  return ret;
}

gboolean copy_opt_data() {
  gboolean ret = FALSE;

  if ((globals.cur_opt.height != globals.new_opt.height) ||
      (globals.cur_opt.width != globals.new_opt.width)) {
    ret = TRUE;
  }

  globals.cur_opt = globals.new_opt;

  return ret;
}

gboolean change_small_image(int xx, int yy) {
  gboolean ret = FALSE;

  if (globals.image != NULL) {
    struct IMAGE_INFO *inf = &(globals.image[yy][xx]);

    if (inf) {
      struct PIC_DB *db = inf->matches[inf->match_no];

      if (db) {
        /* load the image */
        GError *gerror = NULL;
        GdkPixbuf *im = gdk_pixbuf_new_from_file(db->fname, &gerror);

        if (im) {
          /* scale the image */
          GdkPixbuf *scale_im = gdk_pixbuf_scale_simple(
              im, globals.cur_opt.pixW, globals.cur_opt.pixH,
              GDK_INTERP_BILINEAR);
          if (scale_im) {
            /* add an alpha channel */
            GdkPixbuf *scale_im_alpha =
                gdk_pixbuf_add_alpha(scale_im, 0, 0, 0, 0);

            if (scale_im_alpha) {
              copy_image_RGB(scale_im_alpha, globals.out_im, xx, yy);

              g_object_unref(scale_im_alpha);

              ret = TRUE;
            } else {
              fprintf(stderr, "Error: Unable to add alpha to  %s\n", db->fname);
            }
            g_object_unref(scale_im);
          } else {
            fprintf(stderr, "Error: Unable to scale image %s\n", db->fname);
          }
          g_object_unref(im);
        } else {
          fprintf(stderr, "Error: Can't load image %s: ", db->fname);
          if (gerror) {
            fprintf(stderr, "%s", gerror->message);
          }
          fprintf(stderr, "\n");
        }
      }
    }
  }

  return ret;
}
