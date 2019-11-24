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
/* options.c by Paul Wilkins 1/2/2000 */

#include "options.h"
#include "draw_image.h"
#include "globals.h"
#include <ctype.h>

static GtkWidget *optWindow = NULL;

GtkWidget *find_child(GtkWidget *parent, const gchar *name) {
  if (g_ascii_strcasecmp(gtk_widget_get_name((GtkWidget *)parent),
                         (gchar *)name) == 0) {
    return parent;
  }

  if (GTK_IS_BIN(parent)) {
    GtkWidget *child = gtk_bin_get_child(GTK_BIN(parent));
    return find_child(child, name);
  }

  if (GTK_IS_CONTAINER(parent)) {
    GList *children = gtk_container_get_children(GTK_CONTAINER(parent));
    while (children) {
      GtkWidget *widget = find_child(children->data, name);
      if (widget != NULL) {
        return widget;
      }
      children = g_list_next(children);
    }
  }

  return NULL;
}

void refresh_options_win(GtkWidget *dialog) {

  if (dialog) {
    char buf[8];
    GtkWidget *ptr;

    if (optWindow == NULL) {
      optWindow = dialog;
    }

    ptr = find_child(optWindow, "pix_width");
    if (ptr) {
      snprintf(buf, sizeof(buf), "%u", globals.new_opt.pixW);
      gtk_entry_set_text(GTK_ENTRY(ptr), buf);
      if (globals.new_opt.opt_alg == PIX_SIZE) {
        gtk_editable_set_editable(GTK_EDITABLE(ptr), TRUE);
        gtk_widget_set_sensitive(ptr, TRUE);
      } else {
        gtk_editable_set_editable(GTK_EDITABLE(ptr), FALSE);
        gtk_widget_set_sensitive(ptr, FALSE);
      }
    } else {
      g_printerr("Error: Unable to find pix_width entry\n");
    }

    ptr = find_child(optWindow, "pix_heigth");
    if (ptr) {
      snprintf(buf, sizeof(buf), "%u", globals.new_opt.pixH);
      gtk_entry_set_text(GTK_ENTRY(ptr), buf);
      if (globals.new_opt.opt_alg == PIX_SIZE) {
        gtk_editable_set_editable(GTK_EDITABLE(ptr), TRUE);
        gtk_widget_set_sensitive(ptr, TRUE);
      } else {
        gtk_editable_set_editable(GTK_EDITABLE(ptr), FALSE);
        gtk_widget_set_sensitive(ptr, FALSE);
      }
    } else {
      g_printerr("Error: Unable to find pix_heigth entry\n");
    }

    ptr = find_child(optWindow, "n_pix_width");
    if (ptr) {
      snprintf(buf, sizeof(buf), "%u", globals.new_opt.nPixW);
      gtk_entry_set_text(GTK_ENTRY(ptr), buf);
      if (globals.new_opt.opt_alg == PIX_SIZE) {
        gtk_editable_set_editable(GTK_EDITABLE(ptr), FALSE);
        gtk_widget_set_sensitive(ptr, FALSE);
      } else {
        gtk_editable_set_editable(GTK_EDITABLE(ptr), TRUE);
        gtk_widget_set_sensitive(ptr, TRUE);
      }
    } else {
      g_printerr("Error: Unable to find n_pix_width entry\n");
    }

    ptr = find_child(optWindow, "n_pix_heigth");
    if (ptr) {
      snprintf(buf, sizeof(buf), "%u", globals.new_opt.nPixH);
      gtk_entry_set_text(GTK_ENTRY(ptr), buf);
      if (globals.new_opt.opt_alg == PIX_SIZE) {
        gtk_editable_set_editable(GTK_EDITABLE(ptr), FALSE);
        gtk_widget_set_sensitive(ptr, FALSE);
      } else {
        gtk_editable_set_editable(GTK_EDITABLE(ptr), TRUE);
        gtk_widget_set_sensitive(ptr, TRUE);
      }
    } else {
      g_printerr("Error: Unable to find n_pix_heigth entry\n");
    }

    ptr = find_child(optWindow, "proximity");
    if (ptr) {
      snprintf(buf, sizeof(buf), "%u", globals.new_opt.proximity);
      gtk_entry_set_text(GTK_ENTRY(ptr), buf);
    } else {
      g_printerr("Error: Unable to find proximity entry\n");
    }
  } else {
    g_printerr("Error: dialog is NULL\n");
  }
}

