#ifndef FILTER_H
#define FILTER_H

#include "img.h"

/**
 * @brief Data structure to store a filter.
 */
struct filter {
    /** image used for the filter */
    Img *img;
    /** weight of white pixels (sum of all pixel values) */
    long int weight;
    /** threshold to trigger white pixel, used in filters only */
    long int threshold;
    /** maximum value when the image is used as a filter */
    long int maxVal;
    /** percentage used to compute threshold from maxVal */
    int percent;
};


/**
 * @brief Short name for 'struct filter'
 */
typedef struct filter Filter;

Filter * newFilter(Img*,int);
Filter * newFilterRead(char *basename);
void deleteFilter(Filter*);
void filterSetWeight(Filter*f,int w);
void filterUpdateValues(Filter*);
void filterWrite(Filter*f,char*basename);
#endif
