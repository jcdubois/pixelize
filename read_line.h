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

/*
 * read_line.h  by Paul Wilkins  11/20/96
 */

#ifndef __READ_LINE_H
#define __READ_LINE_H

/*
 * Reads a line from the specified file into the specified string.
 * Returns 1 on success and EOF if end of file was reached.
 */
int read_line(FILE *, char *);

#endif
