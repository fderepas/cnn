#ifndef IMG_H
#define IMG_H

#include "util.h"

/**
 * @brief A structure to store greyscale image data.
 */
struct img {
    /** number of horizontal pixels */
    int width;
    /** number of vertical pixels */
    int height;
    /** This is an allocated buffer of all pixels in the picture. 
     * Its size should be width times height.
     */
    unsigned char * data;
};

/**
 * @brief Short name for 'struct img'
 */
typedef struct img Img;

/* external data types */
typedef struct filter Filter;

Img * newImgFromArray(int w, int h, unsigned char *s);
Img * newImgColor(int w, int h, unsigned char c);
Img * newImgRead(char *filename);
Img * newImgCopy(Img*myImg);
Img * newImg9By9Dots(int w);
Img * newImgSudoku(int sz);
Img * newImgSquare(int s);
Img * newImgVerticalBar(int s,int w);
void deleteImg(Img*myImg);
unsigned char imgGetVal(Img*p,int x, int y);
void imgPrint(Img*myImg);
void imgWrite(Img*myImg,char *filename);
Img* imgInvert(Img*in);
Img* imgFlattenContrast(Img*in);
Img* imgRaiseContrast(Img*in);
Img* imgBlur(Img*in,int radius);
Img* imgMake3dEffect(Img*in);
Img* imgLuminosityScale(Img*in);
Img* imgConvolution(Img*in,Filter*filter);
Img* imgDownSampleAvg(Img* img,int poolsize,int stride);
Img* imgDownSampleMax(Img* img,int poolsize,int stride);
unsigned char imgScalar(Img*,Img*,int,int);
Img* imgRotate(Img* img,int deg);
Img* imgScale(Img*in,int s);
int imgGetWeight(Img*in);
int imgMain(int,char**);
    
#endif

