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

/* read_db.h by paul wilkins 1/2/2000 */

#ifndef __READ_PIC_DB_H
#define __READ_PIC_DB_H

#include <gtk/gtk.h>

struct PIC_DB {
  gboolean done;
  guint refcnt;
  char *fname;
  guint **data;
  struct PIC_DB *next;
};

struct PIC_DB *read_database(guint *max_order);

struct PIC_DB *malloc_db(int strlen, int max_order);

void free_db(struct PIC_DB *db, int max_order);

void reset_db_data(struct PIC_DB *head);

#endif
