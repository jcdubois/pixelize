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
/* render.c by Paul Wilkins 1/2/2000 */

#include "render.h"
#include "cursor.h"
#include "display.h"
#include "draw_image.h"
#include "find_match.h"
#include "globals.h"
#include "render_image.h"
#include "stats.h"
#include "status.h"

#include <math.h>

#define STATS_SIZE 6

struct PIX {
  int cnt;
  double r;
  double g;
  double b;
};

void average_image_area(struct PIX *avg, GdkPixbuf *im, guint x, guint y,
                        guint w, guint h) {
  guint i, j;
  guint r, g, b;
  guint xx, yy;
  guchar *pixels, *p;
  guint rowstride, n_channels;

  n_channels = gdk_pixbuf_get_n_channels(im);
  rowstride = gdk_pixbuf_get_rowstride(im);
  pixels = gdk_pixbuf_get_pixels(im);

  avg->r = 0.0;
  avg->g = 0.0;
  avg->b = 0.0;
  for (j = 0; j < h; j++) {
    yy = j + y;
    for (i = 0; i < w; i++) {
      xx = i + x;

      p = pixels + yy * rowstride + xx * n_channels;

      r = p[0];
      g = p[1];
      b = p[2];

      avg->r += r;
      avg->g += g;
      avg->b += b;
    }
  }
  avg->cnt = w * h;
  avg->r /= (double)(w * h);
  avg->g /= (double)(w * h);
  avg->b /= (double)(w * h);
}

static double calc_stddev(GdkPixbuf *im, guint x, guint y, guint nPixW,
                          guint nPixH) {
  double meanR, stdR;
  double meanG, stdG;
  double meanB, stdB;
  double dataR[STATS_SIZE * STATS_SIZE];
  double dataG[STATS_SIZE * STATS_SIZE];
  double dataB[STATS_SIZE * STATS_SIZE];

  double xoff = (double)x / (double)nPixW * (double)gdk_pixbuf_get_width(im);
  double yoff = (double)y / (double)nPixH * (double)gdk_pixbuf_get_height(im);
  guint n = 0;
  guint pyy = (int)yoff;
  guint h;

  for (h = 0; h < STATS_SIZE; h++) {
    guint yy =
        (int)(yoff + (double)(h + 1) * (double)gdk_pixbuf_get_height(im) /
                         (double)nPixH / (double)STATS_SIZE);
    guint hh = yy - pyy;
    guint pxx = (int)xoff;
    guint w;

    for (w = 0; w < STATS_SIZE; w++) {
      guint xx =
          (int)(xoff + (double)(w + 1) * (double)gdk_pixbuf_get_width(im) /
                           (double)nPixW / (double)STATS_SIZE);
      guint ww = xx - pxx;
      struct PIX avg;

      average_image_area(&avg, im, pxx, pyy, ww, hh);
      dataR[n] = avg.r;
      dataG[n] = avg.g;
      dataB[n] = avg.b;
      n++;

      pxx = xx;
    }
    pyy = yy;
  }

  calc_mead_std(dataR, n, &meanR, &stdR);
  calc_mead_std(dataG, n, &meanG, &stdG);
  calc_mead_std(dataB, n, &meanB, &stdB);

  return (stdR + stdG + stdB) / 3.0;
}

static void init_db() {
  static gboolean init = FALSE;

  if (init == FALSE) {
    /* read in the picture database */
    globals.head = read_database(&(globals.max_order));
    if (globals.head == NULL) {
      fprintf(stderr, "Error reading database.\n");
      exit(1);
    }
    init = TRUE;
  }
}

