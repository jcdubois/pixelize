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


#include <stdio.h>
#include <gtk/gtk.h>
/* #include <gdk_imlib.h> */

#include "globals.h"
#include "menu.h"
#include "help.h"
#include "render.h"
#include "file_dialog.h"
#include "options.h"

gint delete_event(GtkWidget *w, GdkEvent *e, gpointer d){
   return FALSE;  /* will call our destroy function */
}

void destroy(GtkWidget *widget, gpointer data){
   gtk_main_quit();
}

/* quit the program */
void quitCB(gpointer data){
   gtk_main_quit();
}

/* open an image to render */
void openCB(gpointer data){

   file_open_dialog(NULL);
}

/* save the rendered image */
void saveCB(gpointer data){

   file_save_dialog(NULL);
}

/* save the rendered image */
void renderCB(gpointer data){

   render();
}

void licenseCB(gpointer data){
   license_popup();
}

void helpCB(gpointer data){
   help_popup();
}


