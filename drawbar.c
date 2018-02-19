/*
 * This program is release under GPL license.
 * \author Yannick Garcia
 * \since 2018
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

    y_superpose_images(bar, progress, X_OFFSET + index, Y_OFFSET);
}


/**
 * Create the output image.
 * \param rate the progression to show
 * \param output filename of the file to create
 * \return 0 in case of success
 */
int create_bar(float rate, char *output) {

    yImage *bar, *first, *normal, *last;
    int err;
    int position;
    int i;

    bar = y_load_png(BACK);
    if(bar == NULL) {
        fprintf(stderr, "Could not read %s\n", BACK);
        return 1;
    }

    first = y_load_png(FIRST);
    if(first == NULL) {
        fprintf(stderr, "Could not read %s\n", FIRST);
        return 1;
    }

    normal = y_load_png(NORMAL);
    if(normal == NULL) {
        fprintf(stderr, "Could not read %s\n", NORMAL);
        return 1;
    }

    last = y_load_png(LAST);
    if(last == NULL) {
        fprintf(stderr, "Could not read %s\n", LAST);
        return 1;
    }

    position = (int) (rate * BAR_LENGHT);
    for(i=0; i<position; i++) {
        progressTo(i, bar, first, normal, last);
    }

    err = y_save_png(bar, output);
    y_destroy_image(bar);
    return err;
}


/**
 * Take two integer as parameters :
 * A number n and a total t, the progress bar will show n/t.
 */
int main(int argc, char **argv) {

    int n, t;
    float rate; // n/t

    if(argc < 3) {
        usage(argv[0]);
    }

    n=atoi(argv[1]);
    t=atoi(argv[2]);
    rate=(float) n/t;

    return create_bar(rate, OUTPUT);
}

