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
#include "yImage.h"
#include "yImage_io.h"
#include "yText.h"

// input images
#define BACK   "share/empty.png"
#define FIRST  "share/first.png"
#define NORMAL "share/normal.png"
#define LAST   "share/last.png"

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
void progressTo(int index, yImage *bar, yImage *first, yImage *normal, yImage *last) {

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

    bar = y_load_png(BACK);
    if(bar == NULL) {
        fprintf(stderr, "Could not read %s\n", BACK);
        return NULL;
    }

    first = y_load_png(FIRST);
    if(first == NULL) {
        fprintf(stderr, "Could not read %s\n", FIRST);
    }

    normal = y_load_png(NORMAL);
    if(normal == NULL) {
        fprintf(stderr, "Could not read %s\n", NORMAL);
    }

    last = y_load_png(LAST);
    if(last == NULL) {
        fprintf(stderr, "Could not read %s\n", LAST);
    }

    position = (int) (rate * BAR_LENGHT);
    for(i=0; i<position; i++) {
        progressTo(i, bar, first, normal, last);
    }

    return bar;
}


/**
 * \return the number of characters needed to write the given number
 */
int numberChar(int value) {
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
yImage *createText(int n, int t, int height) {

    yImage *textImage;
    int nb = 0; // the number of caracters to write
    int err;
    char text[100];

    nb = numberChar(n) + numberChar(t) + 3;

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

    textImage = createText(n, t, height);
    progressBar = concat(bar, textImage);

    y_destroy_image(bar);
    y_destroy_image(textImage);

    if(progressBar != NULL) {
        err = y_save_png(progressBar, OUTPUT);
        y_destroy_image(progressBar);
    }

    return err;
}
