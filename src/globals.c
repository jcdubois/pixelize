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

#include "globals.h"

struct _Globals globals = {.topwin = NULL,
                           .draw_area = NULL,
                           .head = NULL,
                           .image = NULL,
                           .max_order = 0,
                           .do_highlight = 0,
                           .show_rendered = TRUE,
                           .command_mode = FALSE,
                           .in_fname = NULL,
                           .in_im = NULL,
                           .in_im_scaled = NULL,
                           .out_fname = NULL,
                           .out_im = NULL,
                           .cur_opt.pixW = 10,
                           .cur_opt.pixH = 10,
                           .cur_opt.nPixW = 30,
                           .cur_opt.nPixH = 30,
                           .cur_opt.proximity = 3,
                           .cur_opt.opt_alg = PIX_SIZE,
                           .new_opt.pixW = 10,
                           .new_opt.pixH = 10,
                           .new_opt.nPixW = 30,
                           .new_opt.nPixH = 30,
                           .new_opt.proximity = 3,
                           .new_opt.opt_alg = PIX_SIZE};
