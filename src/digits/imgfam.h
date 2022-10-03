#ifndef IMGFAM_H
#define IMGFAM_H

#include "img.h"

/**
 * @brief A structure to store a collection (also
 * called familly) of images.
 */
struct imgfam {
    /** number of images in the familly */
    int count;
    /** image data */
    Img ** imgs;
};

/**
 * @brief Short name for 'struct imgfam'
 */
typedef struct imgfam ImgFam;

ImgFam * newImgFam(int);
void imgFamSetImg(ImgFam*,int,Img*);
ImgFam * imgFamApplyConvolution(ImgFam*,Img*,IntFilter);
ImgFam * imgFamDownSampleMax(ImgFam*,int,int);
ImgFam * imgFamDownSampleAvg(ImgFam*,int,int);
void imgFamCalibrate(ImgFam*);
void imgFamWrite(ImgFam*,char*);
ImgFam * imgFamRead(char*);
Img* imgFamScalar(ImgFam*,ImgFam*);
void deleteImgFam(ImgFam *);

#endif
