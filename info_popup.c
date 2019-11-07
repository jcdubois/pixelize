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

#include "draw_image.h"
#include "find_match.h"
#include "globals.h"
#include "highlight.h"
#include "info_popup.h"
#include "render_image.h"

#define LIST_WIDTH 400

static GtkWidget *infWindow = NULL;
static GtkWidget *info_list = NULL;
static GtkWidget *info_label = NULL;

static guint info_x = 0; /* the current cell */
static guint info_y = 0;
static gboolean ignore_selection = FALSE;

static void set_highlight() {
  struct IMAGE_INFO *inf = &(globals.image[info_y][info_x]);

  if (inf) {
    if (globals.do_highlight & DO_HIGHLIGHT) {
      inf->do_highlight = TRUE;
    }
  }
}

static void set_highlight_dups() {
  /* find duplicates and mark them */
  struct IMAGE_INFO *inf = &(globals.image[info_y][info_x]);

  if (inf) {
    guint x, y;
    struct PIC_DB *db = inf->matches[inf->match_no];

    for (y = 0; y < globals.cur_opt.nPixH; y++) {
      for (x = 0; x < globals.cur_opt.nPixW; x++) {
        struct IMAGE_INFO *inf2;

        if (x == info_x && y == info_y)
          continue;

        inf2 = &(globals.image[y][x]);
        if (inf2) {
          struct PIC_DB *db2 = inf2->matches[inf2->match_no];
          if (db == db2)
            inf2->do_highlight = TRUE;
        }
      }
    }
  }
}

static void clear_all_highlights(int clear_current) {
  guint x, y;

  /* clear everyone */
  for (y = 0; y < globals.cur_opt.nPixH; y++) {
    for (x = 0; x < globals.cur_opt.nPixW; x++) {
      struct IMAGE_INFO *inf;

      if (x == info_x && y == info_y && clear_current == 0)
        continue;

      inf = &(globals.image[y][x]);

      if (inf) {
        inf->do_highlight = FALSE;
      }
    }
  }
}

static void info_fill_list() {
  struct IMAGE_INFO *inf = &(globals.image[info_y][info_x]);

  fprintf(stderr, "%s: enterring\n", __func__);

  ignore_selection = TRUE;

  if (inf && info_list) {
    /* create a new store */
    GtkListStore *store = gtk_list_store_new(1, G_TYPE_STRING);

    if (store) {
      int i;
      GtkTreeIter iter;
      GtkTreeSelection *select;

      /* fill it in */
      for (i = 0; i < MAX_MATCHES; i++) {
        struct PIC_DB *db = inf->matches[i];

        if (db) {
          gtk_list_store_append(store, &iter);
          gtk_list_store_set(store, &iter, 0, db->fname, -1);
        }
      }

      /* set the store as a model to the tree view */
      gtk_tree_view_set_model(GTK_TREE_VIEW(info_list), GTK_TREE_MODEL(store));

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
    }
  }

  ignore_selection = FALSE;

  fprintf(stderr, "%s: exiting\n", __func__);
}

void info_prev() {
  struct IMAGE_INFO *inf = &(globals.image[info_y][info_x]);

  if (inf) {
    GtkTreeSelection *select =
        gtk_tree_view_get_selection(GTK_TREE_VIEW(info_list));

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
      gtk_tree_selection_select_path(select, path);
      gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(info_list), path, NULL, FALSE,
                                   0, 0);
      gtk_tree_path_free(path);

      ignore_selection = FALSE;
    }

    /* finally, draw the new image */
    change_small_image(info_x, info_y);

    /* clear and optionalls reset the highlites */
    clear_all_highlights(0);
    if (globals.do_highlight & DO_HIGHLIGHT) {
      set_highlight();
    }
    if (globals.do_highlight & DO_HIGHLIGHT_DUPS) {
      set_highlight_dups();
    }
    highlight_changed();
  }
}

