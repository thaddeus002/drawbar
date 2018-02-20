/*
 * Copyright (c) 2018 Yannick Garcia <thaddeus.dupont@free.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GPL license. See file LICENSE for details.
 */

/**
 * \file drawbar.c
 * \brief main program file for drawbar.
 *
 * Drawbar is a command line program to create an image of a progressbar.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "yImage.h"
#include "yImage_io.h"
#include "yText.h"

#ifndef INSTALL_DIR
/* the prefix assets installation */
#define INSTALL_DIR "/usr"
#endif

// input images
#define BACK   "empty.png"
#define FIRST  "first.png"
#define NORMAL "normal.png"
#define LAST   "last.png"

#define X_OFFSET 13
#define Y_OFFSET 3
#define BAR_LENGHT 200


// generated file
#define OUTPUT "progressbar.png"

/**
 * The usage function.
 * \param prog the program's name.
 */
void usage(char *prog) {
    fprintf(stderr, "\nCreate a progressbar.\n");
    fprintf(stderr, "Usage: %s <number> <total>\n\n", prog);
    exit(1);
}


/**
 * Draw on the bar to show the given index is reached.
 */
void progress_to(int index, yImage *bar, yImage *first, yImage *normal, yImage *last) {

    yImage *progress = normal;

    if(index == 0) {
        progress = first;
    }

    if(index == BAR_LENGHT) {
        progress = last;
    }

    if(progress != NULL) {
        y_superpose_images(bar, progress, X_OFFSET + index, Y_OFFSET);
    }
}



yImage *read_file(const char *filename) {

    yImage *im;
    char *fullname = malloc(sizeof(char) * (strlen(filename) + strlen(INSTALL_DIR) + 20));

    sprintf(fullname, "%s/share/drawbar/%s", INSTALL_DIR, filename);
    im = y_load_png(fullname);
    if(im == NULL) {
        fprintf(stderr, "Could not read %s\n", fullname);
    }

    free(fullname);
    return im;
}



/**
 * Create the output image.
 * \param rate the progression to show
 * \return The created image or NULL
 */
yImage *create_bar(float rate) {

    yImage *bar, *first, *normal, *last;
    int err;
    int position;
    int i;

    bar = read_file(BACK);
    if(bar == NULL) {
        return NULL;
    }

    first = read_file(FIRST);

    normal = read_file(NORMAL);

    last = read_file(LAST);

    position = (int) (rate * BAR_LENGHT);
    for(i=0; i<position; i++) {
        progress_to(i, bar, first, normal, last);
    }

    return bar;
}


/**
 * \return the number of characters needed to write the given number
 */
int number_char(int value) {
    int i = 0;

    if(value==0) return 1;

    while(value>0) {
        i++;
        value = value / 10;
    }

    return i;
}


/**
 * Create the text to illustrate the progressbar.
 */
yImage *create_text(int n, int t, int height) {

    yImage *textImage;
    int nb = 0; // the number of caracters to write
    int err;
    char text[100];

    nb = number_char(n) + number_char(t) + 3;

    textImage = y_create_image(&err, NULL, nb*9+18, height);
    if(err) return NULL;

    y_transp(textImage);
    sprintf(text, "  %d/%d", n, t);
    y_display_text(textImage, 0, (height-12) / 2, text);

    return textImage;
}



yImage *concat(yImage *bar, yImage *text){

    int l, h;
    yImage *image;
    int err;
    int x = 0;

    l = 0;
    h = 0;

    if(bar != NULL) {
        h = bar->rgbHeight;
        l += bar->rgbWidth;
    }

    if(text != NULL) {
        if(text->rgbHeight > h) {
            h = text->rgbHeight;
        }
        l += text->rgbWidth;
    }

    if(l==0 || h==0) {
        return NULL;
    }

    image = y_create_image(&err, NULL, l, h);
    if(err) return NULL;

    y_transp(image);

    if(bar != NULL) {
        y_superpose_images(image, bar, x, 0);
        x+=bar->rgbWidth;
    }

    if(text != NULL) {
        y_superpose_images(image, text, x, 0);
    }

    return image;
}




/**
 * Take two integer as parameters :
 * A number n and a total t, the progress bar will show n/t.
 */
int main(int argc, char **argv) {

    int n, t;
    float rate; // n/t
    yImage *bar;
    yImage *textImage;
    int height = 20;
    yImage *progressBar;
    int err = -1;

    if(argc < 3) {
        usage(argv[0]);
    }

    n=atoi(argv[1]);
    t=atoi(argv[2]);
    rate=(float) n/t;

    bar = create_bar(rate);
    if(bar!=NULL) {
        height = bar->rgbHeight;
    }

    textImage = create_text(n, t, height);
    progressBar = concat(bar, textImage);

    y_destroy_image(bar);
    y_destroy_image(textImage);

    if(progressBar != NULL) {
        err = y_save_png(progressBar, OUTPUT);
        y_destroy_image(progressBar);
    }

    return err;
}
