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


#include <stdio.h>
#include <stdlib.h>
#ifdef USE_GNOME
#include <gnome.h>
#endif
#include <gtk/gtk.h>
/* #include <gdk_imlib.h> */

#include "globals.h"
#include "menu.h"
#include "options.h"

extern void quitCB(gpointer);
extern void openCB(gpointer);
extern void saveCB(gpointer);
extern void renderCB(gpointer);
extern void optionsCB(gpointer);
extern void licenseCB(gpointer);
extern void helpCB(gpointer);


#ifdef USE_GNOME

GnomeUIInfo filemenu[] = {
   {GNOME_APP_UI_ITEM, 
      "Open Image", "Open an image to render to",
      openCB, NULL, NULL, 
      GNOME_APP_PIXMAP_NONE, NULL, 0, 0, NULL},
   {GNOME_APP_UI_ITEM, 
      "Save Image", "Save the rendered image",
      saveCB, NULL, NULL, 
      GNOME_APP_PIXMAP_NONE, NULL, 0, 0, NULL},
   GNOMEUIINFO_MENU_EXIT_ITEM(quitCB, NULL),
   GNOMEUIINFO_END
};
GnomeUIInfo optmenu[] = {
   {GNOME_APP_UI_ITEM, 
      "Render", "Render the image",
      renderCB, NULL, NULL, 
      GNOME_APP_PIXMAP_NONE, NULL, 0, 0, NULL},
   {GNOME_APP_UI_ITEM, 
      "Options", "Options",
      optionsCB, NULL, NULL, 
      GNOME_APP_PIXMAP_NONE, NULL, 0, 0, NULL},
   GNOMEUIINFO_END
};
GnomeUIInfo helpmenu[] = {
   {GNOME_APP_UI_ITEM, 
      "License", "License",
      licenseCB, NULL, NULL, 
      GNOME_APP_PIXMAP_NONE, NULL, 0, 0, NULL},
   GNOMEUIINFO_HELP("grpn"),
   GNOMEUIINFO_END
};
GnomeUIInfo mainmenu[] = {
   GNOMEUIINFO_MENU_FILE_TREE(filemenu),
   GNOMEUIINFO_MENU_HELP_TREE(optmenu),
   GNOMEUIINFO_MENU_HELP_TREE(helpmenu),
   GNOMEUIINFO_END
};


/* create the menubar */
GtkWidget *setup_menu(GtkWidget *parent)
{
   gnome_app_create_menus(GNOME_APP(parent), mainmenu);

   return NULL;
}

#else /* not USE_GNOME */

/* create the menubar */
GtkWidget *setup_menu(GtkWidget *parent)
{
   GtkWidget *menu_bar;
   GtkWidget    *fileM, *fileI;
   GtkWidget       *openI;
   GtkWidget       *saveI;
   GtkWidget       *sepI;
   GtkWidget       *quitI;
   GtkWidget    *optM, *optI;
   GtkWidget       *renderI;
   GtkWidget       *optionsI;
   GtkWidget    *helpM, *helpI;
   GtkWidget       *hlpI;
   GtkWidget       *licenseI;



   /********************************************************/

   /* File */
   fileM = gtk_menu_new(); /* Remember: don't gtk_widget_show the menu */

      /* Open */
      openI = gtk_menu_item_new_with_label("Open");
      gtk_menu_append(GTK_MENU(fileM), openI);
      gtk_widget_show(openI);
      gtk_signal_connect_object(GTK_OBJECT(openI), "activate",
				GTK_SIGNAL_FUNC(openCB), NULL);

      /* Save */
      saveI = gtk_menu_item_new_with_label("Save");
      gtk_menu_append(GTK_MENU(fileM), saveI);
      gtk_widget_show(saveI);
      gtk_signal_connect_object(GTK_OBJECT(saveI), "activate",
				GTK_SIGNAL_FUNC(saveCB), NULL);

      /* separator */
      sepI = gtk_menu_item_new();
      gtk_menu_append(GTK_MENU(fileM), sepI);
      gtk_widget_show(sepI);

      /* Quit */
      quitI = gtk_menu_item_new_with_label("Quit");
      gtk_menu_append(GTK_MENU(fileM), quitI);
      gtk_widget_show(quitI);
      gtk_signal_connect_object(GTK_OBJECT(quitI), "activate",
				GTK_SIGNAL_FUNC(quitCB), NULL);


   /********************************************************/

   /* Options */
   optM = gtk_menu_new(); /* Remember: don't gtk_widget_show the menu */

      /* render */
      renderI = gtk_menu_item_new_with_label("Render");
      gtk_menu_append(GTK_MENU(optM), renderI);
      gtk_widget_show(renderI);
      gtk_signal_connect_object(GTK_OBJECT(renderI), "activate",
				GTK_SIGNAL_FUNC(renderCB), NULL);

      /* Options */
      optionsI = gtk_menu_item_new_with_label("Options");
      gtk_menu_append(GTK_MENU(optM), optionsI);
      gtk_widget_show(optionsI);
      gtk_signal_connect_object(GTK_OBJECT(optionsI), "activate",
				GTK_SIGNAL_FUNC(optionsCB), NULL);


   /********************************************************/

   /* create the "Help" pulldown menu */
   helpM = gtk_menu_new();

      /* Help */
      hlpI = gtk_menu_item_new_with_label("Help");
      gtk_menu_append(GTK_MENU(helpM), hlpI);
      gtk_widget_show(hlpI);
      gtk_signal_connect_object(GTK_OBJECT(hlpI), "activate",
				GTK_SIGNAL_FUNC(helpCB), NULL);
      /* Help */
      licenseI = gtk_menu_item_new_with_label("License");
      gtk_menu_append(GTK_MENU(helpM), licenseI);
      gtk_widget_show(licenseI);
      gtk_signal_connect_object(GTK_OBJECT(licenseI), "activate",
				GTK_SIGNAL_FUNC(licenseCB), NULL);

   /********************************************************/

   /* tell the menubar who is the heplp widget */

   menu_bar = gtk_menu_bar_new();
   gtk_box_pack_start(GTK_BOX(parent), menu_bar, FALSE, FALSE, 0);
   gtk_widget_show(menu_bar);
 
   fileI = gtk_menu_item_new_with_label("File");
   gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileI), fileM);
   gtk_menu_bar_append(GTK_MENU_BAR(menu_bar), fileI);
   gtk_widget_show(fileI);

   optI = gtk_menu_item_new_with_label("Options");
   gtk_menu_item_set_submenu(GTK_MENU_ITEM(optI), optM);
   gtk_menu_bar_append(GTK_MENU_BAR(menu_bar), optI);
   gtk_widget_show(optI);

   helpI = gtk_menu_item_new_with_label("Help");
   gtk_menu_item_right_justify(GTK_MENU_ITEM(helpI));
   gtk_menu_item_set_submenu(GTK_MENU_ITEM(helpI), helpM);
   gtk_menu_bar_append(GTK_MENU_BAR(menu_bar), helpI);
   gtk_widget_show(helpI);

   return menu_bar;
}
#endif  /* USE_GNOME */

