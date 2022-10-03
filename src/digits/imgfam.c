#include "imgfam.h"
#include <limits.h>

/**
 * @brief create a a new familly of images.
 * @param c the size of the familly.
 */
ImgFam * newImgFam(int c) {
    ImgFam * answer = (ImgFam*)malloc(sizeof(struct imgfam));
    answer->count=c;
    answer->imgs=(Img**)malloc(sizeof(Img**)*c);
    memset(answer->imgs,0,sizeof(Img*)*c);
    return answer;
}

/** 
 * @brief Deletes a famelly of images.
 * @param ifa the familly to delete.
 */
void deleteImgFam(ImgFam * ifa) {
    for (int i=0;i<ifa->count;++i) {
        deleteImg(ifa->imgs[i]);
    }
    memset(ifa->imgs,0,sizeof(Img*)*ifa->count); 
    free(ifa->imgs);
    memset(ifa,0,sizeof(ImgFam));
    free(ifa);
}

/**
 * @brief set the i-th image of a familly of images.
 * @param imgFam the familly of images in which we are going to set the image.
 * @param i index of the image to be in the collection.
 * @param img the image that will be at i-th location in imgFam.
 */
void imgFamSetImg(ImgFam*imgFam,int i,Img*img) {
    if (i<0 && i>=imgFam->count)
        ERROR("out of bounds.","");
    imgFam->imgs[i]=img;
}

/**
 * @brief dowm sample all images in a familly using a max pooling strategy.
 * @param imgFam the familly of images in which we are going to down sample.
 * @param poolsize size of the square in which we are taking the max.
 * @param stride steps between two squares to get maximum.
 */
ImgFam * imgFamDownSampleMax(ImgFam* imgFam,int poolsize,int stride) {
    ImgFam * answer = newImgFam (imgFam->count);
    for (int i=0;i<imgFam->count;++i) {
        Img * nimg = imgDownSampleMax(imgFam->imgs[i],poolsize,stride);
        imgFamSetImg(answer,i,nimg);
    }
    return answer;
}

/**
 * @brief dowm sample all images in a familly using an average pooling strategy.
 * @param imgFam the familly of images in which we are going to down sample.
 * @param poolsize size of the square in which we are taking the average
 * @param stride steps between two squares to get average.
 */
ImgFam * imgFamDownSampleAvg(ImgFam* imgFam,int poolsize,int stride) {
    ImgFam * answer = newImgFam (imgFam->count);
    for (int i=0;i<imgFam->count;++i) {
        Img * nimg = imgDownSampleAvg(imgFam->imgs[i],poolsize,stride);
        imgFamSetImg(answer,i,nimg);
    }
    return answer;
}

/**
 * @brief Apply as a convolution filters all images in a familly.
 * @param filters the familly of filters to apply.
 * @param img the image on which to apply these filters
 * @param the filters to apply.
 * @return the newly created familly of resulting images.
 */
ImgFam * imgFamApplyConvolution(ImgFam* filters,Img* img,IntFilter intFilter) {
    ImgFam * answer = newImgFam (filters->count);
    for (int i=0;i<filters->count;++i) {
        imgFamSetImg(answer,i,imgConvolution(img,filters->imgs[i],intFilter));
    }
    return answer;
}

/**
 * @brief sets the global variables which are used to control
 * the convolution filter, based on the weight of 
 * convolution filters used.
 * @param imgFam the filters to use.
 */
void imgFamCalibrate(ImgFam*imgFam) {
    long int min=LONG_MAX;
    long int max=0;
    for (int i=0;i<imgFam->count;++i) {
        Img * img = imgFam->imgs[i];
        int aw=img->width;
        int ah=img->height;
        long int v=0;
        for(int y = 0; y < ah; y++) {
            for(int x = 0; x < aw; x++) {
                v+=255*img->data[x+aw*y];
            }
        }
        if (v>max) max=v;
        if (v<min) min=v;
    }
    if (max-min>1500) {
        fprintf(stderr,"min=%ld max=%ld delta=%ld\n",min,max,max-min);
        ERROR("Too much distance.","");
    }
    
    imgIntFilterMinMaxParam(max/2,max);
}

/**
 * @brief Saves as png files the image familly.
 * @param imgFam the familly to save.
 * @param basename the base name for all files save. The final
 *   name of each file will be basename_i.png where i
 *   is the number of the picture in the familly.
 * @see imgFamRead
 */
void imgFamWrite(ImgFam*imgFam,char*basename) {
    char s[99];
    for (int i=0;i<imgFam->count;++i) {
        sprintf(s,"%s_%d.png",basename,i);
        imgWrite(imgFam->imgs[i],s);
    }
}

/**
 * @brief Read a set of png files to return an image familly.
 * @param imgFam the familly to save.
 * @param basename the base name for all files read.
 *   names read have the form basename_i.png where i
 *   is the number of the picture in the familly.
 * @see imgFamWrite
 */
ImgFam * imgFamRead(char*basename) {
    char s[99];
    int count=0;
    int found=1;
    while (found) {
        sprintf(s,"%s_%d.png",basename,count);
        FILE * f = fopen(s,"r");
        if (f!=NULL) {
            fclose(f);
            ++count;
        } else  {
            found=0;
        }
    }
    ImgFam * answer = newImgFam(count);
    for (int i=0;i<answer->count;++i) {
        sprintf(s,"%s_%d.png",basename,i);
        imgFamSetImg(answer,i,newImgRead(s));
    }
    return answer;
}

Img* imgFamScalar(ImgFam* if1,ImgFam* if2) {
    if (if1->count!=if2->count) {
        ERROR("Wrong number of images.","");
    }
    if (if1->imgs[0]->width<if2->imgs[0]->width) {
        ERROR("Wrong size of images.","");
    }
    if (if1->imgs[0]->height<if2->imgs[0]->height) {
        ERROR("Wrong size of images.","");
    }
    int w = if1->imgs[0]->width-if2->imgs[0]->width+1;
    int h = if1->imgs[0]->height-if2->imgs[0]->height+1;
    Img * answer = newImgColor(w,h,0);
    for (int xoffset=0;xoffset<w;++xoffset) {
        for (int yoffset=0;yoffset<h;++yoffset) {
            int s;
            for (int i=0;i<if1->count;++i) {
                s+= imgScalar(if1->imgs[i],if2->imgs[i],xoffset,yoffset);
            }
            answer->data[xoffset+yoffset*w]=INBYTE(s/if1->count);
        }
    }
    return answer;
}
