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
/* info_popup.c by Paul Wilkins 1/2/2000 */

#include "info_popup.h"
#include "draw_image.h"
#include "find_match.h"
#include "globals.h"
#include "highlight.h"
#include "render_image.h"

#define LIST_WIDTH 300

static GtkWidget *draw_area = NULL;
static GtkWidget *info_list = NULL;
static guint info_x = 0; /* the current cell */
static guint info_y = 0;
static gboolean ignore_selection = FALSE;

gboolean info_draw_area_draw_cb(GtkWidget *widget, cairo_t *cr, gpointer data) {
  (void)data;

  struct IMAGE_INFO *inf = &(globals.image[info_y][info_x]);

  if (inf) {
    struct PIC_DB *db = inf->matches[inf->match_no];

    if (db) {
      GError *gerror = NULL;
      GdkPixbuf *im = gdk_pixbuf_new_from_file(db->fname, &gerror);

      if (im) {
        GdkPixbuf *im2 = gdk_pixbuf_add_alpha(im, 0, 0, 0, 0);
        g_object_unref(im);

        if (im2) {
          GdkPixbuf *im3;
          gint width = gdk_pixbuf_get_width(im2);
          gint height = gdk_pixbuf_get_height(im2);

          if (width > height) {
            height = height * LIST_WIDTH / width;
            width = LIST_WIDTH;
          } else {
            width = width * LIST_WIDTH / height;
            height = LIST_WIDTH;
          }

          im3 =
              gdk_pixbuf_scale_simple(im2, width, height, GDK_INTERP_BILINEAR);
          g_object_unref(im2);

          if (im3) {
            gdk_cairo_set_source_pixbuf(cr, im3, 0, 0);
            gtk_widget_set_size_request(widget, width, height);
            g_object_unref(im3);
          } else {
            g_printerr("%s: failed to scale the pixbuf\n", __func__);
          }
        } else {
          g_printerr("%s: failed to to add alpha\n", __func__);
        }
      } else {
        g_printerr("%s: failed to make pixbuf from file\n", __func__);
      }
    } else {
      g_printerr("%s: no DB defined for match %u\n", __func__, inf->match_no);
    }
  } else {
    g_printerr("%s: no image info defined for cell(%u, %u)\n", __func__, info_y,
               info_x);
  }

  cairo_paint(cr);

  return FALSE;
}

static void set_highlight() {
  struct IMAGE_INFO *inf = &(globals.image[info_y][info_x]);

  if (inf) {
    if (globals.do_highlight & DO_HIGHLIGHT) {
      inf->do_highlight = TRUE;
    }
  } else {
    g_printerr("%s: no image info defined for cell(%u, %u)\n", __func__, info_y,
               info_x);
  }
}

static void set_highlight_dups() {
  /* find duplicates and mark them */
  struct IMAGE_INFO *inf = &(globals.image[info_y][info_x]);

  if (inf) {
    struct PIC_DB *db = inf->matches[inf->match_no];
    guint x, y;

    for (y = 0; y < globals.cur_opt.nPixH; y++) {
      for (x = 0; x < globals.cur_opt.nPixW; x++) {
        struct IMAGE_INFO *inf2;

        if (x == info_x && y == info_y) {
          continue;
        }

        inf2 = &(globals.image[y][x]);
        if (inf2) {
          struct PIC_DB *db2 = inf2->matches[inf2->match_no];
          if (db == db2) {
            inf2->do_highlight = TRUE;
          }
        }
      }
    }
  } else {
    g_printerr("%s: no image info defined for cell(%u, %u)\n", __func__, info_y,
               info_x);
  }
}

static void clear_all_highlights(gboolean clear_current) {
  guint x, y;

  /* clear everyone */
  for (y = 0; y < globals.cur_opt.nPixH; y++) {
    for (x = 0; x < globals.cur_opt.nPixW; x++) {
      struct IMAGE_INFO *inf;

      if (x == info_x && y == info_y && clear_current == FALSE) {
        continue;
      }

      inf = &(globals.image[y][x]);

      if (inf) {
        inf->do_highlight = FALSE;
      }
    }
  }
}

