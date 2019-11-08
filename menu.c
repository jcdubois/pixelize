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
/* setup_menu.c by Paul Wilkins 1/2/2000 */

#include "menu.h"
#include "file_dialog.h"
#include "help.h"
#include "options.h"
#include "render.h"

static void open_callback(GtkMenuItem *menuitem, gpointer user_data) {
  (void)menuitem;
  (void)user_data;

  file_open_dialog();
}

static void save_callback(GtkMenuItem *menuitem, gpointer user_data) {
  (void)menuitem;
  (void)user_data;

  file_save_dialog();
}

static void quit_callback(GtkMenuItem *menuitem, gpointer user_data) {
  (void)menuitem;
  (void)user_data;

  gtk_main_quit();
}

static gpointer render_compute_thread(gpointer data) {
  (void)data;

  render();

  return NULL;
}

static void render_callback(GtkMenuItem *menuitem, gpointer user_data) {
  (void)menuitem;
  (void)user_data;

  g_thread_new("render", render_compute_thread, NULL);
}

static void license_callback(GtkMenuItem *menuitem, gpointer user_data) {
  (void)menuitem;
  (void)user_data;

  license_popup();
}

static void help_callback(GtkMenuItem *menuitem, gpointer user_data) {
  (void)menuitem;
  (void)user_data;

  help_popup();
}

/* create the menubar */
void setup_menu(GtkWidget *parent) {

  /********************************************************/
  /* create the menu bar */

  GtkWidget *menu_bar = gtk_menu_bar_new();

  if (menu_bar) {
    GtkWidget *menu_item;
    GtkWidget *bar_item;

    gtk_grid_attach(GTK_GRID(parent), menu_bar, 0, 0, 2, 1);
    gtk_widget_show(menu_bar);

    /********************************************************/
    /* create the "File" pulldown menu */

    bar_item = gtk_menu_new();

    if (bar_item) {
      /* Open */
      menu_item = gtk_menu_item_new_with_label("Open");
      if (menu_item) {
        gtk_menu_shell_append(GTK_MENU_SHELL(bar_item), menu_item);
        gtk_widget_show(menu_item);
        g_signal_connect(menu_item, "activate", G_CALLBACK(open_callback),
                         NULL);
      }

      /* Save */
      menu_item = gtk_menu_item_new_with_label("Save");
      if (menu_item) {
        gtk_menu_shell_append(GTK_MENU_SHELL(bar_item), menu_item);
        gtk_widget_show(menu_item);
        g_signal_connect(menu_item, "activate", G_CALLBACK(save_callback),
                         NULL);
      }

      /* separator */
      menu_item = gtk_menu_item_new();
      if (menu_item) {
        gtk_menu_shell_append(GTK_MENU_SHELL(bar_item), menu_item);
        gtk_widget_show(menu_item);
      }

      /* Quit */
      menu_item = gtk_menu_item_new_with_label("Quit");
      if (menu_item) {
        gtk_menu_shell_append(GTK_MENU_SHELL(bar_item), menu_item);
        gtk_widget_show(menu_item);
        g_signal_connect(menu_item, "activate", G_CALLBACK(quit_callback),
                         NULL);
      }

      menu_item = gtk_menu_item_new_with_label("File");
      if (menu_item) {
        gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), bar_item);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_item);
        gtk_widget_show(menu_item);
      }
    }

    /********************************************************/
    /* create the "Options" pulldown menu */

    bar_item = gtk_menu_new();

    if (bar_item) {
      /* render */
      menu_item = gtk_menu_item_new_with_label("Render");
      if (menu_item) {
        gtk_menu_shell_append(GTK_MENU_SHELL(bar_item), menu_item);
        gtk_widget_show(menu_item);
        g_signal_connect(menu_item, "activate", G_CALLBACK(render_callback),
                         NULL);
      }

      /* Options */
      menu_item = gtk_menu_item_new_with_label("Options");
      if (menu_item) {
        gtk_menu_shell_append(GTK_MENU_SHELL(bar_item), menu_item);
        gtk_widget_show(menu_item);
        g_signal_connect(menu_item, "activate", G_CALLBACK(optionsCB), NULL);
      }

      menu_item = gtk_menu_item_new_with_label("Options");
      if (menu_item) {
        gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), bar_item);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_item);
        gtk_widget_show(menu_item);
      }
    }

    /********************************************************/
    /* create the "Help" pulldown menu */

    bar_item = gtk_menu_new();

    if (bar_item) {
      /* Help */
      menu_item = gtk_menu_item_new_with_label("Help");
      if (menu_item) {
        gtk_menu_shell_append(GTK_MENU_SHELL(bar_item), menu_item);
        gtk_widget_show(menu_item);
        g_signal_connect(menu_item, "activate", G_CALLBACK(help_callback),
                         NULL);
      }

      /* License */
      menu_item = gtk_menu_item_new_with_label("License");
      if (menu_item) {
        gtk_menu_shell_append(GTK_MENU_SHELL(bar_item), menu_item);
        gtk_widget_show(menu_item);
        g_signal_connect(menu_item, "activate", G_CALLBACK(license_callback),
                         NULL);
      }

      menu_item = gtk_menu_item_new_with_label("Help");
      if (menu_item) {
        gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), bar_item);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_item);
        gtk_widget_show(menu_item);
      }
    }
  }
}
