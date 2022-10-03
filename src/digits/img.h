#ifndef IMG_H
#define IMG_H

#include "util.h"

/**
 * @brief A structure to store image data.
 * Field row_pointers point to and array of size height.
 * row_pointers[i] for i between 0 and height-1 points
 * to a array of size width of unsigned char storing i-th line 
 * in the picture.
 */
struct img {
    /** number of horizontal pixels */
    int width;
    /** number of vertical pixels */
    int height;
    /** This array of all pixels in the picture. */
    unsigned char * data;
    /** weight of white pixels */
    long int weight;
};

/**
 * @brief Short name for 'struct img'
 */
typedef struct img Img;

typedef unsigned char (*IntFilter)(long int);


extern long int imgIntFilterMin;
extern long int imgIntFilterMax;
extern long int imgIntFilterMaxMinusMin;
extern long int imgIntFilterMax;

void imgUpdateWeight(Img*img);
Img * newImgColor(int w, int h, unsigned char c);
Img * newImgRead(char *filename);
Img * newImgCopy(Img*myImg);
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
void imgIntFilterMinMaxParam(long int min,long int max);
unsigned char imgIntFilterMinMax(long int n) ;
Img* imgConvolution(Img*in,Img*filter,IntFilter intFilter);
Img* imgSetWeight(Img*in,int w);
Img* imgDownSampleAvg(Img* img,int poolsize,int stride);
Img* imgDownSampleMax(Img* img,int poolsize,int stride);
unsigned char imgScalar(Img*,Img*,int,int);
int imgMain(int,char**);
    
#endif

