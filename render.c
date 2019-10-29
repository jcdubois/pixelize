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
/* render.c by Paul Wilkins 1/2/2000 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gtk/gtk.h>
/* #include <gdk_imlib.h> */

#include "globals.h"
#include "read_db.h"
#include "display.h"
#include "stats.h"
#include "status.h"
#include "find_match.h"
#include "draw_image.h"
#include "cursor.h"
#include "render_image.h"
#include "render.h"

#define STATS_SIZE 6

struct PIX {
   int cnt;
   double r;
   double g;
   double b;
};

void average_image_area(struct PIX *avg, GdkPixbuf *im, int x, int y, int w, int h){
   int i, j;
   int r, g, b;
   int xx, yy;
   guchar *pixels, *p;
   int rowstride, n_channels;;

   /* printf("average_image_area: x: %d y: %d w: %d h: %d\n", x, y, w, h);  */

	n_channels = gdk_pixbuf_get_n_channels(im);
	rowstride = gdk_pixbuf_get_rowstride(im);
	pixels = gdk_pixbuf_get_pixels(im);


   avg->r = 0.0;
   avg->g = 0.0;
   avg->b = 0.0;
   for(j=0; j<h; j++){
      yy = j + y;
      for(i=0; i<w; i++){
         xx = i + x;

			p = pixels + yy * rowstride + xx * n_channels;

         r = p[0];
         g = p[1];
         b = p[2];

         /* printf("average_image_area: pixel %dx%d r: %d g: %d b: %d\n", xx, yy, r, g, b); */

         avg->r += r;
         avg->g += g;
         avg->b += b;

      }
   }
   avg->cnt = w * h;
   avg->r /= (double)(w*h);
   avg->g /= (double)(w*h);
   avg->b /= (double)(w*h);

   /* printf("average_image_area: r: %g g: %g b: %g\n", avg->r, avg->g, avg->b);  */
}



double calc_stddev(GdkPixbuf *im, int x, int y, int nPixW, int nPixH){
   int w, h, n;
   int xx, yy, ww, hh;
   int pxx, pyy;
   double xoff, yoff;
   double meanR, stdR;
   double meanG, stdG;
   double meanB, stdB;
   struct PIX avg;
   double dataR[STATS_SIZE*STATS_SIZE];
   double dataG[STATS_SIZE*STATS_SIZE];
   double dataB[STATS_SIZE*STATS_SIZE];

   /* printf("calc_stddev: x: %d y: %d\n", x, y); */

   xoff = (double)x / (double)nPixW * (double)gdk_pixbuf_get_width(im);
   yoff = (double)y / (double)nPixH * (double)gdk_pixbuf_get_height(im);

   n = 0;
   pyy = (int)yoff;
   for(h=0; h<STATS_SIZE; h++){

      yy = (int)(yoff + (double)(h+1) * (double)gdk_pixbuf_get_height(im) / (double)nPixH / (double)STATS_SIZE);
      hh = yy - pyy;
      pxx = (int)xoff;

      for(w=0; w<STATS_SIZE; w++){

         xx = (int)(xoff + (double)(w+1) * (double)gdk_pixbuf_get_width(im) / (double)nPixW / (double)STATS_SIZE);
         ww = xx - pxx;

         average_image_area(&avg, im, pxx, pyy, ww, hh);
         dataR[n] = avg.r;
         dataG[n] = avg.g;
         dataB[n] = avg.b;
         n++;

         pxx = xx;
      }
      pyy = yy;
   }

   calc_mead_std(dataR, n, &meanR, &stdR);
   calc_mead_std(dataG, n, &meanG, &stdG);
   calc_mead_std(dataB, n, &meanB, &stdB);

   return (stdR + stdG + stdB) / 3.0;
}

int init_db(){
   static int init = 0;

   if(init == 0){
      /* read in the picture database */
      if(NULL == (globals.head = read_database(&(globals.max_order)))){
         fprintf(stderr, "Error reading database.\n");
         exit(1);
      }
      init = 1;
   }
   return 1;
}

