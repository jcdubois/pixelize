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
      free(db->fname);
    }
    if (db->data) {
      int i;
      for (i = 0; i < n; i++) {
        if (db->data[i]) {
          free(db->data[i]);
        }
      }
      free(db->data);
    }
    free(db);
  }
}

static struct PIC_DB *malloc_db(int strlen, int n) {

  struct PIC_DB *db = malloc(sizeof(struct PIC_DB));

  if (db) {
    db->fname = malloc((strlen + 1) * sizeof(char));

    if (db->fname) {
      db->data = malloc(n * sizeof(int *));

      if (db->data) {
        int i;
        for (i = 0; i < n; i++) {
          db->data[i] = NULL;
        }
        for (i = 0; i < n; i++) {
          db->data[i] = malloc(3 * (i + 1) * (i + 1) * sizeof(int));

          if (db->data[i] == NULL) {
            perror("malloc");
            free_db(db, n);
            db = NULL;
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
      guint ndb = 0;
      struct PIC_DB *db = NULL;

      /* read in the db */
      while (fscanf(dbfp, "%s\n", line) == 1) {
        guint i, j, len;

        len = strlen(line);

        db = malloc_db(len, *max_order);

        if (db) {
          strcpy(db->fname, line);

          for (j = 0; j < *max_order; j++) {
            guint *p1 = db->data[j];

            for (i = 0; i < (j + 1) * (j + 1); i++) {
              if (fscanf(dbfp, "%u %u %u\n", &p1[0], &p1[1], &p1[2]) != 3) {
                fprintf(stderr, "Error: can't read data from pic_db.dat\n");
                exit(1);
              }
              p1 += 3;
            }
          }

          /* add db to the list */
          db->next = head;
          /* new db head */
          head = db;
          ndb++;
        }
      }
    } else {
      fprintf(stderr, "Error: can't read size from pic_db.dat\n");
    }

    fclose(dbfp);
  } else {
    fprintf(stderr, "Error opening pic_db.dat for read\n");
  }

  return head;
}
