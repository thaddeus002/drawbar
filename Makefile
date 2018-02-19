
CC=gcc
PROG=drawbar

all: $(PROG)


yImage/libyImage.a:
	make -C yImage libyImage.a


drawbar: drawbar.c yImage/libyImage.a
	$(CC) -o drawbar $< -IyImage -LyImage -lyImage -lpng
