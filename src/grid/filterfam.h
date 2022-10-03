#ifndef FILTERFAM_H
#define FILTERFAM_H

#include "filter.h"


/**
 * @brief A structure to store a collection (also
 * called familly) of filters.
 */
struct filterfam {
    /** number of images in the familly */
    int count;
    /** filter data */
    Filter ** filters;
};

/**
 * @brief Short name for 'struct filterfam'
 */
typedef struct filterfam FilterFam;

// external types
typedef struct imgfam ImgFam;

FilterFam * newFilterFam(int);
ImgFam * filterFamApplyConvolution(FilterFam* filters,Img* img);
ImgFam * filterFamApplyConvolutionOnFam(FilterFam* filters,ImgFam* imgFam);
void filterFamSetFilter(FilterFam*filterFam,int i,Filter*filter);
void deleteFilterFam(FilterFam *);
FilterFam * filterFamRead(char*basename);
void filterFamWrite(FilterFam*filterFam,char*basename);

#endif
