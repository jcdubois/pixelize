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
/* stats.c by Paul Wilkins 1/2/2000 */

#include <stdio.h>
#include <math.h>

#include "stats.h"

int calc_mead_std(double *data, int n, double *mean, double *std){
   int i;
   double sum;
   double sd;

   if(n == 0){

      *std = 0.0;
      *mean = 0.0;

   } else if(n == 1){

      *std = 0.0;
      *mean = data[0];

   } else {

      /* find the mean */
      sum = 0.0;
      for(i=0; i<n; i++){
	 sum += data[i];
      }
      *mean = sum / (double)n;

      /* find the std */
      sd = 0.0;
      for(i=0; i<n; i++){
	 sd += (data[i] - *mean) * (data[i] - *mean);
      }
      *std = sqrt(sd / (double)(n - 1));
   }

   return 1;
}
