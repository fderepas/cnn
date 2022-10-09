#include "layer.h"
#include "filterfam.h"
#include "imgfam.h"

/**
 * @brief Allocates memory for a new layer.
 * @param name name of this layer (a filename)
 * @param convFilterLoc name of the convolution filter to load
 * @param downSamplePoolSize pool size value for downsizing.
 * @param downSampleStride stride value for downsizing.
 */
Layer * newLayer(char * name,
                 char * convFilterLoc,
                 int downSamplePoolSize,
                 int downSampleStride)
{
    Layer * answer = (Layer*)malloc(sizeof(struct layer));
    answer->name = stringCopy(name);
    answer->convFilterLoc = stringCopy(convFilterLoc);
    answer->convFilter=filterFamRead(convFilterLoc);
    answer->downSamplePoolSize=downSamplePoolSize;
    answer->downSampleStride=downSampleStride;
    return answer;
}

/**
 * @brief free space previously allocated for a layer.
 */
void deleteLayer(Layer* l) {
    free(l->name);
    free(l->convFilterLoc);
    deleteFilterFam(l->convFilter);
    memset(l,0,sizeof(Layer));
    free(l);
}

/**
 * @brief Get the number of images in the filter.
 * @param convFilterLoc location of the pictures. Name of pictures
 *        will be convFilterLoc_<num>.png
 * @return number of elements in this filter
 */
int layerCount(char * convFilterLoc) {
    return filterFamCount(convFilterLoc);
}

/**
 * @brief Passes an image through a layer
 * @param l a layer 
 * @param i an image
 * @return output of i through l
 */
ImgFam * layerPassImg(Layer*l,Img*i) {
    ImgFam * intermediateOutput =
        filterFamApplyConvolution(l->convFilter,i);
    ImgFam * maxPoolOutput=imgFamDownSampleMax(intermediateOutput,
                                               l->downSamplePoolSize,
                                               l->downSampleStride);
    return maxPoolOutput;
}

/**
 * @brief Passes a familly of images through a layer
 * @param l a layer 
 * @param i a familly of images
 * @return output of i through l
 */
ImgFam * layerPassImgFam(Layer*l,ImgFam*i) {
    ImgFam * intermediateOutput =
        filterFamApplyConvolutionOnFam(l->convFilter,i);
    ImgFam * maxPoolOutput=imgFamDownSampleMax(intermediateOutput,
                                               l->downSamplePoolSize,
                                               l->downSampleStride);
    return maxPoolOutput;
}
