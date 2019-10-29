# compile stuff in this dir

CC = gcc

CFLAGS = -g -I/usr/X11R6/include `pkg-config --cflags gtk+-2.0` -Wall
DFLAGS = -L/usr/X11R6/lib `pkg-config --libs gtk+-2.0`  -Wall
LIBS = -lm

OBJS = main.o setup_menu.o callback_menu.o help.o options.o \
       display.o read_db.o globals.o read_line.o \
       file_dialog.o draw_image.o status.o \
       render.o render_image.o stats.o find_match.o \
       info_popup.o highlight.o cursor.o

all:	make_db pixelize

merge:	merge.o read_line.o stats.o
	$(CC) -o $@ merge.o read_line.o stats.o $(DFLAGS) $(LIBS)

make_db:	make_db.o
	$(CC) -o $@ make_db.o $(DFLAGS) $(LIBS)

#	$(CC) -o $@ make_db.o `pkg-config --libs imlib2` $(LIBS)

pixelize:	$(OBJS)
	$(CC) -o $@ $(OBJS) $(DFLAGS) $(LIBS)

.c.o:
	$(CC) -c -o $@ $(CFLAGS) $<

depend:
	$(CC) -MM $(CFLAGS) *.c > depend

clean:
	rm -f *.o pixelize make_db core

include depend
