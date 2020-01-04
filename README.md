
# Pixelize Version  2.0.0

## By Paul Wilkins

Pixelize is a program that will use many scaled down images to try to
duplicate, as closely as possible, another image.

Pixelize works by splitting up the image you want rendered (or duplicated)
into a grid of small rectangular areas. Each area is analyzed, and replaced
with an image chosen from a large database of images. Pixelize tries to pick
images that best matches each area.

### To build Pixelize:

 * Edit the Makefile to tell the compiler where to find the X and GTK headers
   and libraries if you do not have the gtk-config script properly installed.
   Most systems will not need this.

 * Run make.

 * Copy the executables, pixelize and make_db, to it's final installation
   directory, for example /usr/local/bin.

### Usage:

 * Run make_db to create the image database (a file called pic_db.dat).
   It takes a list of files as argument.

   You could do something like
     $ find . -name "*.jpg" | xargs make_db
   or
     $ make_db *.jpg
   in the directory where your pictures are stored.

 * Run pixelize.
   * Open an image (File->Open)
   * Select Options->Render

### Source code:

Pixelize can be found at the following locations:

 * http://lashwhip.com/pixelize.html

 * https://github.com/jcdubois/pixelize

That's it.  Good Luck.

Paul

pwilkins@lashwhip.com
