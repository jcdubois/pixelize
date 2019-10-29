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



#include <stdio.h>
#include <ctype.h>
#include <gtk/gtk.h>
/* #include <gdk_imlib.h> */

#include "globals.h"
#include "read_db.h"
#include "find_match.h"
#include "render_image.h"
#include "draw_image.h"
#include "highlight.h"
#include "info_popup.h"

#define LIST_WIDTH 400

GtkWidget *infWindow = NULL;
GtkWidget *info_list;
GtkWidget *info_label;

int info_x;  /* the current cell */
int info_y;
int ignore_selection = 0;

void info_fill_list();
void clear_all_highlights(int);
void set_highlight();
void set_highlight_dups();

/* pops up a new window with all the options in it */
void info_popup(int x, int y){
   char buf[64];
   GtkWidget *vbox;
   GtkWidget *table;
   GtkWidget *scrolled_window;
   GtkWidget *button;
   GtkWidget *radio;

   void info_prevCB(GtkWidget *, gpointer);
   void info_nextCB(GtkWidget *, gpointer);
   void info_highlightCB(GtkWidget *, gpointer);
   void info_highlight_dupsCB(GtkWidget *, gpointer);
   void info_selectionCB(GtkWidget *, gint, gint, GdkEventButton *, gpointer);

   if (!infWindow) {

      infWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
      gtk_window_set_title(GTK_WINDOW(infWindow), "Image Information");

      gtk_signal_connect(GTK_OBJECT(infWindow), "destroy",
			GTK_SIGNAL_FUNC(gtk_widget_destroyed),
			&infWindow);
      gtk_container_set_border_width(GTK_CONTAINER(infWindow), 4);


      /* all the sections go in the vbox */
      vbox = gtk_vbox_new(FALSE, 0);
      gtk_container_add(GTK_CONTAINER(infWindow), vbox);
      gtk_widget_show(vbox);

      /***** image location label *******************************************************/

      info_label = gtk_label_new("");
      gtk_box_pack_start(GTK_BOX(vbox), info_label, FALSE, FALSE, 0);
      gtk_widget_show(info_label);

      /***** list box with scrolled window *******************************************************/

      /* This is the scrolled window to put the GtkList widget inside */
      scrolled_window = gtk_scrolled_window_new(NULL, NULL);
      gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
	 GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
      gtk_widget_set_usize(scrolled_window, LIST_WIDTH, 250);
      gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);
      gtk_widget_show(scrolled_window);

      info_list = gtk_clist_new(1);
      GTK_WIDGET_SET_FLAGS(info_list, GTK_CAN_DEFAULT);
      gtk_signal_connect(GTK_OBJECT(info_list), "select_row",
	 GTK_SIGNAL_FUNC(info_selectionCB),
	 NULL);
      gtk_clist_set_column_width(GTK_CLIST(info_list), 0, LIST_WIDTH);
      gtk_clist_set_selection_mode(GTK_CLIST(info_list), 
        /* GTK_SELECTION_BROWSE */
        GTK_SELECTION_EXTENDED
      );

      gtk_container_add(GTK_CONTAINER(scrolled_window), info_list);
      /* gtk_scrolled_window_add_with_viewport( GTK_SCROLLED_WINDOW(scrolled_window), info_list); */
      gtk_widget_show(info_list);


      /***** next and previous buttons *******************************************************/

      table = gtk_table_new(1, 2, TRUE);
      gtk_box_pack_start(GTK_BOX(vbox), table, FALSE, FALSE, 0);
      gtk_widget_show(table);

      button = gtk_button_new_with_label("Previous");
      GTK_WIDGET_UNSET_FLAGS(button, GTK_CAN_FOCUS);
      gtk_signal_connect_object(GTK_OBJECT(button), "clicked",
				 GTK_SIGNAL_FUNC(info_prevCB),
				 NULL);
      gtk_table_attach(GTK_TABLE(table), button, 0, 1, 0, 1,
         GTK_FILL, GTK_FILL, 0, 0);
      gtk_widget_show(button);

      button = gtk_button_new_with_label("Next");
      GTK_WIDGET_UNSET_FLAGS(button, GTK_CAN_FOCUS);
      gtk_signal_connect_object(GTK_OBJECT(button), "clicked",
				 GTK_SIGNAL_FUNC(info_nextCB),
				 NULL);
      gtk_table_attach(GTK_TABLE(table), button, 1, 2, 0, 1,
         GTK_FILL, GTK_FILL, 0, 0);
      gtk_widget_show(button);

      /***** highlight radio buttons *******************************************************/

      radio = gtk_check_button_new_with_label("Highlight Image");
      GTK_WIDGET_UNSET_FLAGS(radio, GTK_CAN_FOCUS);
      gtk_box_pack_start(GTK_BOX(vbox), radio, FALSE, FALSE, 0);
      gtk_signal_connect(GTK_OBJECT(radio), "toggled",
	 GTK_SIGNAL_FUNC(info_highlightCB), NULL);
      gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(radio),
         (globals.do_highlight&DO_HIGHLIGHT) ? TRUE : FALSE);
      gtk_widget_show(radio);

      radio = gtk_check_button_new_with_label("Highlight Duplicates");
      GTK_WIDGET_UNSET_FLAGS(radio, GTK_CAN_FOCUS);
      gtk_box_pack_start(GTK_BOX(vbox), radio, FALSE, FALSE, 0);
      gtk_signal_connect(GTK_OBJECT(radio), "toggled",
	 GTK_SIGNAL_FUNC(info_highlight_dupsCB), NULL);
      gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(radio),
         (globals.do_highlight&DO_HIGHLIGHT_DUPS) ? TRUE : FALSE);
      gtk_widget_show(radio);

      /************************************************************/

      button = gtk_button_new_with_label("Dismiss");
      GTK_WIDGET_UNSET_FLAGS(button, GTK_CAN_FOCUS);
      gtk_signal_connect_object(GTK_OBJECT(button), "clicked",
                                 GTK_SIGNAL_FUNC(gtk_widget_hide),
                                 GTK_OBJECT(infWindow));
      gtk_box_pack_end(GTK_BOX(vbox), button, FALSE, FALSE, 0);
      gtk_widget_show(button);


   }

   /* save the x and y coords */
   info_x = x;
   info_y = y;

   /* set the coord label */
   sprintf(buf, "Image coordinates: %dx%d", info_x, info_y);
   gtk_label_set(GTK_LABEL(info_label), buf);

   /* clear and optionalls reset the highlites */
   clear_all_highlights(1);
   if(globals.do_highlight & DO_HIGHLIGHT){
      set_highlight();
   }
   if(globals.do_highlight & DO_HIGHLIGHT_DUPS){
      set_highlight_dups();
   }
   highlight_changed();

   /* fill the list box */
   info_fill_list();

   if(!GTK_WIDGET_VISIBLE(infWindow)) {
      gtk_widget_show(infWindow);
   }
}