void info_next() {
  struct IMAGE_INFO *inf = &(globals.image[info_y][info_x]);

  if (inf) {
    GtkTreeSelection *select =
        gtk_tree_view_get_selection(GTK_TREE_VIEW(info_list));

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
      gtk_tree_selection_select_path(select, path);
      gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(info_list), path, NULL, FALSE,
                                   0, 0);
      gtk_tree_path_free(path);

      ignore_selection = FALSE;
    }

    /* finally, draw the new image */
    change_small_image(info_x, info_y);

    /* clear and optionalls reset the highlites */
    clear_all_highlights(0);
    if (globals.do_highlight & DO_HIGHLIGHT) {
      set_highlight();
    }
    if (globals.do_highlight & DO_HIGHLIGHT_DUPS) {
      set_highlight_dups();
    }
    highlight_changed();
  }
}

static void info_prevCB(GtkWidget *widget, gpointer data) {
  (void)widget;
  (void)data;
  info_prev();
}

static void info_nextCB(GtkWidget *widget, gpointer data) {
  (void)widget;
  (void)data;
  info_next();
}

static void info_selectionCB(GtkTreeSelection *selection, gpointer data) {
  (void)data;

  if (!ignore_selection) {
    struct IMAGE_INFO *inf = &(globals.image[info_y][info_x]);

    if (inf) {
      GtkTreeIter iter;
      GtkTreeModel *model;

      if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
        gint *row = gtk_tree_path_get_indices(path);

        if ((int)inf->match_no != row[0]) {
          /* store the new row */
          inf->match_no = (guint)row[0];

          /* finally, draw the new image */
          change_small_image(info_x, info_y);

          /* clear and optionalls reset the highlites */
          clear_all_highlights(0);

          if (globals.do_highlight & DO_HIGHLIGHT) {
            set_highlight();
          }
          if (globals.do_highlight & DO_HIGHLIGHT_DUPS) {
            set_highlight_dups();
          }

          highlight_changed();
        }
      }
    }
  }
}

static void info_highlightCB(GtkWidget *widget, gpointer data) {
  struct IMAGE_INFO *inf;

  (void)data;

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
    globals.do_highlight |= DO_HIGHLIGHT;
  } else {
    globals.do_highlight &= ~DO_HIGHLIGHT;
  }

  inf = &(globals.image[info_y][info_x]);
  if (globals.do_highlight & DO_HIGHLIGHT) {
    inf->do_highlight = TRUE;
  } else {
    inf->do_highlight = FALSE;
  }

  highlight_changed();
  if (globals.do_highlight)
    start_highlight_timer();
  else
    stop_highlight_timer();
}

static void info_highlight_dupsCB(GtkWidget *widget, gpointer data) {
  (void)data;

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
    globals.do_highlight |= DO_HIGHLIGHT_DUPS;
  } else {
    globals.do_highlight &= ~DO_HIGHLIGHT_DUPS;
  }

  if (globals.do_highlight & DO_HIGHLIGHT_DUPS) {
    set_highlight_dups();
  } else {
    clear_all_highlights(0);
  }

  highlight_changed();
  if (globals.do_highlight)
    start_highlight_timer();
  else
    stop_highlight_timer();
}

