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

#include "help.h"
#include "license.h"
#include <gtk/gtk.h>

#define HELP_TXT                                                               \
  "Pixelize is a program that will use many scaled down images\n\
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
"

void about_popup() {
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file("pixelize.png", NULL);

  GtkWidget *dialog = gtk_about_dialog_new();
  gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "Pixelize");
  gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "2.0.0");
  gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), "(c) Paul Wilkins");
  gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), HELP_TXT);
  gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog),
                               "http://lashwhip.com/pixelize.html");
  if (pixbuf) {
    gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), pixbuf);
    g_object_unref(pixbuf), pixbuf = NULL;
  }
  gtk_about_dialog_set_license(GTK_ABOUT_DIALOG(dialog), LICENSE_TXT);
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}
