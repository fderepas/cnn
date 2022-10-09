#ifndef LAYER_H
#define LAYER_H

#include "util.h"

// external type
typedef struct filterfam FilterFam;
typedef struct imgfam ImgFam;
typedef struct img Img;

/**
 * a layer in a convolution neural network.
 */
struct layer {
    /** name of the layer */
    char * name;
    /** prefix of the path name where filters are stored. */
    char * convFilterLoc;
    /** Filters used */
    FilterFam * convFilter;
    /** value of pool size */
    int downSamplePoolSize;
    /** stride value */
    int downSampleStride;
};

typedef struct layer Layer;

Layer * newLayer(char * name,
                 char * convFilterLoc,
                 int downSamplePoolSize,
                 int downSampleStride);
void deleteLayer(Layer*);
ImgFam * layerPassImg(Layer*,Img*);
ImgFam * layerPassImgFam(Layer*,ImgFam*);
int layerCount(char * convFilterLoc);

#endif
