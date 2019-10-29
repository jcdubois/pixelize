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
/* help.c by Paul Wilkins 1/2/2000 */

#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include "help.h"
#include "license.h"

#define HELP_TXT \
"\n\
Pixelize v1.0.0\n\
\n\
By: Paul Wilkins\n\
    pwilkins@wilkins.ne.mediaone.net\n\
\n\
Pixelize is a program that will use many scaled down images\n\
to try to duplicate, as closely as possible, another image.\n\
\n\
Pixelize works by splitting up the image you want rendered\n\
(or duplicated) into a grid of small rectangular areas.  Each\n\
area is analyzed, and replaced with an image chosen from a\n\
large database of images.  Pixelize tries to pick images that\n\
best matches each area.\n\
\n\
Pixelize may be told how to size the grid, by either the number\n\
of areas in the grid, or by the dimensions of the areas in the\n\
grid.\n\
\n\
Once Pixelize has rendered an image, it displays the grid of\n\
small images that it has chosen, in place of the original image.\n\
By clicking on each small image, Pixelize will allow you to\n\
choose alternate images from a list of images, that also fit\n\
well into that area.\n\
\n\
Pixelize works best when it can choose images from a very large\n\
database of images.  With about 1000 images, Pixelize can do a\n\
reasonable job.\n\
\n\
Steps to create a \"rendered\" image:\n\
\n\
1) Create the database that Pixelize requires. Use the included\n\
   program, make_db.\n\
\n\
2) Start Pixelize.  Open an image using \"Open\" from the File menu.\n\
\n\
3) Render the image.  Use \"Render\" from the Options manu.\n\
\n\
"
 
void popup_window(GtkWidget **dialog, char *txt, char *title){

   GtkWidget *vbox;
   GtkWidget *scrolled_win;
   GtkWidget *label;
   GtkWidget *button;
 
   if (!*dialog) {

      *dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
      gtk_window_set_title(GTK_WINDOW(*dialog), title);

      gtk_signal_connect(GTK_OBJECT(*dialog), "destroy",
                        GTK_SIGNAL_FUNC(gtk_widget_destroyed),
                        dialog);
      gtk_container_set_border_width(GTK_CONTAINER(*dialog), 5); 

      gtk_window_set_title(GTK_WINDOW(*dialog), title);
      gtk_widget_set_usize(*dialog, 470, 470);

      vbox = gtk_vbox_new(FALSE, 0);
      gtk_container_add(GTK_CONTAINER(*dialog), vbox);
      gtk_widget_show(vbox);

      scrolled_win = gtk_scrolled_window_new (NULL, NULL);
      gtk_container_set_border_width(GTK_CONTAINER(scrolled_win), 5); 
      gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW (scrolled_win),
                                     GTK_POLICY_AUTOMATIC,
                                     GTK_POLICY_AUTOMATIC);
      gtk_box_pack_start(GTK_BOX(vbox), scrolled_win, TRUE, TRUE, 0);
      gtk_widget_show(scrolled_win);

      label = gtk_label_new(txt);
      gtk_signal_connect(GTK_OBJECT(label), "destroy",
                          GTK_SIGNAL_FUNC(gtk_widget_destroyed), &label);
      gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
      gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_win), label);
      gtk_widget_show(label);

      button = gtk_button_new_with_label("Dismiss");
      gtk_signal_connect_object(GTK_OBJECT(button), "clicked",
                                 GTK_SIGNAL_FUNC(gtk_widget_destroy),
                                 GTK_OBJECT(*dialog));
      gtk_box_pack_end(GTK_BOX(vbox), button, FALSE, FALSE, 0);
      GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);
      /* gtk_widget_grab_default(button);  This puts an ugly box around the botton */
      gtk_widget_show(button);
 
   }

   if (!GTK_WIDGET_VISIBLE(*dialog))
      gtk_widget_show(*dialog);
   else
      gtk_widget_destroy(*dialog);

}
 

void license_popup(){
   static GtkWidget *dialog = NULL;
 
   popup_window(&dialog, LICENSE_TXT, "License");
}
 
 

void help_popup(){
 
   static GtkWidget *dialog = NULL;

   popup_window(&dialog, HELP_TXT, "Help");
}