static guint *gen_master_data(GdkPixbuf *im, guint x, guint y, guint nPixW,
                              guint nPixH, guint order) {
  /* this must be free'd outside this function */
  guint *data = g_malloc0(3 * order * order * sizeof(guint));

  if (data) {
    double xoff = (double)x / (double)nPixW * (double)gdk_pixbuf_get_width(im);
    double yoff = (double)y / (double)nPixH * (double)gdk_pixbuf_get_height(im);
    guint pyy = (int)yoff;
    guint *p1 = data;
    guint h;

    for (h = 0; h < order; h++) {
      guint yy =
          (int)(yoff + (double)(h + 1) * (double)gdk_pixbuf_get_height(im) /
                           (double)nPixH / (double)order);
      guint hh = yy - pyy;
      guint pxx = (int)xoff;
      guint w;

      for (w = 0; w < order; w++) {

        guint xx =
            (int)(xoff + (double)(w + 1) * (double)gdk_pixbuf_get_width(im) /
                             (double)nPixW / (double)order);
        guint ww = xx - pxx;
        struct PIX avg;

        average_image_area(&avg, im, pxx, pyy, ww, hh);
        *p1++ = (int)avg.r;
        *p1++ = (int)avg.g;
        *p1++ = (int)avg.b;

        pxx = xx;
      }
      pyy = yy;
    }
  } else {
    perror("malloc");
    exit(1);
  }

  return data;
}

void free_image_data() {
  if (globals.image) {
    guint hh;
    /* free old image data */
    for (hh = 0; hh < globals.cur_opt.nPixH; hh++) {
      if (globals.image[hh]) {
        guint ww;
        for (ww = 0; ww < globals.cur_opt.nPixW; ww++) {
          if (globals.image[hh][ww].matches) {
            g_free(globals.image[hh][ww].matches);
          }
        }
        g_free(globals.image[hh]);
      }
    }
    g_free(globals.image);

    globals.image = NULL;
  }
}

static gboolean image_borders_n(struct IMAGE_INFO **image, struct PIC_DB *match,
                                guint n, guint cx, guint cy, guint maxx,
                                guint maxy) {
  int x, y;
  for (y = cy - n; y <= (int)(cy + n); y++) {
    if (y < 0 || y >= (int)maxy)
      continue;
    for (x = (int)cx - n; x <= (int)(cx + n); x++) {
      if (x < 0 || x >= (int)maxx)
        continue;
      if (x == (int)cx && y == (int)cy)
        continue;
      if (image[y][x].db == match)
        return TRUE;
    }
  }
  return FALSE;
}

static guint guess_order(double val, guint pixW, guint pixH, guint max_order) {
  guint order = (guint)(sqrt((double)(pixW * pixH)) / 9.0 + (val / 6.0));
  if (order > max_order) {
    order = max_order;
  }
  return order;
}