static void info_fill_list(void) {
  if (info_list) {
    struct IMAGE_INFO *inf = &(globals.image[info_y][info_x]);

    if (inf) {
      /* create a new store */
      GtkListStore *store = gtk_list_store_new(1, G_TYPE_STRING);

      if (store) {
        int i;
        GtkTreeIter iter;
        GtkTreeSelection *select;

        ignore_selection = TRUE;

        /* fill it in */
        for (i = 0; i < MAX_MATCHES; i++) {
          struct PIC_DB *db = inf->matches[i];

          if (db) {
            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter, 0, db->fname, -1);
          }
        }

        /* set the store as a model to the tree view */
        gtk_tree_view_set_model(GTK_TREE_VIEW(info_list),
                                GTK_TREE_MODEL(store));

        /*
         * The tree view has acquired its own reference to the store, so we can
         * drop ours. That way the store will be freed automaitcally when the
         * tree view is destroyed
         */
        g_object_unref(store);

        /* retrieve the tree view selection object */
        select = gtk_tree_view_get_selection(GTK_TREE_VIEW(info_list));

        if (select) {
          /* select a default row */
          GtkTreePath *path = gtk_tree_path_new_from_indices(inf->match_no, -1);
          gtk_tree_selection_select_path(select, path);
          gtk_tree_path_free(path);
        }

        ignore_selection = FALSE;
      }
    } else {
      g_printerr("%s: no image info defined for cell(%u, %u)\n", __func__,
                 info_y, info_x);
    }
  } else {
    g_printerr("%s: info_list is NULL\n", __func__);
  }
}

void info_prev_button_clicked_cb(GtkWidget *widget, gpointer data) {
  (void)widget;
  struct IMAGE_INFO *inf = &(globals.image[info_y][info_x]);

  if (inf) {
    GtkTreeSelection *select = gtk_tree_view_get_selection(GTK_TREE_VIEW(data));

    if (select) {
      GtkTreePath *path = gtk_tree_path_new_from_indices(inf->match_no, -1);
      gtk_tree_selection_unselect_path(select, path);
      gtk_tree_path_free(path);

      /* the previous item in the list */
      if (inf->match_no == 0) {
        inf->match_no = MAX_MATCHES - 1;
      } else {
        inf->match_no--;
      }

      ignore_selection = TRUE;

      path = gtk_tree_path_new_from_indices(inf->match_no, -1);

      if (path) {
        gtk_tree_selection_select_path(select, path);
        gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(data), path, NULL, FALSE, 0,
                                     0);
        gtk_tree_path_free(path);
      }

      ignore_selection = FALSE;

      gtk_widget_queue_draw(draw_area);
    } else {
      g_printerr("%s: failed to retreive a selection\n", __func__);
    }

    /* finally, draw the new image */
    change_small_image(info_x, info_y);

    /* clear and optionalls reset the highlites */
    clear_all_highlights(FALSE);
    if (globals.do_highlight & DO_HIGHLIGHT) {
      set_highlight();
    }
    if (globals.do_highlight & DO_HIGHLIGHT_DUPS) {
      set_highlight_dups();
    }

    highlight_changed();

  } else {
    g_printerr("%s: no image info defined for cell(%u, %u)\n", __func__, info_y,
               info_x);
  }
}

void info_next_button_clicked_cb(GtkWidget *widget, gpointer data) {
  (void)widget;
  struct IMAGE_INFO *inf = &(globals.image[info_y][info_x]);

  if (inf) {
    GtkTreeSelection *select = gtk_tree_view_get_selection(GTK_TREE_VIEW(data));

    if (select) {
      GtkTreePath *path = gtk_tree_path_new_from_indices(inf->match_no, -1);
      gtk_tree_selection_unselect_path(select, path);
      gtk_tree_path_free(path);

      /* the previous item in the list */
      if (inf->match_no == MAX_MATCHES - 1) {
        inf->match_no = 0;
      } else {
        inf->match_no++;
      }

      ignore_selection = TRUE;

      path = gtk_tree_path_new_from_indices(inf->match_no, -1);

      if (path) {
        gtk_tree_selection_select_path(select, path);
        gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(data), path, NULL, FALSE, 0,
                                     0);
        gtk_tree_path_free(path);
      }

      ignore_selection = FALSE;

      if (draw_area) {
        gtk_widget_queue_draw(draw_area);
      }
    } else {
      g_printerr("%s: failed to retreive a selection\n", __func__);
    }

    /* finally, draw the new image */
    change_small_image(info_x, info_y);

    /* clear and optionalls reset the highlites */
    clear_all_highlights(FALSE);
    if (globals.do_highlight & DO_HIGHLIGHT) {
      set_highlight();
    }
    if (globals.do_highlight & DO_HIGHLIGHT_DUPS) {
      set_highlight_dups();
    }

    highlight_changed();

  } else {
    g_printerr("%s: no image info defined for cell(%u, %u)\n", __func__, info_y,
               info_x);
  }
}

