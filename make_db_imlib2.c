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
/* make_db.c by Paul Wilkins 1/2/2000 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <Imlib2.h>

struct PIX {
   int cnt;
   double r;
   double g;
   double b;
};

#define MAX_SIZE 5
#define MAX_PATH_LEN 512

int
main(int argc, char **argv)
{
   int write_db = 1;
   int i, j, n;
   int size;
   Display *disp;
   Imlib_Image *im;
   int qh, qw;
   int width, height;
   int ww, hh;
   DATA32 pixel;
   DATA32 *p1;
   struct PIX ***quad;
   char my_cwd[MAX_PATH_LEN+1];
   FILE *dbfp;

   /* Be nice and tell the user if they don't, to provide a file as an arg */
   if (argc <= 1) {
      fprintf(stderr, "make_db is a program that scanns pictures and creates a file\n");
      fprintf(stderr, "called pic_db.dat. The file pic_db.dat is used by Pixelize\n");
      fprintf(stderr, "when \"rendering\" images.\n");
      fprintf(stderr, "\n");
      fprintf(stderr, "Usage:\n %s image_files\n", argv[0]);
      exit(1);
   }

   if(NULL == (quad=malloc((MAX_SIZE+1)*sizeof(struct PIX *)))){
      perror("malloc");
      exit(1);
   }
   for(i=1; i<=MAX_SIZE; i++){
      if(NULL == (quad[i]=malloc((MAX_SIZE+1)*sizeof(struct PIX *)))){
         perror("malloc");
         exit(1);
      }
      for(j=0; j<=MAX_SIZE; j++){
         if(NULL == (quad[i][j]=malloc((MAX_SIZE+1)*sizeof(struct PIX)))){
            perror("malloc");
            exit(1);
         }
      }
   }

   /* Connect to the default Xserver */
   disp = XOpenDisplay(NULL);

   /* Immediately afterwards Intitialise Imlib */
/*   id = Imlib_init(disp); */
   imlib_set_color_usage(128);
   imlib_context_set_dither(1);
   imlib_context_set_display(disp);
   imlib_context_set_visual(DefaultVisual(disp, DefaultScreen(disp)));
   imlib_context_set_colormap(DefaultColormap(disp, DefaultScreen(disp)));

   if(write_db){
      if(NULL == (dbfp=fopen("pic_db.dat", "a+"))){
         printf("Error opening pic_db.dat for write\n");
         exit(1);
      }
      fprintf(dbfp, "%d\n", MAX_SIZE);
   }

   /* get the cwd.  we may use it later */
   if(NULL == (getcwd(my_cwd, MAX_PATH_LEN))){
      fprintf(stderr, "Warning: Can't determine Current working directory.\n");
      fprintf(stderr, "         This may generate an invalid pic_db.dat file.\n");
      my_cwd[0] = '\0';
   }

   /* for each file */
   for(n=1; n<argc; n++){

      
      /* Load the image specified as the first argument */
      if(NULL == (im = imlib_load_image_immediately(argv[n]))){
         fprintf(stderr, "Error: Can't load image %s\n", argv[n]);
         continue;
      }
      imlib_context_set_image(im);

      /* Suck the image's original width and height out of the Image structure */
      width = imlib_image_get_width();
      height = imlib_image_get_height();

      printf("file %d: %s width: %d height: %d\n", n, argv[n], width, height);

      if(write_db){
         if(argv[n][0] != '/'){
            fprintf(dbfp, "%s/%s\n", my_cwd, argv[n]);
         } else {
            fprintf(dbfp, "%s\n", argv[n]);
         }
      }

      for(size=1; size<=MAX_SIZE; size++){ 
         for(hh=0; hh<size; hh++){
            for(ww=0; ww<size; ww++){
               quad[size][ww][hh].cnt = 0;
               quad[size][ww][hh].r = 0.0;
               quad[size][ww][hh].g = 0.0;
               quad[size][ww][hh].b = 0.0;
            }
         }
      }

      /* generate the data for each size */
      for(size=3; size<=MAX_SIZE; size++){ 

         p1 = imlib_image_get_data_for_reading_only();

         for(hh=0; hh<height; hh++){

            qh = (int)((double)hh / (double)height * (double)size);
            if(qh < 0) qh = 0;
            if(qh >= size) qh = size - 1;

            for(ww=0; ww<width; ww++){

               qw = (int)((double)ww / (double)width * (double)size);
               if(qw < 0) qw = 0;
               if(qw >= size) qw = size - 1;
               
               pixel = *p1;

               quad[size][qw][qh].cnt++;
               quad[size][qw][qh].r += (double)((pixel>>16)&0xff);
               quad[size][qw][qh].g += (double)((pixel>> 8)&0xff);
               quad[size][qw][qh].b += (double)((pixel    )&0xff);

               p1++;
            }
         }
      }

      /* special case for size = 2 */
      for(hh=0; hh<2; hh++){
         for(ww=0; ww<2; ww++){

            for(i=0; i<2; i++){
               for(j=0; j<2; j++){
                  quad[2][ww][hh].cnt += quad[4][ww*2+i][hh*2+j].cnt;
                  quad[2][ww][hh].r += quad[4][ww*2+i][hh*2+j].r;
                  quad[2][ww][hh].g += quad[4][ww*2+i][hh*2+j].g;
                  quad[2][ww][hh].b += quad[4][ww*2+i][hh*2+j].b;
               }
            }
         }
      }

      /* special case for size = 1 */
      for(i=0; i<2; i++){
         for(j=0; j<2; j++){
            quad[1][0][0].cnt += quad[2][i][j].cnt;
            quad[1][0][0].r += quad[2][i][j].r;
            quad[1][0][0].g += quad[2][i][j].g;
            quad[1][0][0].b += quad[2][i][j].b;
         }
      }

      for(size=1; size<=MAX_SIZE; size++){
         if(write_db){
            for(hh=0; hh<size; hh++){
               for(ww=0; ww<size; ww++){
                  fprintf(dbfp, "%03d %03d %03d\n",
                     (int)(quad[size][ww][hh].r / quad[size][ww][hh].cnt),
                     (int)(quad[size][ww][hh].g / quad[size][ww][hh].cnt),
                     (int)(quad[size][ww][hh].b / quad[size][ww][hh].cnt));
               }
            }
         }

      }
/* BDD - FIXME */
/*      Imlib_kill_image(id, im); */
   }
   if(write_db) fclose(dbfp);

   return 0;
}
