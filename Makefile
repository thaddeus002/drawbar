
CC=gcc
PROG=drawbar

ifndef PREFIX
PREFIX=/usr/local
endif

all: $(PROG)


yImage/libyImage.a:
	PREFIX=$(PREFIX) make -C yImage libyImage.a


drawbar: drawbar.c yImage/libyImage.a
	$(CC) -o drawbar $< -DINSTALL_DIR=\"$(PREFIX)\" -IyImage -LyImage -lyImage -lpng


install: drawbar
	install drawbar $(PREFIX)/bin
	mkdir -p $(PREFIX)/share/drawbar
	install -m 644 share/*.png $(PREFIX)/share/drawbar

.PHONY: mrproper

mrproper:
	make -C yImage mrproper
	rm -f *.o $(PROG)
