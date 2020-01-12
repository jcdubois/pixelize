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
/* read_db.c by Paul Wilkins 1/2/2000 */

#include "read_db.h"

static void free_db(struct PIC_DB *db, int n) {
  if (db) {
    if (db->fname) {
      g_free(db->fname);
    }
    if (db->data) {
      int i;
      for (i = 0; i < n; i++) {
        if (db->data[i]) {
          g_free(db->data[i]);
        }
      }
      g_free(db->data);
    }
    g_free(db);
  }
}

static struct PIC_DB *malloc_db(int strlen, int n) {

  struct PIC_DB *db = g_malloc0(sizeof(struct PIC_DB));

  if (db) {
    db->data = NULL;
    db->fname = g_malloc0((strlen + 1) * sizeof(char));

    if (db->fname) {
      db->data = g_malloc0(n * sizeof(guint *));

      if (db->data) {
        int i;
        for (i = 0; i < n; i++) {
          db->data[i] = NULL;
        }
        for (i = 0; i < n; i++) {
          db->data[i] = g_malloc0(3 * (i + 1) * (i + 1) * sizeof(guint));

          if (db->data[i] == NULL) {
            perror("malloc");
            free_db(db, n);
            db = NULL;
            break;
          }
        }
      } else {
        perror("malloc");
        free_db(db, n);
        db = NULL;
      }
    } else {
      perror("malloc");
      free_db(db, n);
      db = NULL;
    }
  } else {
    perror("malloc");
  }
  return db;
}

void reset_db_data(struct PIC_DB *head) {
  struct PIC_DB *db;

  for (db = head; db != NULL; db = db->next) {
    db->refcnt = 0;
    db->done = FALSE;
  }
}

struct PIC_DB *read_database(guint *max_order) {
  FILE *dbfp = NULL;
  struct PIC_DB *head = NULL;

  dbfp = fopen("pic_db.dat", "r");

  if (dbfp) {
    char line[1024];

    if (fscanf(dbfp, "%u\n", max_order) == 1) {

      /* read in the db */
      while (fscanf(dbfp, "%1023s\n", line) == 1) {
        guint len = strlen(line);
        struct PIC_DB *db = malloc_db(len, *max_order);

        if (db) {
          guint i;
          guint j;
          strcpy(db->fname, line);

          for (j = 0; j < *max_order; j++) {
            guint *p1 = db->data[j];

            for (i = 0; i < (j + 1) * (j + 1); i++) {
              if (fscanf(dbfp, "%3u %3u %3u\n", &p1[0], &p1[1], &p1[2]) != 3) {
                g_printerr(
                    "Error: can't read data from pic_db.dat for file %s\n",
                    line);
                g_printerr("Error: is data base file corrupted ?\n");
                exit(1);
              }
              p1 += 3;
            }
          }

          /* check the referenced file is present */
          if (access(line, R_OK) != 0) {
            g_printerr("Error: file %s does not exist\n", line);
            g_printerr("Error: ignoring file %s\n", line);

            /* we free the db and we continue to the nexte file */
            free_db(db, *max_order);
          } else {
            /* add db to the list */
            db->next = head;

            /* new db head */
            head = db;
          }
        }
      }
    } else {
      g_printerr("Error: can't read size from pic_db.dat\n");
    }

    fclose(dbfp);
  } else {
    g_printerr("Error opening pic_db.dat for read\n");
    g_printerr("Use the make_db(1) tool before running pixelize!\n");
  }

  return head;
}
