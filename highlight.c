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
/* highlight.c by Paul Wilkins 1/2/2000 */

#include <stdio.h>
#include <gtk/gtk.h>
/* #include <gdk_imlib.h> */
/* #include <gdk/gdk.h> */

#include "globals.h"
#include "draw_image.h"
#include "render_image.h"
#include "highlight.h"

int timeout_exists = 0;

int  highlight_timeout(gpointer data){
   static int state = 0;
   int xx, yy;
   GdkGC *gc;
   struct IMAGE_INFO *inf;

   if(globals.do_highlight == 0) return TRUE;

   /* search for the images to highlight and draw the highlight here */
   for(yy=0; yy<globals.cur_opt.nPixH; yy++){
      for(xx=0; xx<globals.cur_opt.nPixW; xx++){
         inf = &(globals.image[yy][xx]);
         if(inf->do_highlight){
            if(state == 1){
               gc = GTK_WIDGET(globals.picDA)->style->white_gc;
            } else {
               gc = GTK_WIDGET(globals.picDA)->style->black_gc;
            }
            gdk_draw_rectangle(GTK_WIDGET(globals.picDA)->window,
               gc,
               FALSE,
               xx*globals.cur_opt.pixW, yy*globals.cur_opt.pixH,
	       globals.cur_opt.pixW-1, globals.cur_opt.pixH-1);
         }
      }
   }

   state = !state;

   /* make sure our drawing happens now */
   gdk_flush();

   return TRUE;

}

int highlight_changed(){

   /* redraw the screen */
   redraw_screen(0, 0, globals.cur_opt.width, globals.cur_opt.height);

   /* redraw the highlights */
   highlight_timeout(NULL);

   return 1;
}


int start_highlight_timer(){
   
   /* start a timer if one does not already exist */
   if(!timeout_exists){
      globals.timer = gtk_timeout_add(200, highlight_timeout, NULL);
      timeout_exists = 1;
   }

   return 1;
}

int stop_highlight_timer(){

   if(timeout_exists){
      gtk_timeout_remove(globals.timer);
      timeout_exists = 0;
   }

   /* redraw the screen */
   redraw_screen(0, 0, globals.cur_opt.width, globals.cur_opt.height);

   return 1;
}

