# compile stuff in this dir

CC = gcc

CFLAGS += -Iinclude
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

SRC_FILES := $(wildcard src/*.c)

PIX_OBJS += obj/main.o
PIX_OBJS += obj/menu.o
PIX_OBJS += obj/options.o
PIX_OBJS += obj/display.o
PIX_OBJS += obj/read_db.o
PIX_OBJS += obj/globals.o
PIX_OBJS += obj/file_dialog.o
PIX_OBJS += obj/status.o
PIX_OBJS += obj/cursor.o
PIX_OBJS += obj/render.o
PIX_OBJS += obj/render_image.o
PIX_OBJS += obj/stats.o
PIX_OBJS += obj/find_match.o
PIX_OBJS += obj/info_popup.o
PIX_OBJS += obj/highlight.o
PIX_OBJS += obj/draw_image.o

DB_OBJS += obj/make_db.o

all:	obj make_db pixelize

make_db:	$(DB_OBJS)
	$(CC) -o $@ $(DB_OBJS) $(DFLAGS) $(LIBS)

pixelize:	$(PIX_OBJS)
	$(CC) -o $@ $(PIX_OBJS) $(DFLAGS) $(LIBS)

obj/%.o: depend
obj/%.o: src/%.c
	$(CC) -c -o $@ $(CFLAGS) $<

obj:
	mkdir $@

depend:	Makefile include/pixelize_model.h
	$(CC) -MM $(CFLAGS) $(SRC_FILES)  > depend

include/pixelize_model.h: glade/pixelize.glade
	./tools/glade2c -i $< -o $@

clean:
	rm -rf obj pixelize make_db core depend include/pixelize_model.h

include depend
