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

#include "stats.h"
#include <math.h>

gboolean calc_mead_std(const double *data, int n, double *mean, double *std) {
  if (data && mean && std) {
    if (n == 0) {
      *std = 0.0;
      *mean = 0.0;
    } else if (n == 1) {
      *std = 0.0;
      *mean = data[0];
    } else {
      int i;
      double sd = 0.0;
      double sum = 0.0;

      /* find the mean */
      for (i = 0; i < n; i++) {
        sum += data[i];
      }
      *mean = sum / (double)n;

      /* find the std */
      for (i = 0; i < n; i++) {
        sd += (data[i] - *mean) * (data[i] - *mean);
      }
      *std = sqrt(sd / (double)(n - 1));
    }

    return TRUE;
  }
  return FALSE;
}