int render() {
  guint i, j;
  guint pixW, pixH;
  guint nPixW, nPixH;
  guint proximity;
  GdkPixbuf *im;
  GdkPixbuf *out_im;
  guint ww, hh;
  struct PIC_DB **matches;

  if (globals.in_fname == NULL || globals.in_im_scaled == NULL) {
    fprintf(stderr, "Error: Open an Image first!\n");
    return 0;
  }

  /* free the old data, if any */
  free_image_data();

  /* calculate the new images dimensions */
  if (0 == calc_dimensions(&(globals.new_opt))) {
    fprintf(stderr, "Invalid option.  Bye.\n");
    return 0;
  }

  /* set the cursor */
  cursor_busy();

  /* copy the image rendering data from new_opt to cur_opt */
  if (copy_opt_data()) {
    resize_window();

    if (globals.in_im_scaled != NULL) {
      g_object_unref(globals.in_im_scaled);
      globals.in_im_scaled = NULL;
    }

    /* scale the image */
    if ((globals.in_im_scaled = gdk_pixbuf_scale_simple(
             globals.in_im, globals.cur_opt.pixW * globals.cur_opt.nPixW,
             globals.cur_opt.pixH * globals.cur_opt.nPixH,
             GDK_INTERP_BILINEAR)) == NULL) {
      fprintf(stderr, "Error: Unable to scale image: %s\n", globals.in_fname);
      return 0;
    }
  }

  /* update the mode display info */
  refresh_mode_display();

  /* set our local copies of some of the globals */
  im = globals.in_im_scaled;
  pixW = globals.cur_opt.pixW;
  pixH = globals.cur_opt.pixH;
  nPixW = globals.cur_opt.nPixW;
  nPixH = globals.cur_opt.nPixH;

  /* malloc the array to store the image info */
  globals.image = g_malloc0(nPixH * sizeof(struct IMAGE_INFO *));
  if (globals.image) {
    for (hh = 0; hh < nPixH; hh++) {
      globals.image[hh] = g_malloc0(nPixW * sizeof(struct IMAGE_INFO));
      if (!globals.image[hh]) {
        perror("Malloc");
        free_image_data();
        return 0;
      }
    }
  } else {
    perror("Malloc");
    return 0;
  }

  /* initialize the database if not already done */
  init_db();

  /* clear data stored in the database from the last render */
  reset_db_data(globals.head);

  /* init the progress bar to 0 */
  set_progress_indicator(0.0);

  /* from now on, we display the rendered picture */
  globals.show_rendered = 1;

  fprintf(stderr, "Start image match\n");

  /* go pix by pix through the image calculating images that match well */
  for (hh = 0; hh < nPixH; hh++) {
    for (ww = 0; ww < nPixW; ww++) {
      guint *match_data;

      /* calc_stats */
      double stddev = calc_stddev(im, ww, hh, nPixW, nPixH);

      /* depending on stddev, match to different orders */
      guint order = guess_order(stddev, pixW, pixH, globals.max_order);

      /* generate the data to match to from the master image */
      match_data = gen_master_data(im, ww, hh, nPixW, nPixH, order);

      if (match_data) {
        /* find the best matching picture for this "pixel" */
        matches = find_match(order, match_data, globals.head);

        /* store the image we picked */
        globals.image[hh][ww].matches = matches;

        /* we are done with it now */
        g_free(match_data);
      } else {
        globals.image[hh][ww].matches = NULL;
        fprintf(stderr, "Error; no matches for x(%d), y(%d)\n", ww, hh);
      }

      /* update the progress bar */
      set_progress_indicator((double)(hh * nPixW + ww + 1) /
                             (double)(nPixH * nPixW));
    }
  }

  fprintf(stderr, "End image match\n");

  fprintf(stderr, "Find match\n");

  /* go pix by pix through the image finding the best match */
  for (j = 0; j < nPixH * nPixW; j++) {

    /* randomly pick an image to match */
    hh = random() % nPixH;
    ww = random() % nPixW;

    /* find the next free image */
    while (globals.image[hh][ww].db != NULL) {
      ww = (ww + 1) % nPixW;
      if (ww == 0) {
        hh = (hh + 1) % nPixH;
      }
    }

    matches = globals.image[hh][ww].matches;

    if (matches) {
      /* pick one of the (hopefully the best) matches to fill this spot */
      proximity = globals.cur_opt.proximity;
      i = MAX_MATCHES;

      while (proximity > 0 && i == MAX_MATCHES) {
        for (i = 0; i < MAX_MATCHES &&
                    image_borders_n(globals.image, matches[i], proximity, ww,
                                    hh, nPixW, nPixH);
             i++) {
          /* do nothing */
        }
        proximity--;
      }

      if (matches[i] == NULL) {
        /* probably only one file */
        i = 0;
      }

      /* store the image we picked */
      globals.image[hh][ww].db = matches[i];
      if (matches[i]) {
        globals.image[hh][ww].db->refcnt++;
        globals.image[hh][ww].db->done = FALSE;
      }
      globals.image[hh][ww].match_no = i;

    } else {
      globals.image[hh][ww].db = NULL;
      globals.image[hh][ww].match_no = 0;
      fprintf(stderr, "Error; no matches for x(%d), y(%d)\n", ww, hh);
    }
  }

  fprintf(stderr, "End Find match\n");

  /* render the image */
  out_im = render_image(globals.image, nPixW, nPixH, pixW, pixH);
  if (out_im == NULL) {
    fprintf(stderr, "Error: Can't render image.\n");
  }

  /* display it */
  gtk_widget_queue_draw(globals.draw_area);

  /* set the cursor */
  cursor_normal();

  return 1;
}
