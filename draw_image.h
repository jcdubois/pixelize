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

/* draw_image.h by Paul Wilkins 1/2/2000 */

#ifndef __DRAW_IMAGE_H
#define __DRAW_IMAGE_H

int copy_opt_data();
int calc_dimensions(struct IMAGE_OPTIONS *im_opt);
void render_image_pixmap(GdkPixbuf *);
int draw_big_image(GdkPixbuf *im);
void redraw_screen(int x, int y, int w, int h);
int draw_small_image(GdkPixbuf *scale_im, int x, int y);
int change_small_image(int xx, int yy);

#endif
