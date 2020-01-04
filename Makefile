# compile stuff in this dir

CC = gcc
GLADE2C = ./tools/glade2c

CFLAGS += -Iinclude
CFLAGS += -I/usr/X11R6/include `pkg-config --cflags gtk+-3.0`
CFLAGS += -O3
CFLAGS += -fPIE
CFLAGS += -Wl,--export-dynamic
CFLAGS += -DGSEAL_ENABLE
CLFLAS += -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED
CFLAGS += -DGTK_DISABLE_SINGLE_INCLUDES
CFLAGS += -DGDK_PIXBUF_DISABLE_DEPRECATED
CFLAGS += -DG_DISABLE_DEPRECATED
CFLAGS += -Wall -Wextra -Werror
DFLAGS = -L/usr/X11R6/lib
DFLAGS += `pkg-config --libs gtk+-3.0`
DFLAGS += `pkg-config --libs gmodule-2.0`
DFLAGS += -Wall -Wextra
DFLAGS += -fPIE
LIBS = -lm

SRC_FILES := $(wildcard src/*.c)
HDR_FILES := $(wildcard include/*.h)
OBJ_FILES := $(subst .c,.o,$(subst src/,,$(SRC_FILES)))

DB_OBJS  = make_db.o
PIX_OBJS = $(filter-out $(DB_OBJS),$(OBJ_FILES))

all:	make_db pixelize

make_db:	$(DB_OBJS)
	$(CC) -o $@ $(DB_OBJS) $(DFLAGS) $(LIBS)

pixelize:	$(PIX_OBJS)
	$(CC) -o $@ $(PIX_OBJS) $(DFLAGS) $(LIBS)

%.o:	depend
%.o:	src/%.c
	$(CC) -c -o $@ $(CFLAGS) $<

depend:	Makefile $(HDR_FILES) $(SRC_FILES) include/pixelize_model.h
	$(CC) -MM $(CFLAGS) $(SRC_FILES) > depend

include/pixelize_model.h: glade/pixelize.glade
	$(GLADE2C) -i $< -o $@
	clang-format -i $@

clean:
	rm -rf $(OBJ_FILES) pixelize make_db core depend include/pixelize_model.h

include depend
