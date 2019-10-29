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
/* globals.c by Paul Wilkins 1/2/2000 */

#include <gtk/gtk.h>
#include <math.h>
#include <stdio.h>
/* #include <gdk_imlib.h> */

#include "globals.h"
#include "read_db.h"

struct GLOBALS globals;

int init_globals() {
  globals.topwin = NULL;
  globals.ebox = NULL;
  globals.disp_w = 0;
  globals.disp_h = 0;
  globals.picDA = NULL;
  globals.picScroll = NULL;
  globals.head = NULL;
  globals.image = NULL;
  globals.max_order = 0;
  globals.do_highlight = 0;
  globals.show_rendered = 1;
  globals.in_fname = NULL;
  globals.in_im = NULL;
  globals.in_im_scaled = NULL;
  // globals.pixmap = NULL;
  globals.out_fname = NULL;
  globals.out_im = NULL;
  globals.start_fname = NULL;

  globals.cur_opt.pixW = 25;
  globals.cur_opt.pixH = 25;
  globals.cur_opt.nPixW = 30;
  globals.cur_opt.nPixH = 30;
  globals.cur_opt.proximity = 8;
  globals.cur_opt.opt_alg = PIX_SIZE;

  globals.new_opt.pixW = 25;
  globals.new_opt.pixH = 25;
  globals.new_opt.nPixW = 30;
  globals.new_opt.nPixH = 30;
  globals.new_opt.proximity = 8;
  globals.new_opt.opt_alg = PIX_SIZE;

  return 1;
}
