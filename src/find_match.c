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

#include "find_match.h"
#include <math.h>

struct PIC_DB **find_match(guint order, guint *match_data,
                           struct PIC_DB *head) {
  double best_fit[MAX_MATCHES];

  struct PIC_DB **matches = g_malloc0(MAX_MATCHES * sizeof(struct PIC_DB *));

  if (matches) {
    guint i;
    struct PIC_DB *db;

    for (i = 0; i < MAX_MATCHES; i++) {
      matches[i] = NULL;
      best_fit[i] = 1e300;
    }

    /* loop through each pic in the db */
    for (db = head; db != NULL; db = db->next) {
      guint *p1 = db->data[order - 1];
      guint *p2 = match_data;
      double fit = 0.0;

      for (i = 0; i < order * order; i++) {
        guint r1 = *p1++;
        guint r2 = *p2++;
        guint g1 = *p1++;
        guint g2 = *p2++;
        guint b1 = *p1++;
        guint b2 = *p2++;
        guint r = r1 - r2;
        guint g = g1 - g2;
        guint b = b1 - b2;

        /* it looks like this emphasizes the colors more.
           But i don't understand why?  Why not multiply?
           if(r1 > g1 && r1 > b1) r /= 2.0;
           else if(g1 > r1 && g1 > b1) g /= 2.0;
           else if(b1 > r1 && b1 > g1) b /= 2.0;
         */

        fit += sqrt(r * r + g * g + b * b);
      }

      for (i = 0; i < MAX_MATCHES; i++) {
        if (fit < best_fit[i]) {
          guint j;
          for (j = MAX_MATCHES - 1; j > i; j--) {
            best_fit[j] = best_fit[j - 1];
            matches[j] = matches[j - 1];
          }
          best_fit[i] = fit;
          matches[i] = db;
          break;
        }
      }
    }
  } else {
    perror("malloc");
  }

  return matches;
}