int *gen_master_data(GdkPixbuf *im, int x, int y, int nPixW, int nPixH, int order){
   int w, h;
   int xx, yy, ww, hh;
   int pxx, pyy;
   int *p1;
   int *data;
   double xoff, yoff;
   struct PIX avg;

   /* this must be free'd outside this function */
   if(NULL == (data=malloc(3*order*order*sizeof(int)))){
      perror("malloc");
      exit(1);
   }

   xoff = (double)x / (double)nPixW * (double)gdk_pixbuf_get_width(im);
   yoff = (double)y / (double)nPixH * (double)gdk_pixbuf_get_height(im);

   pyy = (int)yoff;
   p1 = data;
   for(h=0; h<order; h++){

      yy = (int)(yoff + (double)(h+1) * (double)gdk_pixbuf_get_height(im) / (double)nPixH / (double)order);
      hh = yy - pyy;
      pxx = (int)xoff;

      for(w=0; w<order; w++){

         xx = (int)(xoff + (double)(w+1) * (double)gdk_pixbuf_get_width(im) / (double)nPixW / (double)order);
         ww = xx - pxx;

         average_image_area(&avg, im, pxx, pyy, ww, hh);
         *p1++ = (int)avg.r;
         *p1++ = (int)avg.g;
         *p1++ = (int)avg.b;

         pxx = xx;
      }
      pyy = yy;
   }

   return data;
}

void free_image_data(){
   int ww, hh;

   if(globals.image == NULL) return;

   /* free old image data */
   for(hh=0; hh<globals.cur_opt.nPixH; hh++){
      for(ww=0; ww<globals.cur_opt.nPixW; ww++){
         free(globals.image[hh][ww].matches);
      }
      free(globals.image[hh]);
   }
   free(globals.image);

   globals.image = NULL;
}

int image_borders_n(struct IMAGE_INFO **image, struct PIC_DB *match, int n, int cx, int cy, int maxx, int maxy){
   int x, y;
   for(y=cy-n; y<=cy+n; y++){
      if(y<0 || y>=maxy) continue;
      for(x=cx-n; x<=cx+n; x++){
         if(x<0 || x>=maxx) continue;
         if(x==cx && y==cy) continue;
         if(image[y][x].db == match) return 1;
      }
   }
   return 0;
}



int guess_order(double val, int pixW, int pixH, int max_order){
   int order;
   order = (int)(sqrt((double)(pixW*pixH))/9.0 + (val / 6.0));
   if(order > max_order) order = max_order;
   return order;
}


