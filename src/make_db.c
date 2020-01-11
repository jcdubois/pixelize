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
/* make_db.c by Paul Wilkins 1/2/2000 */

#include <gtk/gtk.h>

struct PIX {
  int cnt;
  double r;
  double g;
  double b;
};

#define MAX_SIZE 5
#define MAX_PATH_LEN 512

int main(int argc, char **argv) {
  GOptionEntry entries[] = {
      {NULL},
  };

  if (gtk_init_with_args(&argc, &argv, "[FILE…]", entries, NULL, NULL)) {
    guint i;
    guint j;
    guint n;
    guint size;
    GError *gerror;
    GdkPixbuf *pb;
    guint qh;
    guint qw;
    guint width;
    guint height;
    guint ww;
    guint hh;
    guchar *p;
    guchar *pixels;
    guint rowstride;
    guint n_channels;
    struct PIX ***quad;
    char my_cwd[MAX_PATH_LEN + 1];
    FILE *dbfp;

    /* Be nice and tell the user if they don't, to provide a file as an arg */
    if (argc <= 1) {
      g_printerr(
          "make_db is a program that scans pictures and creates a file\n");
      g_printerr(
          "called pic_db.dat. The file pic_db.dat is used by Pixelize\n");
      g_printerr("when \"rendering\" images.\n");
      g_printerr("\n");
      g_printerr("Usage:\n %s image_files\n", argv[0]);
      exit(1);
    }

    if (NULL == (dbfp = fopen("pic_db.dat", "a+"))) {
      g_printerr("Error opening pic_db.dat for write\n");
      exit(1);
    }

    if (NULL == (quad = g_malloc0((MAX_SIZE + 1) * sizeof(struct PIX **)))) {
      perror("malloc");
      exit(1);
    }

    for (i = 1; i <= MAX_SIZE; i++) {
      if (NULL ==
          (quad[i] = g_malloc0((MAX_SIZE + 1) * sizeof(struct PIX *)))) {
        perror("malloc");
        exit(1);
      }
      for (j = 0; j <= MAX_SIZE; j++) {
        if (NULL ==
            (quad[i][j] = g_malloc0((MAX_SIZE + 1) * sizeof(struct PIX)))) {
          perror("malloc");
          exit(1);
        }
      }
    }

    fprintf(dbfp, "%u\n", MAX_SIZE);

    /* get the cwd.  we may use it later */
    if (NULL == (getcwd(my_cwd, MAX_PATH_LEN))) {
      g_printerr("Warning: Can't determine Current working directory.\n");
      g_printerr("         This may generate an invalid pic_db.dat file.\n");
      my_cwd[0] = '\0';
    }

    /* for each file */
    for (n = 1; n < (guint)argc; n++) {

      /* Load the image specified as the first argument */
      gerror = NULL;

      pb = gdk_pixbuf_new_from_file(argv[n], &gerror);

      if (pb == NULL) {
        g_printerr("Error: Can't load image %s: %s\n", argv[n],
                   gerror->message);
        continue;
      }

      /* Suck the image's original width and height out of the Image structure
       */
      width = gdk_pixbuf_get_width(pb);
      height = gdk_pixbuf_get_height(pb);
      pixels = gdk_pixbuf_get_pixels(pb);
      rowstride = gdk_pixbuf_get_rowstride(pb);
      n_channels = gdk_pixbuf_get_n_channels(pb);

      g_assert(gdk_pixbuf_get_colorspace(pb) == GDK_COLORSPACE_RGB);
      g_assert(gdk_pixbuf_get_bits_per_sample(pb) == 8);

      printf("file %u: %s width: %u height: %u\n", n, argv[n], width, height);

      if (argv[n][0] != '/') {
        fprintf(dbfp, "%s/%s\n", my_cwd, argv[n]);
      } else {
        fprintf(dbfp, "%s\n", argv[n]);
      }

      for (size = 1; size <= MAX_SIZE; size++) {
        for (hh = 0; hh < size; hh++) {
          for (ww = 0; ww < size; ww++) {
            quad[size][ww][hh].cnt = 0;
            quad[size][ww][hh].r = 0.0;
            quad[size][ww][hh].g = 0.0;
            quad[size][ww][hh].b = 0.0;
          }
        }
      }

      /* generate the data for each size */
      for (size = 3; size <= MAX_SIZE; size++) {

        for (hh = 0; hh < height; hh++) {

          qh = (guint)((double)hh / (double)height * (double)size);
          if (qh >= size) {
            qh = size - 1;
          }

          for (ww = 0; ww < width; ww++) {

            qw = (int)((double)ww / (double)width * (double)size);
            if (qw >= size) {
              qw = size - 1;
            }

            p = pixels + hh * rowstride + ww * n_channels;

            quad[size][qw][qh].cnt++;
            quad[size][qw][qh].r += (double)(p[0]);
            quad[size][qw][qh].g += (double)(p[1]);
            quad[size][qw][qh].b += (double)(p[2]);
          }
        }
      }

      /* special case for size = 2 */
      for (hh = 0; hh < 2; hh++) {
        for (ww = 0; ww < 2; ww++) {
          for (i = 0; i < 2; i++) {
            for (j = 0; j < 2; j++) {
              quad[2][ww][hh].cnt += quad[4][ww * 2 + i][hh * 2 + j].cnt;
              quad[2][ww][hh].r += quad[4][ww * 2 + i][hh * 2 + j].r;
              quad[2][ww][hh].g += quad[4][ww * 2 + i][hh * 2 + j].g;
              quad[2][ww][hh].b += quad[4][ww * 2 + i][hh * 2 + j].b;
            }
          }
        }
      }

      /* special case for size = 1 */
      for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
          quad[1][0][0].cnt += quad[2][i][j].cnt;
          quad[1][0][0].r += quad[2][i][j].r;
          quad[1][0][0].g += quad[2][i][j].g;
          quad[1][0][0].b += quad[2][i][j].b;
        }
      }

      for (size = 1; size <= MAX_SIZE; size++) {
        for (hh = 0; hh < size; hh++) {
          for (ww = 0; ww < size; ww++) {
            fprintf(dbfp, "%03u %03u %03u\n",
                    (guint)(quad[size][ww][hh].r / quad[size][ww][hh].cnt),
                    (guint)(quad[size][ww][hh].g / quad[size][ww][hh].cnt),
                    (guint)(quad[size][ww][hh].b / quad[size][ww][hh].cnt));
          }
        }
      }

      g_object_unref(pb);
    }

    fclose(dbfp);

    return 0;
  } else {
    g_printerr("%s: Error: in parameters\n", argv[0]);
    exit(1);
  }
}
