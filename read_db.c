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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "read_line.h"
#include "read_db.h"

struct PIC_DB *malloc_db(int strlen, int n){
   int i;
   struct PIC_DB *db;
   if(NULL == (db=malloc(sizeof(struct PIC_DB)))){
      perror("malloc");
      exit(1);
   }
   if(NULL == (db->fname=malloc((strlen+1)*sizeof(char)))){
      perror("malloc");
      exit(1);
   }
   if(NULL == (db->data=malloc(n*sizeof(int *)))){
      perror("malloc");
      exit(1);
   }
   for(i=0; i<n; i++){
      if(NULL == (db->data[i]=malloc(3*(i+1)*(i+1)*sizeof(int)))){
	 perror("malloc");
	 exit(1);
      }
   }
   return db;
}


void reset_db_data(struct PIC_DB *head){
   struct PIC_DB *db;

   for(db=head; db!=NULL; db=db->next){
      db->refcnt = 0;
      db->done = 0;
   }
}


struct PIC_DB *read_database(int *max_order){
   int i, j, done;
   int len;
   int ndb;
   int size;
   int ret;
   FILE *dbfp;
   int *p1;
   char line[1024];
   struct PIC_DB *head, *db;

   if(NULL == (dbfp=fopen("pic_db.dat", "r"))){
      fprintf(stderr, "Error opening pic_db.dat for read\n");
      exit(1);
   }
   ret = read_line(dbfp, line);
   if(ret == EOF) {
      fprintf(stderr, "Error: can't read first line from pic_db.dat\n");
      exit(1);
   }
   if(1 != sscanf(line, "%d", &size)){
      fprintf(stderr, "Error: can't read size from pic_db.dat\n");
      exit(1);
   }
   *max_order = size;

   ndb = 0;
   head = NULL;

   /* read in the db */
   done = 0;
   do {
      ret = read_line(dbfp, line);
      if(*line == '\0') continue;
      if(*line == '#') continue;
      if(*line == '5') continue;

      len = strlen(line);

      db = malloc_db(len, size);

      strcpy(db->fname, line);
      /* printf("filename: %s\n", db->fname);  */

      for(j=0; j<size; j++){

	 p1 = db->data[j];

	 for(i=0; i<3*(j+1)*(j+1); i++){
	    if(1 != fscanf(dbfp, "%d", p1)){
	       fprintf(stderr, "Error: File %d: %s: reading value %d from pic_db.dat\n", 
		  ndb, db->fname, i);
	       exit(1);
	    }
	    /* printf("Read value no: %d: %d\n", i, *p1); */
	    p1++;
	 }
      }

      /* add db to the list */
      db->next = head;
      head = db;
      ndb++;
      
   } while (ret != EOF && !done);

   fclose(dbfp);

   return head;
}