int render(){
   int i, j;
   int pixW, pixH;
   int nPixW, nPixH;
   int *match_data;
   int proximity;
   double percent;
   GdkPixbuf *im;
   GdkPixbuf *out_im;
   int order;
   int ww, hh;
   double stddev;
   struct IMAGE_INFO **image;
   struct PIC_DB **matches;

   if(globals.in_fname == NULL || globals.in_im_scaled == NULL){
      fprintf(stderr, "Error: Open an Image first!\n");
      return 0;
   }

   /* free the old data, if any */
   free_image_data();

   /* calculate the new images dimensions */
   if(0 == calc_dimensions(&(globals.new_opt))){
      fprintf(stderr, "Invalid option.  Bye.\n");
      return 0;
   }

   /* set the cursor */
   cursor_busy();

   /* copy the image rendering data from new_opt to cur_opt */
   if(copy_opt_data()){
      resize_window();

		if(globals.in_im_scaled != NULL) g_object_unref(globals.in_im_scaled);

		/* scale the image */
		if(NULL == (globals.in_im_scaled=gdk_pixbuf_scale_simple(
			globals.in_im,
			globals.cur_opt.pixW * globals.cur_opt.nPixW,
			globals.cur_opt.pixH * globals.cur_opt.nPixH,
			GDK_INTERP_BILINEAR
		))){
			fprintf(stderr, "Error: Unable to scale image: %s\n", globals.in_fname);
			return 0;
		}
   }

   /* update the mode display info */
   refresh_mode_display();

   /* set our local copies of some of the globals */
   im = globals.in_im_scaled;
   pixW = globals.cur_opt.pixW;
   pixH = globals.cur_opt.pixH;
   nPixW = globals.cur_opt.nPixW;
   nPixH = globals.cur_opt.nPixH;

   /* malloc the array to store the image info */
   if(NULL == (image=malloc(nPixH*sizeof(struct IMAGE_INFO *)))){
      perror("Malloc");
      exit(1);
   }
   for(hh=0; hh<nPixH; hh++){
      if(NULL == (image[hh]=malloc(nPixW*sizeof(struct IMAGE_INFO)))){
         perror("Malloc");
         exit(1);
      }
      for(ww=0; ww<nPixW; ww++){
         image[hh][ww].nMatch = 0;
         image[hh][ww].db = NULL;
         image[hh][ww].do_highlight = 0;
      }
   }
   globals.image = image;

   /* initialize the database */
   init_db();

   /* clear data stored in the database from the last render */
   reset_db_data(globals.head);

   /* turn on the progress bar */
   toggle_progress_indicator(ST_PERCENT);
   set_progress_indicator(0.0);

   globals.show_rendered = 1;

   /* go pix by pix through the image calculating images that match well */
   for(hh=0; hh<nPixH; hh++){
      for(ww=0; ww<nPixW; ww++){

         /* calc_stats */
         stddev = calc_stddev(im, ww, hh, nPixW, nPixH);

         /* depending on stddev, match to different orders */
         order = guess_order(stddev, pixW, pixH, globals.max_order);

         /* generate the data to match to from the master image */
         match_data = gen_master_data(im, ww, hh, nPixW, nPixH, order);

         /* find the best matching picture for this "pixel" */
         matches = find_match(order, match_data, globals.head);

         /* we are done with it now */
         free(match_data);

         /* store the image we picked */
         image[hh][ww].matches = matches;

         /* update the progress bar */
         percent = (double)(hh*nPixW + ww + 1) / (double)(nPixH*nPixW);
			set_progress_indicator(percent);
      }
   }

   /* go pix by pix through the image finding the best match */
   for(j=0; j<nPixH*nPixW; j++){

      /* randomly pick an image to match */
      hh = random() % nPixH;
      ww = random() % nPixW;
      while(image[hh][ww].db != NULL){
         ww++;
         if(ww >= nPixW){ 
            ww = 0;
            hh++;
            if(hh >= nPixH) hh = 0;
         }
      }

      matches = image[hh][ww].matches;

      /* pick one of the (hopefully the best) matches to fill this spot */
      proximity = globals.cur_opt.proximity;
      do {
			for(i=0;
				i<MAX_MATCHES && 1==image_borders_n(image, matches[i], proximity, ww, hh, nPixW, nPixH);
				i++) /* do nothing */;
			proximity--;
      } while (proximity>=0 && i == MAX_MATCHES);

      /* probably only one file */
      if(matches[i] == NULL) i = 0;

      /* store the image we picked */
      image[hh][ww].match_no = i;
      image[hh][ww].db = matches[i];
      image[hh][ww].db->refcnt++;
      image[hh][ww].db->done = 0;

   }

   /* render the image */
   if(NULL == (out_im = render_image(image, nPixW, nPixH, pixW, pixH))){
      fprintf(stderr, "Error: Can't render image.\n");
      exit(1);
   }
   globals.out_im = out_im;

   /* display it */
   // draw_big_image(out_im);
   redraw_screen(0, 0, globals.cur_opt.width, globals.cur_opt.height);

   /* turn off the progress bar */
   toggle_progress_indicator(ST_READY);

   /* set the cursor */
   cursor_normal();

   return 1;
}
