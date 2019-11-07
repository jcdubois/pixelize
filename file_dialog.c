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
/* file_dialog.c by Paul Wilkins 1/2/2000 */

#include "file_dialog.h"
#include "cursor.h"
#include "draw_image.h"
#include "globals.h"
#include "options.h"
#include <sys/stat.h>

/*   1 == regular
 *   0 == not regular
 *  -1 == does not exist
 */
static int file_type(const char *file_name) {
  if (file_name) {
    struct stat statb;

    /* get the file's size */
    if (stat(file_name, &statb) == -1) {
      return -1;
    }

    if (S_ISREG(statb.st_mode)) {
      return 1;
    }

    return 0;
  } else {
    return -1;
  }
}

static gboolean check_valid_types(const char *type) {
  GSList *format_list, *iterator;

  format_list = gdk_pixbuf_get_formats();

  if (format_list) {
    for (iterator = format_list; iterator; iterator = g_slist_next(iterator)) {
      GdkPixbufFormat *pixbuf_format = (GdkPixbufFormat *)iterator->data;

      if (pixbuf_format) {
        gchar *format_name = gdk_pixbuf_format_get_name(pixbuf_format);

        if (format_name) {
          if (type) {
            if (strcmp(type, format_name) == 0) {
              return TRUE;
            }
          } else {
            fprintf(stderr, "%s\n", format_name);
          }
        }
      }
    }

    g_slist_free(format_list);
  }
  return FALSE;
}

static char *get_extension(const char *file_name) {
  if (file_name) {
    char *p1 = strrchr(file_name, '.');
    if (p1 == NULL) {
      fprintf(stderr, "Error: %s: Can not find file extension\n", __func__);
      return NULL;
    }
    return p1 + 1;
  } else {
    fprintf(stderr, "Error: %s: no name provided\n", __func__);
    return NULL;
  }
}

static gboolean save_image(void) {
  gboolean ret = FALSE;

  fprintf(stderr, "%s: enter\n", __func__);

  if (globals.out_fname && strlen(globals.out_fname) &&
      file_type(globals.out_fname) != 0) {

    if (globals.out_im) {
      char *extension = get_extension(globals.out_fname);

      if (extension) {

        if (check_valid_types(extension)) {
          GError *gerror = NULL;

          if (1 == gdk_pixbuf_save(globals.out_im, globals.out_fname, extension,
                                   &gerror, NULL)) {
            ret = TRUE;
          } else {
            fprintf(stderr, "Error: %s: Can't write %s: %s\n", __func__,
                    globals.out_fname, gerror->message);
          }
        } else {
          fprintf(stderr,
                  "Error: %s: Can not determine file type from extension\n",
                  __func__);
          fprintf(stderr, "Valid extension types are:\n");
          check_valid_types(NULL);
        }
      } else {
        fprintf(stderr, "Error: %s: unknown extension\n", __func__);
      }
    } else {
      fprintf(stderr, "Error: %s: render an image first.\n", __func__);
    }
  } else {
    fprintf(stderr, "Error: %s: Invalid file name.\n", __func__);
  }

  return ret;
}

gboolean open_image(void) {
  gboolean ret = FALSE;

  fprintf(stderr, "%s: enter\n", __func__);

  if (globals.in_fname && strlen(globals.in_fname) != 0 &&
      file_type(globals.in_fname) == 1) {
    GdkPixbuf *im;
    GError *gerror;

    /* free old data */
    if (globals.in_im != NULL) {
      g_object_unref(globals.in_im);
    }
    globals.in_im = NULL;

    if (globals.in_im_scaled != NULL) {
      g_object_unref(globals.in_im_scaled);
    }
    globals.in_im_scaled = NULL;

    free_image_data();

    /* Load the image specified as the first argument */
    gerror = NULL;
    im = gdk_pixbuf_new_from_file(globals.in_fname, &gerror);
    if (im) {

      /* add an alpha channel */
      globals.in_im = gdk_pixbuf_add_alpha(im, 0, 0, 0, 0);
      g_object_unref(im);

      if (globals.in_im) {

        if (calc_dimensions(&(globals.new_opt))) {
          copy_opt_data();
          refresh_options_win(&(globals.new_opt));

          /* scale the image */
          globals.in_im_scaled = gdk_pixbuf_scale_simple(
              globals.in_im, globals.cur_opt.pixW * globals.cur_opt.nPixW,
              globals.cur_opt.pixH * globals.cur_opt.nPixH,
              GDK_INTERP_BILINEAR);

          if (globals.in_im_scaled) {

            if (globals.draw_area) {
              gtk_widget_queue_draw(globals.draw_area);
            }

            ret = TRUE;
          } else {
            fprintf(stderr, "Error: %s: Unable to scale image: %s\n", __func__,
                    globals.in_fname);
          }
        } else {
          fprintf(stderr, "Error: %s: Invalid option value.\n", __func__);
        }
      } else {
        fprintf(stderr, "Error: %s: while adding alpha channel\n", __func__);
      }
    } else {
      fprintf(stderr, "Error: %s: Can't load image %s: %s\n", __func__,
              globals.in_fname, gerror->message);
    }
  } else {
    fprintf(stderr, "Error: %s: Invalid file name.\n", __func__);
  }

  fprintf(stderr, "%s: exit\n", __func__);
  return ret;
}

void file_open_dialog(void) {

  fprintf(stderr, "%s: enter\n", __func__);

  GtkWidget *dialog = gtk_file_chooser_dialog_new(
      "Open File", NULL, GTK_FILE_CHOOSER_ACTION_SAVE, "Cancel",
      GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL);

  if (dialog) {
    gint res = gtk_dialog_run(GTK_DIALOG(dialog));

    if (res == GTK_RESPONSE_ACCEPT) {
      GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);

      if (chooser) {
        char *filename = gtk_file_chooser_get_filename(chooser);

        if (filename) {
          cursor_busy();
          if (globals.in_fname) {
            g_free(globals.in_fname);
          }
          globals.in_fname = filename;
          open_image();
          cursor_normal();
        }
      }
    }

    gtk_widget_destroy(dialog);
  }

  fprintf(stderr, "%s: exit\n", __func__);
}

void file_save_dialog(void) {

  fprintf(stderr, "%s: enter\n", __func__);

  GtkWidget *dialog = gtk_file_chooser_dialog_new(
      "Save File", NULL, GTK_FILE_CHOOSER_ACTION_SAVE, "Cancel",
      GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL);

  if (dialog) {
    GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);

    if (chooser) {
      gint res;

      gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);

      gtk_file_chooser_set_current_name(
          chooser, globals.out_fname ? globals.out_fname : "Untitled document");

      res = gtk_dialog_run(GTK_DIALOG(dialog));

      if (res == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(chooser);

        if (filename) {
          cursor_busy();
          if (globals.out_fname) {
            g_free(globals.out_fname);
          }
          globals.out_fname = filename;
          save_image();
          cursor_normal();
        }
      }
    }

    gtk_widget_destroy(dialog);
  }

  fprintf(stderr, "%s: exit\n", __func__);
}
