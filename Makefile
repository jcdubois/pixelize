# compile stuff in this dir

CC = gcc

CFLAGS = -DGSEAL_ENABLE -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGTK_DISABLE_SINGLE_INCLUDES -g -I/usr/X11R6/include `pkg-config --cflags gtk+-2.0` -Wall -Wextra -Werror
DFLAGS = -L/usr/X11R6/lib `pkg-config --libs gtk+-2.0`  -Wall -Wextra
LIBS = -lm

OBJS = main.o setup_menu.o callback_menu.o help.o options.o \
       display.o read_db.o globals.o read_line.o \
       file_dialog.o status.o cursor.o \
       render.o render_image.o stats.o find_match.o \
       info_popup.o highlight.o draw_image.o

all:	make_db pixelize

make_db:	make_db.o
	$(CC) -o $@ make_db.o $(DFLAGS) $(LIBS)

pixelize:	$(OBJS)
	$(CC) -o $@ $(OBJS) $(DFLAGS) $(LIBS)

.c.o:	depend
	$(CC) -c -o $@ $(CFLAGS) $<

depend:
	$(CC) -MM $(CFLAGS) *.c > depend

clean:
	rm -f *.o pixelize make_db core depend

include depend