void info_fill_list(){
   int i;
   char *ary[1];
   struct PIC_DB *db;
   struct IMAGE_INFO *inf;

   ignore_selection = 1;

   inf = &(globals.image[info_y][info_x]);

   /* don't allow the widget to update */
   gtk_clist_freeze(GTK_CLIST(info_list));

   /* set the list entries */
   gtk_clist_clear(GTK_CLIST(info_list));
   for(i=0; i<MAX_MATCHES; i++){

      db = inf->matches[i];

      ary[0] = db->fname;
      gtk_clist_append(GTK_CLIST(info_list), ary);
   }
   
   /* select the current image in the list */
   gtk_clist_select_row(GTK_CLIST(info_list), inf->match_no, 0);

   /* possibly scroll the list */
   if(GTK_VISIBILITY_FULL != gtk_clist_row_is_visible(GTK_CLIST(info_list), inf->match_no)){
      gtk_clist_moveto(GTK_CLIST(info_list), inf->match_no, -1, 0.5, 0.0);
   }

   /* allow the widget to update */
   gtk_clist_thaw(GTK_CLIST(info_list));

   ignore_selection = 0;

}


void info_selectionCB(
   GtkWidget *clist,
   gint row,
   gint col,
   GdkEventButton *event,
   gpointer data 
){
   struct IMAGE_INFO *inf;

   if(ignore_selection) return;

   inf = &(globals.image[info_y][info_x]);

   /* if it is the current image */
   if(inf->match_no == row) return;

   /* store the new row */
   inf->match_no = row;

   /* finally, draw the new image */
   change_small_image(info_x, info_y);

   /* clear and optionalls reset the highlites */
   clear_all_highlights(0);
   if(globals.do_highlight & DO_HIGHLIGHT){
      set_highlight();
   }
   if(globals.do_highlight & DO_HIGHLIGHT_DUPS){
      set_highlight_dups();
   }
   highlight_changed();
}

void info_prevCB(GtkWidget *widget, gpointer data){
   info_prev();
}

void info_nextCB(GtkWidget *widget, gpointer data){
   info_next();
}

void info_highlightCB(GtkWidget *widget, gpointer data){
   struct IMAGE_INFO *inf;

   if(TRUE == GTK_TOGGLE_BUTTON(widget)->active){
      globals.do_highlight |= DO_HIGHLIGHT;
   } else {
      globals.do_highlight &= ~DO_HIGHLIGHT;
   }

   inf = &(globals.image[info_y][info_x]);
   if(globals.do_highlight & DO_HIGHLIGHT){
      inf->do_highlight = 1;
   } else {
      inf->do_highlight = 0;
   }

   highlight_changed();
   if(globals.do_highlight) start_highlight_timer();
   else stop_highlight_timer();
}

