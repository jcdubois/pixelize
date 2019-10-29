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

/* render_image.h by Paul Wilkins 1/2/2000 */

#ifndef __RENDER_IMAGE_H
#define __RENDER_IMAGE_H

struct IMAGE_INFO {
   int nMatch;                 /* number of matches in matches */
   int match_no;               /* which match in matches is db */
   int do_highlight;           /* should we highlight this image somehow */
   struct PIC_DB *db;          /* the image we are actually using */
   struct PIC_DB **matches;    /* all the other possible matches */
};

GdkPixbuf *render_image(struct IMAGE_INFO **image, int nPixW, int nPixH, int pixW, int pixH);
void copy_image_RGB(
   GdkPixbuf *src,
   GdkPixbuf *dst,
   int col,
   int row);


#endif

