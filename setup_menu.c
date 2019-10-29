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

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
/* #include <gdk_imlib.h> */

#include "callback_menu.h"
#include "globals.h"
#include "menu.h"
#include "options.h"

/* create the menubar */
GtkWidget *setup_menu(GtkWidget *parent) {
  GtkWidget *menu_bar;
  GtkWidget *fileM, *fileI;
  GtkWidget *openI;
  GtkWidget *saveI;
  GtkWidget *sepI;
  GtkWidget *quitI;
  GtkWidget *optM, *optI;
  GtkWidget *renderI;
  GtkWidget *optionsI;
  GtkWidget *helpM, *helpI;
  GtkWidget *hlpI;
  GtkWidget *licenseI;

  /********************************************************/

  /* File */
  fileM = gtk_menu_new(); /* Remember: don't gtk_widget_show the menu */

  /* Open */
  openI = gtk_menu_item_new_with_label("Open");
  gtk_menu_shell_append(GTK_MENU_SHELL(fileM), openI);
  gtk_widget_show(openI);
  g_signal_connect_swapped(GTK_OBJECT(openI), "activate", G_CALLBACK(openCB),
                           NULL);

  /* Save */
  saveI = gtk_menu_item_new_with_label("Save");
  gtk_menu_shell_append(GTK_MENU_SHELL(fileM), saveI);
  gtk_widget_show(saveI);
  g_signal_connect_swapped(GTK_OBJECT(saveI), "activate", G_CALLBACK(saveCB),
                           NULL);

  /* separator */
  sepI = gtk_menu_item_new();
  gtk_menu_shell_append(GTK_MENU_SHELL(fileM), sepI);
  gtk_widget_show(sepI);

  /* Quit */
  quitI = gtk_menu_item_new_with_label("Quit");
  gtk_menu_shell_append(GTK_MENU_SHELL(fileM), quitI);
  gtk_widget_show(quitI);
  g_signal_connect_swapped(GTK_OBJECT(quitI), "activate", G_CALLBACK(quitCB),
                           NULL);

  /********************************************************/

  /* Options */
  optM = gtk_menu_new(); /* Remember: don't gtk_widget_show the menu */

  /* render */
  renderI = gtk_menu_item_new_with_label("Render");
  gtk_menu_shell_append(GTK_MENU_SHELL(optM), renderI);
  gtk_widget_show(renderI);
  g_signal_connect_swapped(GTK_OBJECT(renderI), "activate",
                           G_CALLBACK(renderCB), NULL);

  /* Options */
  optionsI = gtk_menu_item_new_with_label("Options");
  gtk_menu_shell_append(GTK_MENU_SHELL(optM), optionsI);
  gtk_widget_show(optionsI);
  g_signal_connect_swapped(GTK_OBJECT(optionsI), "activate",
                           G_CALLBACK(optionsCB), NULL);

  /********************************************************/

  /* create the "Help" pulldown menu */
  helpM = gtk_menu_new();

  /* Help */
  hlpI = gtk_menu_item_new_with_label("Help");
  gtk_menu_shell_append(GTK_MENU_SHELL(helpM), hlpI);
  gtk_widget_show(hlpI);
  g_signal_connect_swapped(GTK_OBJECT(hlpI), "activate", G_CALLBACK(helpCB),
                           NULL);
  /* Help */
  licenseI = gtk_menu_item_new_with_label("License");
  gtk_menu_shell_append(GTK_MENU_SHELL(helpM), licenseI);
  gtk_widget_show(licenseI);
  g_signal_connect_swapped(GTK_OBJECT(licenseI), "activate",
                           G_CALLBACK(licenseCB), NULL);

  /********************************************************/

  /* tell the menubar who is the heplp widget */

  menu_bar = gtk_menu_bar_new();
  gtk_box_pack_start(GTK_BOX(parent), menu_bar, FALSE, FALSE, 0);
  gtk_widget_show(menu_bar);

  fileI = gtk_menu_item_new_with_label("File");
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileI), fileM);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), fileI);
  gtk_widget_show(fileI);

  optI = gtk_menu_item_new_with_label("Options");
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(optI), optM);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), optI);
  gtk_widget_show(optI);

  helpI = gtk_menu_item_new_with_label("Help");
  gtk_menu_item_set_right_justified(GTK_MENU_ITEM(helpI), TRUE);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(helpI), helpM);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), helpI);
  gtk_widget_show(helpI);

  return menu_bar;
}