void info_highlight_dupsCB(GtkWidget *widget, gpointer data){


   if(TRUE == GTK_TOGGLE_BUTTON(widget)->active){
      globals.do_highlight |= DO_HIGHLIGHT_DUPS;
   } else {
      globals.do_highlight &= ~DO_HIGHLIGHT_DUPS;
   }

   if(globals.do_highlight & DO_HIGHLIGHT_DUPS){
      set_highlight_dups();
   } else {
      clear_all_highlights(0);
   }

   highlight_changed();
   if(globals.do_highlight) start_highlight_timer();
   else stop_highlight_timer();
}



void info_prev(){
   struct IMAGE_INFO *inf;

   inf = &(globals.image[info_y][info_x]);

   /* unselect the current image in the list */
   gtk_clist_unselect_row(GTK_CLIST(info_list), inf->match_no, 0);

   /* the previous item in the list */
   inf->match_no--;
   if(inf->match_no < 0) inf->match_no = MAX_MATCHES-1;

   ignore_selection = 1;

   /* select the current image in the list */
   gtk_clist_select_row(GTK_CLIST(info_list), inf->match_no, 0);
   
   /* possibly scroll the list */
   if(GTK_VISIBILITY_FULL != gtk_clist_row_is_visible(GTK_CLIST(info_list), inf->match_no)){
      gtk_clist_moveto(GTK_CLIST(info_list), inf->match_no, -1, 0.0, 0.0);
   }

   ignore_selection = 0;

   /* finally, draw the new image */
   change_small_image(info_x, info_y);

   /* clear and optionalls reset the highlites */
   clear_all_highlights(0);
   if(globals.do_highlight & DO_HIGHLIGHT){
      set_highlight();
   }
   if(globals.do_highlight & DO_HIGHLIGHT_DUPS){
      set_highlight_dups();
   }
   highlight_changed();
}

void info_next(){
   struct IMAGE_INFO *inf;

   inf = &(globals.image[info_y][info_x]);

   /* unselect the current image in the list */
   gtk_clist_unselect_row(GTK_CLIST(info_list), inf->match_no, 0);

   /* the next item in the list */
   inf->match_no++;
   if(inf->match_no >= MAX_MATCHES) inf->match_no = 0;

   ignore_selection = 1;

   /* select the current image in the list */
   gtk_clist_select_row(GTK_CLIST(info_list), inf->match_no, 0);
   
   /* possibly scroll the list */
   if(GTK_VISIBILITY_FULL != gtk_clist_row_is_visible(GTK_CLIST(info_list), inf->match_no)){
      gtk_clist_moveto(GTK_CLIST(info_list), inf->match_no, -1, 1.0, 0.0);
   }

   ignore_selection = 0;

   /* finally, draw the new image */
   change_small_image(info_x, info_y);

   /* clear and optionalls reset the highlites */
   clear_all_highlights(0);
   if(globals.do_highlight & DO_HIGHLIGHT){
      set_highlight();
   }
   if(globals.do_highlight & DO_HIGHLIGHT_DUPS){
      set_highlight_dups();
   }
   highlight_changed();
}

void set_highlight(){
   struct IMAGE_INFO *inf;
   inf = &(globals.image[info_y][info_x]);
   if(globals.do_highlight & DO_HIGHLIGHT){
      inf->do_highlight = 1;
   }
}

void set_highlight_dups(){
   int x, y;
   struct PIC_DB *db, *db2;
   struct IMAGE_INFO *inf, *inf2;

   /* find duplicates and mark them */
   inf = &(globals.image[info_y][info_x]);
   db = inf->matches[inf->match_no];
   for(y=0; y<globals.cur_opt.nPixH; y++){
      for(x=0; x<globals.cur_opt.nPixW; x++){

         if(x == info_x && y == info_y) continue;

	 inf2 = &(globals.image[y][x]);
	 db2 = inf2->matches[inf2->match_no];
	 if(db == db2) inf2->do_highlight = 1;

      }
   }
}

void clear_all_highlights(int clear_current){
   int x, y;
   struct IMAGE_INFO *inf;

   /* clear everyone */
   for(y=0; y<globals.cur_opt.nPixH; y++){
      for(x=0; x<globals.cur_opt.nPixW; x++){

	 if(x == info_x && y == info_y && clear_current==0) continue;

	 inf = &(globals.image[y][x]);
	 inf->do_highlight = 0;
      }
   }
}