void info_tree_selection_changed_cb(GtkTreeSelection *selection,
                                    gpointer data) {
  (void)data;

  if (!ignore_selection) {
    struct IMAGE_INFO *inf = &(globals.image[info_y][info_x]);

    if (inf) {
      GtkTreeIter iter;
      GtkTreeModel *model;

      if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        if (model) {
          GtkTreePath *path = gtk_tree_model_get_path(model, &iter);

          if (path) {
            gint *row = gtk_tree_path_get_indices(path);

            if (row && (int)inf->match_no != row[0]) {
              /* store the new row */
              inf->match_no = (guint)row[0];

              /* finally, draw the new image */
              change_small_image(info_x, info_y);

              /* clear and optionalls reset the highlites */
              clear_all_highlights(FALSE);

              if (globals.do_highlight & DO_HIGHLIGHT) {
                set_highlight();
              }
              if (globals.do_highlight & DO_HIGHLIGHT_DUPS) {
                set_highlight_dups();
              }

              highlight_changed();

              if (draw_area) {
                gtk_widget_queue_draw(draw_area);
              }
            }
          } else {
            g_printerr("%s: no path\n", __func__);
          }
        } else {
          g_printerr("%s: no model\n", __func__);
        }
      }
    } else {
      g_printerr("%s: no image info defined for cell(%u, %u)\n", __func__,
                 info_y, info_x);
    }
  }
}

void info_highlight_toggled_cb(GtkWidget *widget, gpointer data) {
  struct IMAGE_INFO *inf;

  (void)data;

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
    globals.do_highlight |= DO_HIGHLIGHT;
  } else {
    globals.do_highlight &= ~DO_HIGHLIGHT;
  }

  inf = &(globals.image[info_y][info_x]);

  if (inf) {
    if (globals.do_highlight & DO_HIGHLIGHT) {
      inf->do_highlight = TRUE;
    } else {
      inf->do_highlight = FALSE;
    }
  } else {
    g_printerr("%s: no image info defined for cell(%u, %u)\n", __func__, info_y,
               info_x);
  }

  highlight_changed();

  if (globals.do_highlight) {
    start_highlight_timer();
  } else {
    stop_highlight_timer();
  }
}

void info_highlight_dup_toggled_cb(GtkWidget *widget, gpointer data) {
  (void)data;

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
    globals.do_highlight |= DO_HIGHLIGHT_DUPS;
  } else {
    globals.do_highlight &= ~DO_HIGHLIGHT_DUPS;
  }

  if (globals.do_highlight & DO_HIGHLIGHT_DUPS) {
    set_highlight_dups();
  } else {
    clear_all_highlights(FALSE);
  }

  highlight_changed();

  if (globals.do_highlight) {
    start_highlight_timer();
  } else {
    stop_highlight_timer();
  }
}

/* pops up a new window with all the options in it */
void info_popup(GtkWidget *dialog, guint x, guint y) {
  char buf[64];

  if (dialog) {

    /* save the x and y coords */
    info_x = x;
    info_y = y;

    /* set the coord label */
    snprintf(buf, sizeof(buf), "Image coordinates: %ux%u", info_x, info_y);
    gtk_label_set_text(GTK_LABEL(find_child(dialog, "info_label")), buf);

    /* clear and optionalls reset the highlites */
    clear_all_highlights(TRUE);

    if (globals.do_highlight & DO_HIGHLIGHT) {
      set_highlight();
    }

    if (globals.do_highlight & DO_HIGHLIGHT_DUPS) {
      set_highlight_dups();
    }

    highlight_changed();

    if (info_list == NULL) {

      info_list = find_child(dialog, "info_tree_view");

      if (info_list) {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();

        if (renderer) {
          gtk_tree_view_insert_column_with_attributes(
              GTK_TREE_VIEW(info_list), -1, "File", renderer, "text", 0, NULL);
        }
      }
    }

    info_fill_list();

    if (draw_area == NULL) {
      draw_area = find_child(dialog, "info_draw_area");
    }

    if (draw_area) {
      gtk_widget_queue_draw(draw_area);
    }

    gtk_widget_show(dialog);

    gtk_window_set_keep_above(GTK_WINDOW(dialog), TRUE);
  }
}
