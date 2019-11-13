# compile stuff in this dir

CC = gcc

CFLAGS += -I/usr/X11R6/include `pkg-config --cflags gtk+-3.0`
CFLAGS += -g
CFLAGS += -DGSEAL_ENABLE
CLFLAS += -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED
CFLAGS += -DGTK_DISABLE_SINGLE_INCLUDES
CFLAGS += -DGDK_PIXBUF_DISABLE_DEPRECATED
CFLAGS += -DG_DISABLE_DEPRECATED
CFLAGS += -Wall -Wextra -Werror
DFLAGS = -L/usr/X11R6/lib `pkg-config --libs gtk+-3.0`
DFLAGS += -Wall -Wextra
LIBS = -lm

PIX_OBJS = main.o menu.o options.o \
           display.o read_db.o globals.o \
           file_dialog.o status.o cursor.o \
           render.o render_image.o stats.o find_match.o \
           info_popup.o highlight.o draw_image.o

DB_OBJS = make_db.o

all:	make_db pixelize

make_db:	$(DB_OBJS)
	$(CC) -o $@ $(DB_OBJS) $(DFLAGS) $(LIBS)

pixelize:	$(PIX_OBJS)
	$(CC) -o $@ $(PIX_OBJS) $(DFLAGS) $(LIBS)

.c.o:	depend
	$(CC) -c -o $@ $(CFLAGS) $<

depend:	Makefile pixelize_model.h
	$(CC) -MM $(CFLAGS) *.c > depend

pixelize_model.h: pixelize.glade
	./tools/glade2c -i $< -o $@

clean:
	rm -f *.o pixelize make_db core depend pixelize_model.h

include depend
