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
/* display.c by Paul Wilkins 1/2/2000 */


#include <stdio.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h> 

#include "globals.h"
#include "read_db.h"
#include "render_image.h"
#include "file_dialog.h"
#include "draw_image.h"
#include "find_match.h"
#include "info_popup.h"
#include "display.h"


/* some callbacks */
static gint ExposeCB(GtkWidget *widget, GdkEventExpose *event, gpointer func_data);
static gint ResizeCB(GtkWidget *widget, GdkEventConfigure *event, gpointer func_data);


static gint ButtonPressEvnt(GtkWidget *widget, GdkEventButton *event, gpointer func_data);
static gint ButtonReleaseEvnt(GtkWidget *widget, GdkEventButton *event, gpointer func_data);
static gint KeyPressEvnt(GtkWidget *widget, GdkEventKey *event, gpointer func_data);
static gint KeyReleaseEvnt(GtkWidget *widget, GdkEventKey *event, gpointer func_data);


GtkWidget *setup_display(GtkWidget *parent){
   int wid, hgt;

   wid = 300;
   hgt = 300;

   /***** create an event box *****/
   globals.ebox = gtk_event_box_new();
   gtk_box_pack_start(GTK_BOX(parent), globals.ebox, TRUE, TRUE, 0);
   gtk_widget_show(globals.ebox);

   /***** create a new scrolled window. *****/
   globals.picScroll = gtk_scrolled_window_new(NULL, NULL);
   gtk_widget_set_usize(globals.picScroll, wid, hgt);  
   gtk_container_set_border_width(GTK_CONTAINER(globals.picScroll), 0);
   gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(globals.picScroll),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
   gtk_container_add(GTK_CONTAINER(globals.ebox), globals.picScroll);
   /* gtk_box_pack_start(GTK_BOX(parent), globals.picScroll, TRUE, TRUE, 0); */
   gtk_widget_show(globals.picScroll);

   /***** create the drawing area *****/
   globals.picDA = gtk_drawing_area_new();
   gtk_drawing_area_size(GTK_DRAWING_AREA(globals.picDA), wid, hgt);

   gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(globals.picScroll),
                                         globals.picDA);
   gtk_widget_show(globals.picDA);


   /* Signals used to handle window ops */
   gtk_signal_connect(GTK_OBJECT(globals.picDA), "expose_event",
		     (GtkSignalFunc)ExposeCB, NULL);
   gtk_signal_connect(GTK_OBJECT(globals.picDA), "configure_event",
		     (GtkSignalFunc)ResizeCB, NULL);


   /* Event signals (Input) */
   gtk_signal_connect(GTK_OBJECT(globals.picDA), "button_press_event",
		     (GtkSignalFunc)ButtonPressEvnt, NULL);
   gtk_signal_connect(GTK_OBJECT(globals.picDA), "button_release_event",
		     (GtkSignalFunc)ButtonReleaseEvnt, NULL);
   gtk_signal_connect_after(GTK_OBJECT(globals.picDA), "key_press_event",
		     (GtkSignalFunc)KeyPressEvnt, NULL);
   gtk_signal_connect_after(GTK_OBJECT(globals.picDA), "key_release_event",
		     (GtkSignalFunc)KeyReleaseEvnt, NULL);


   gtk_widget_set_events(globals.picDA, 
      GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK |
      GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK |
      GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK);

   GTK_WIDGET_SET_FLAGS(globals.picDA, GTK_CAN_FOCUS);
   gtk_widget_grab_focus(globals.picDA);


   return globals.picScroll;
}


static gint ExposeCB(GtkWidget *widget, GdkEventExpose *event, gpointer func_data){
   static int picInitalized = 0;

   if(picInitalized == 0){

      /* save the size of the display */
      globals.disp_w = gdk_screen_width();
      globals.disp_h = gdk_screen_height();

      picInitalized = 1;

      if(globals.start_fname){
			open_image(globals.start_fname);
      }

   }

   redraw_screen(event->area.x, event->area.y, event->area.width, event->area.height);

   return FALSE;
}



static gint ResizeCB(GtkWidget *widget, GdkEventConfigure *event, gpointer func_data){
   int w, h;

   gdk_window_get_size(GTK_WIDGET(globals.ebox)->window, &w, &h);

   /* we need to tell Gtk's brain dead scrollbar when not to appear */
   if(w >= globals.cur_opt.width &&
      h >= globals.cur_opt.height){
      gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(globals.picScroll),
	 GTK_POLICY_NEVER, GTK_POLICY_NEVER);
   } else {
      gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(globals.picScroll),
	 GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
   }

   /* dammit: let the user resize the window! 
      I think gtk_widget_set_usize() messes with this. */
   /*
   gdk_window_set_hints(GTK_WIDGET(globals.topwin)->window,
      0, 0, 300, 300, 0, 0, GDK_HINT_MIN_SIZE);
   */


   return TRUE;
}


static gint KeyPressEvnt(GtkWidget *widget, GdkEventKey *event, gpointer func_data){
   printf("picKeyPressEvnt\n");
   return TRUE;
}
static gint KeyReleaseEvnt(GtkWidget *widget, GdkEventKey *event, gpointer func_data){
   printf("picKeyReleaseEvnt\n");
   switch(event->keyval){
      case GDK_space:
      case GDK_KP_Space:
         globals.show_rendered = !globals.show_rendered;
         redraw_screen(0, 0, globals.cur_opt.width, globals.cur_opt.height);
         break;
   }
   return TRUE;
}



static gint ButtonPressEvnt(GtkWidget *widget, GdkEventButton *event, gpointer func_data) {
   int xx, yy;

   xx = event->x / globals.cur_opt.pixW;
   yy = event->y / globals.cur_opt.pixH;

   if(event->x < 0 || event->x >= globals.cur_opt.width) return TRUE;
   if(event->y < 0 || event->y >= globals.cur_opt.height) return TRUE;

   if(globals.image != NULL){
      if(event->button == 2){
	 info_popup(xx, yy);
	 info_prev();
      } else if (event->button == 3) {
	 info_popup(xx, yy);
	 info_next();
      } else if (event->button == 1) {
	 info_popup(xx, yy);
      }
   }

   return TRUE;
}


static gint ButtonReleaseEvnt(GtkWidget *widget, GdkEventButton *event, gpointer func_data) {
   return TRUE;
}

int resize_window(){
   int x, y, w, h;

   /* calculate the window size */
   if(globals.cur_opt.width > globals.disp_w){
       w = globals.disp_w;
       x = 0;
   } else {
      w = globals.cur_opt.width;
      x = 100;
   }
   if(globals.cur_opt.height > globals.disp_h){
      h = globals.disp_h;
      y = 0;
   } else {
      h = globals.cur_opt.height;
      y = 100;
   }

   gtk_drawing_area_size(GTK_DRAWING_AREA(globals.picDA),
      globals.cur_opt.width, globals.cur_opt.height);

/*
   gdk_window_resize(GTK_WIDGET(globals.topwin)->window, w, h);
   gtk_window_set_default_size(GTK_WINDOW(globals.mainwin), w, h);
*/

   /* set the size of the window, and allow user resizing */
   gtk_widget_set_usize(GTK_WIDGET(globals.picScroll), w, h);
   gtk_window_set_policy(GTK_WINDOW(globals.topwin), TRUE, TRUE, TRUE);

   return 1;
}
