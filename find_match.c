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
/* find_match.c by Paul Wilkins 1/2/2000 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "read_db.h"
#include "find_match.h"


struct PIC_DB **find_match(int order, int *match_data, struct PIC_DB *head){
   int i, j;
   int r, g, b;
   int r1, g1, b1;
   int r2, g2, b2;
   int *p1, *p2;
   double fit;
   double best_fit[MAX_MATCHES];
   struct PIC_DB **matches;
   struct PIC_DB *db;

   if(NULL == (matches=malloc(MAX_MATCHES*sizeof(struct PIC_DB *)))){
      perror("malloc");
      exit(1);
   }

   for(i=0; i<MAX_MATCHES; i++){
      matches[i] = NULL;
      best_fit[i] = 1e300;
   }

   /* loop through each pic in the db */
   for(db=head; db!=NULL; db=db->next){

      /* printf("comparing against: %s\n", db->fname); */

      p1 = db->data[order-1];
      p2 = match_data;

      fit = 0.0;
      for(i=0; i<order*order; i++){
         r1 = *p1++; r2 = *p2++;
         g1 = *p1++; g2 = *p2++;
         b1 = *p1++; b2 = *p2++;
         r = r1 - r2;
         g = g1 - g2;
         b = b1 - b2;

         /* it looks like this emphasizes the colors more.
            But i don't understand why?  Why not multiply?
         if(r1 > g1 && r1 > b1) r /= 2.0;
         else if(g1 > r1 && g1 > b1) g /= 2.0;
         else if(b1 > r1 && b1 > g1) b /= 2.0;
         */

         fit += sqrt(r*r + g*g + b*b);
      }

      for(i=0; i<MAX_MATCHES; i++){
	 if(fit < best_fit[i]){
            for(j=MAX_MATCHES-1; j>i; j--){
               best_fit[j] = best_fit[j-1];
               matches[j] = matches[j-1];
            }
	    best_fit[i] = fit;
            matches[i] = db;
            break;
	 }
      }
   }
   return matches;
}

