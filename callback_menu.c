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
/* callback_menu.c by Paul Wilkins 1/2/2000 */

#include "callback_menu.h"
#include "file_dialog.h"
#include "globals.h"
#include "help.h"
#include "menu.h"
#include "options.h"
#include "render.h"
#include <gtk/gtk.h>
#include <stdio.h>

gint delete_event(GtkWidget *w, GdkEvent *e, gpointer d) {
  (void)w;
  (void)e;
  (void)d;

  return FALSE; /* will call our destroy function */
}

void destroy(GtkWidget *widget, gpointer data) {
  (void)widget;
  (void)data;

  gtk_main_quit();
}

/* quit the program */
void quitCB(gpointer data) {
  (void)data;

  gtk_main_quit();
}

/* open an image to render */
void openCB(gpointer data) {
  (void)data;

  file_open_dialog(NULL);
}

/* save the rendered image */
void saveCB(gpointer data) {
  (void)data;

  file_save_dialog(NULL);
}

/* render the image */
void renderCB(gpointer data) {
  (void)data;

  render();
}

/* display licence info */
void licenseCB(gpointer data) {
  (void)data;

  license_popup();
}

/* display help info */
void helpCB(gpointer data) {
  (void)data;

  help_popup();
}
