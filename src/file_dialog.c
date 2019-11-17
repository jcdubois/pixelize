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
            g_printerr("%s\n", format_name);
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
      g_printerr("Error: %s: Can not find file extension\n", __func__);
      return NULL;
    }

    return p1 + 1;
  } else {
    g_printerr("Error: %s: no name provided\n", __func__);

    return NULL;
  }
}

gboolean save_image(void) {
  gboolean ret = FALSE;

  if (globals.out_fname && strlen(globals.out_fname) &&
      file_type(globals.out_fname) != 0) {

    if (globals.out_im) {
      char *extension = get_extension(globals.out_fname);

      if (extension) {

        if (check_valid_types(extension)) {
          GError *gerror = NULL;

          if (gdk_pixbuf_save(globals.out_im, globals.out_fname, extension,
                              &gerror, NULL)) {
            ret = TRUE;
          } else {
            g_printerr("Error: %s: Can't write %s: %s\n", __func__,
                       globals.out_fname, gerror->message);
          }
        } else {
          g_printerr("Error: %s: Can not determine file type from extension\n",
                     __func__);
          g_printerr("Valid extension types are:\n");
          check_valid_types(NULL);
        }
      } else {
        g_printerr("Error: %s: unknown extension\n", __func__);
      }
    } else {
      g_printerr("Error: %s: render an image first.\n", __func__);
    }
  } else {
    g_printerr("Error: %s: Invalid file name.\n", __func__);
  }

  return ret;
}

gboolean open_image(void) {
  gboolean ret = FALSE;

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
            g_printerr("Error: %s: Unable to scale image: %s\n", __func__,
                       globals.in_fname);
          }
        } else {
          g_printerr("Error: %s: Invalid option value.\n", __func__);
        }
      } else {
        g_printerr("Error: %s: while adding alpha channel\n", __func__);
      }
    } else {
      g_printerr("Error: %s: Can't load image %s: %s\n", __func__,
                 globals.in_fname, gerror->message);
    }
  } else {
    g_printerr("Error: %s: Invalid file name.\n", __func__);
  }

  return ret;
}