void image_size_button_toggled_cb(GtkToggleButton *button, gpointer user_data) {
  (void)user_data;

  g_printerr("%s: enter\n", __func__);

  if (gtk_toggle_button_get_active(button)) {
    globals.new_opt.opt_alg = PIX_SIZE;
  } else {
    globals.new_opt.opt_alg = PIX_COUNT;
  }
  refresh_options_win(optWindow);
}

gboolean pix_width_focus_out_event_cb(GtkWidget *widget, GdkEvent *event,
                                      gpointer user_data) {
  (void)widget;
  (void)event;
  (void)user_data;
  const gchar *txt = gtk_entry_get_text(GTK_ENTRY(widget));

  g_printerr("%s: enter\n", __func__);

  if (sscanf(txt, "%u", &globals.new_opt.pixW) == 1) {
    if (calc_dimensions(&globals.new_opt) == FALSE) {
      g_printerr("error: %s can't compute dimension\n", __func__);
      return FALSE;
    }
  } else {
    g_printerr("Error: cant make sense of %s for pix_width\n", txt);
  }
  refresh_options_win(optWindow);
  return FALSE;
}

gboolean pix_heigth_focus_out_event_cb(GtkWidget *widget, GdkEvent *event,
                                       gpointer user_data) {
  (void)widget;
  (void)event;
  (void)user_data;
  const gchar *txt = gtk_entry_get_text(GTK_ENTRY(widget));

  g_printerr("%s: enter\n", __func__);

  if (sscanf(txt, "%u", &globals.new_opt.pixH) == 1) {
    if (calc_dimensions(&globals.new_opt) == FALSE) {
      g_printerr("error: %s can't compute dimension\n", __func__);
      return FALSE;
    }
  } else {
    g_printerr("Error: cant make sense of %s for pix_heigth\n", txt);
  }
  refresh_options_win(optWindow);
  return FALSE;
}

gboolean proximity_focus_out_event_cb(GtkWidget *widget, GdkEvent *event,
                                      gpointer user_data) {
  (void)widget;
  (void)event;
  (void)user_data;
  const gchar *txt = gtk_entry_get_text(GTK_ENTRY(widget));

  g_printerr("%s: enter\n", __func__);

  if (sscanf(txt, "%u", &globals.new_opt.proximity) != 1) {
    g_printerr("Error: cant make sense of %s for proximity\n", txt);
  }
  refresh_options_win(optWindow);
  return FALSE;
}

gboolean n_pix_width_focus_out_event_cb(GtkWidget *widget, GdkEvent *event,
                                        gpointer user_data) {
  (void)widget;
  (void)event;
  (void)user_data;
  const gchar *txt = gtk_entry_get_text(GTK_ENTRY(widget));

  g_printerr("%s: enter\n", __func__);

  if (sscanf(txt, "%u", &globals.new_opt.nPixW) == 1) {
    if (calc_dimensions(&globals.new_opt) == FALSE) {
      g_printerr("error: %s can't compute dimension\n", __func__);
      return FALSE;
    }
  } else {
    g_printerr("Error: cant make sense of %s for n_pix_width\n", txt);
  }
  refresh_options_win(optWindow);
  return FALSE;
}

gboolean n_pix_heigth_focus_out_event_cb(GtkWidget *widget, GdkEvent *event,
                                         gpointer user_data) {
  (void)widget;
  (void)event;
  (void)user_data;
  const gchar *txt = gtk_entry_get_text(GTK_ENTRY(widget));

  g_printerr("%s: enter\n", __func__);

  if (sscanf(txt, "%u", &globals.new_opt.nPixH) == 1) {
    if (calc_dimensions(&globals.new_opt) == FALSE) {
      g_printerr("error: %s can't compute dimension\n", __func__);
      return FALSE;
    }
  } else {
    g_printerr("Error: cant make sense of %s for n_pix_heigth\n", txt);
  }
  refresh_options_win(optWindow);
  return FALSE;
}
