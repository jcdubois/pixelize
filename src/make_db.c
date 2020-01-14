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
#include <read_db.h>

struct PIX {
  int cnt;
  double r;
  double g;
  double b;
};

#define MAX_SIZE 5
#define MAX_PATH_LEN 512

static struct PIC_DB *find_db(struct PIC_DB *head, char *file_name) {
  while (head) {
    if (strncmp(head->fname, file_name, strlen(head->fname)) == 0) {
      return head;
    }
    head = head->next;
  }
  return NULL;
}

int main(int argc, char **argv) {
  gboolean reinitialize = FALSE;
  GOptionEntry entries[] = {
      {"reset", 'r', 0, G_OPTION_ARG_NONE, &reinitialize,
       "Reinitialize database", NULL},
      {NULL},
  };

  if (gtk_init_with_args(&argc, &argv, "[FILE...]", entries, NULL, NULL)) {
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
    char *real_path_name;
    FILE *dbfp;
    struct PIC_DB *head = NULL;
    guint max_order = MAX_SIZE;

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

    if (access("pic_db.dat", F_OK) == -1) {
      /* DB file does not exist yet */
      reinitialize = TRUE;
    } else if (access("pic_db.dat", W_OK | R_OK) == -1) {
      /* DB file cannot be read/written */
      g_printerr("%s: Can't read/write pic_db.dat file\n", argv[0]);
      exit(1);
    } else if (!reinitialize) {
      /* we read the existing DB */
      head = read_database(&max_order);

      if (head == NULL) {
        g_printerr("%s: failed to read/process existing pic_db.dat file\n",
                   argv[0]);
        exit(1);
      }

      if (max_order < 3) {
        g_printerr("%s: maw_oder(%u) < 3 in  existing pic_db.dat file\n",
                   argv[0], max_order);
        exit(1);
      }
    }

    quad = g_malloc0((max_order + 1) * sizeof(struct PIX **));
    if (quad == NULL) {
      perror("malloc");
      exit(1);
    }

    for (i = 1; i <= max_order; i++) {
      quad[i] = g_malloc0((max_order + 1) * sizeof(struct PIX *));
      if (quad[i] == NULL) {
        perror("malloc");
        exit(1);
      }
      for (j = 0; j <= max_order; j++) {
        quad[i][j] = g_malloc0((max_order + 1) * sizeof(struct PIX));
        if (quad[i][j] == NULL) {
          perror("malloc");
          exit(1);
        }
      }
    }

    /* for each file on the command line */
    for (n = 1; n < (guint)argc; n++) {
      struct PIC_DB *db = NULL;

      /* get the absolute path name for the file */
      real_path_name = realpath(argv[n], NULL);

      if (real_path_name == NULL) {
        g_printerr("Error: can't get absolute file name for %s\n", argv[n]);
        continue;
      }

      /* look in the database if this file is already used */
      if (find_db(head, real_path_name)) {
        g_printerr("%s: file %s is alreadty in the DB\n", argv[0], argv[n]);
        g_free(real_path_name);
        real_path_name = NULL;
        continue;
      }

      /* Load the image */
      gerror = NULL;

      pb = gdk_pixbuf_new_from_file(real_path_name, &gerror);

      if (pb == NULL) {
        g_printerr("Error: Can't load image %s: %s\n", real_path_name,
                   gerror->message);
        g_free(real_path_name);
        real_path_name = NULL;
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

      g_print("file %u: %s width: %u height: %u\n", n, real_path_name, width,
              height);

      db = malloc_db(strlen(real_path_name), max_order);

      if (db == NULL) {
        g_printerr("Error: Can't alloc db for %s\n", real_path_name);
        g_free(real_path_name);
        real_path_name = NULL;
        exit(1);
      }

      strcpy(db->fname, real_path_name);

      /* we can free up the file name now (allocated in realpath()) */
      g_free(real_path_name);
      real_path_name = NULL;

      for (size = 1; size <= max_order; size++) {
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
      for (size = 3; size <= max_order; size++) {

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

      for (size = 1; size <= max_order; size++) {
        guint *p1 = db->data[size - 1];
        for (hh = 0; hh < size; hh++) {
          for (ww = 0; ww < size; ww++) {
            p1[0] = (guint)(quad[size][ww][hh].r / quad[size][ww][hh].cnt);
            p1[1] = (guint)(quad[size][ww][hh].g / quad[size][ww][hh].cnt);
            p1[2] = (guint)(quad[size][ww][hh].b / quad[size][ww][hh].cnt);
            p1 += 3;
          }
        }
      }

      g_object_unref(pb);

      db->next = head;

      head = db;
    }

    /* free up the quad array */
    for (i = 1; i <= max_order; i++) {
      for (j = 0; j <= max_order; j++) {
        g_free(quad[i][j]);
      }
      g_free(quad[i]);
    }
    g_free(quad);

    /* open the DB file */
    if (NULL == (dbfp = fopen("pic_db.dat", "w+"))) {
      g_printerr("Error opening pic_db.dat for write\n");
      exit(1);
    }

    /* write the max order */
    fprintf(dbfp, "%u\n", max_order);

    /* write all files and related data */
    while (head) {
      struct PIC_DB *db = head;
      fprintf(dbfp, "%s\n", head->fname);

      for (size = 1; size <= max_order; size++) {
        guint *p1 = db->data[size - 1];
        for (hh = 0; hh < size; hh++) {
          for (ww = 0; ww < size; ww++) {
            fprintf(dbfp, "%03u %03u %03u\n", p1[0], p1[1], p1[2]);
            p1 += 3;
          }
        }
      }
      head = head->next;

      free_db(db, max_order);
    }

    /* close file */
    fclose(dbfp);

    return 0;
  } else {
    g_printerr("%s: Error: in parameters\n", argv[0]);
    exit(1);
  }
}