/* pops up a new window with all the options in it */
void info_popup(guint x, guint y) {
  char buf[64];
  GtkWidget *vbox;
  GtkWidget *table;
  GtkWidget *scrolled_window;
  GtkWidget *button;
  GtkWidget *radio;

  fprintf(stderr, "%s: enterring\n", __func__);

  if (!infWindow) {

    infWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(infWindow), "Image Information");

    g_signal_connect(infWindow, "destroy", G_CALLBACK(gtk_widget_destroyed),
                     &infWindow);
    gtk_container_set_border_width(GTK_CONTAINER(infWindow), 4);

    /* all the sections go in the vbox */
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(infWindow), vbox);
    gtk_widget_show(vbox);

    /***** image location label ******************************/

    info_label = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(vbox), info_label, FALSE, FALSE, 0);
    gtk_widget_show(info_label);

    /***** list box with scrolled window *********************/

    /* This is the scrolled window to put the GtkList widget inside */
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scrolled_window, LIST_WIDTH, 250);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);
    gtk_widget_show(scrolled_window);

    /// info_list = gtk_clist_new(1);
    info_list = gtk_tree_view_new();

    if (info_list) {
      GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
      GtkTreeSelection *select;

      if (renderer) {
        gtk_tree_view_insert_column_with_attributes(
            GTK_TREE_VIEW(info_list), -1, "File", renderer, "text", 0, NULL);
      }

      select = gtk_tree_view_get_selection(GTK_TREE_VIEW(info_list));

      if (select) {
        gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);
        g_signal_connect(G_OBJECT(select), "changed",
                         G_CALLBACK(info_selectionCB), NULL);
      }

      gtk_container_add(GTK_CONTAINER(scrolled_window), info_list);
      gtk_widget_show(info_list);
    }

    /***** next and previous buttons *************************/

    // table = gtk_table_new(1, 2, TRUE);
    table = gtk_grid_new();
    gtk_box_pack_start(GTK_BOX(vbox), table, FALSE, FALSE, 0);
    gtk_widget_show(table);

    button = gtk_button_new_with_label("Previous");
    gtk_widget_set_can_focus(GTK_WIDGET(button), FALSE);
    g_signal_connect(button, "clicked", G_CALLBACK(info_prevCB), NULL);
    gtk_grid_attach(GTK_GRID(table), button, 0, 1, 0, 1);
    gtk_widget_show(button);

    button = gtk_button_new_with_label("Next");
    gtk_widget_set_can_focus(GTK_WIDGET(button), FALSE);
    g_signal_connect(button, "clicked", G_CALLBACK(info_nextCB), NULL);
    gtk_grid_attach(GTK_GRID(table), button, 1, 2, 0, 1);
    gtk_widget_show(button);

    /***** highlight radio buttons * *************************/

    radio = gtk_check_button_new_with_label("Highlight Image");
    gtk_widget_set_can_focus(GTK_WIDGET(radio), FALSE);
    gtk_box_pack_start(GTK_BOX(vbox), radio, FALSE, FALSE, 0);
    g_signal_connect(radio, "toggled", G_CALLBACK(info_highlightCB), NULL);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio),
                                 (globals.do_highlight & DO_HIGHLIGHT) ? TRUE
                                                                       : FALSE);
    gtk_widget_show(radio);

    radio = gtk_check_button_new_with_label("Highlight Duplicates");
    gtk_widget_set_can_focus(GTK_WIDGET(radio), FALSE);
    gtk_box_pack_start(GTK_BOX(vbox), radio, FALSE, FALSE, 0);
    g_signal_connect(radio, "toggled", G_CALLBACK(info_highlight_dupsCB), NULL);
    gtk_toggle_button_set_active(
        GTK_TOGGLE_BUTTON(radio),
        (globals.do_highlight & DO_HIGHLIGHT_DUPS) ? TRUE : FALSE);
    gtk_widget_show(radio);

    /*********************************************************/

    button = gtk_button_new_with_label("Dismiss");
    gtk_widget_set_can_focus(GTK_WIDGET(button), FALSE);
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_widget_hide),
                             infWindow);
    gtk_box_pack_end(GTK_BOX(vbox), button, FALSE, FALSE, 0);
    gtk_widget_show(button);
  }

  /* save the x and y coords */
  info_x = x;
  info_y = y;

  /* set the coord label */
  snprintf(buf, sizeof(buf), "Image coordinates: %ux%u", info_x, info_y);
  gtk_label_set_text(GTK_LABEL(info_label), buf);

  /* clear and optionalls reset the highlites */
  clear_all_highlights(1);
  if (globals.do_highlight & DO_HIGHLIGHT) {
    set_highlight();
  }
  if (globals.do_highlight & DO_HIGHLIGHT_DUPS) {
    set_highlight_dups();
  }
  highlight_changed();

  /* fill the list box */
  info_fill_list();

  gtk_widget_show(infWindow);

  fprintf(stderr, "%s: exiting\n", __func__);
}
