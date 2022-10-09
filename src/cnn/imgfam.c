#include <limits.h>
#include "imgfam.h"
#include "filterfam.h"

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
    if (ifa==NULL) return;
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
 * @brief applies the imgLuminosityScale function to all
 *        images in a familly.
 * @param imgFam the familly on which we want to apply imgLuminosityScale.
 * @return the newly allocated familly where imgLuminosityScale has been 
 *         applied.
 */
ImgFam* imgFamLuminosityScale(ImgFam*imgFam) {
    ImgFam * answer = newImgFam(imgFam->count);
    for (int i=0;i<imgFam->count;++i) {
        imgFamSetImg(answer,i,imgLuminosityScale(imgFam->imgs[i]));
    }
    return answer;
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
        snprintf(s,99,"%s_%d.png",basename,i);
        imgWrite(imgFam->imgs[i],s);
    }
}

/**
 * @brief Read a set of png files to return an image familly.
 * @param basename the base name for all files read.
 *   names read have the form basename_i.png where i
 *   is the number of the picture in the familly.
 * @return a newly allocated image familly.
 * @see imgFamWrite
 */
ImgFam * imgFamRead(char*basename) {
    char s[99];
    int count=0;
    int found=1;
    while (found) {
        snprintf(s,99,"%s_%d.png",basename,count);
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
        snprintf(s,99,"%s_%d.png",basename,i);
        imgFamSetImg(answer,i,newImgRead(s));
    }
    return answer;
}

Img* imgFamScalar(ImgFam* if1,FilterFam* if2) {
    if (if1->count!=if2->count) {
        ERROR("Wrong number of images.","");
    }
    if (if1->imgs[0]->width<if2->filters[0]->img->width) {
        ERROR("Wrong size of images.","");
    }
    if (if1->imgs[0]->height<if2->filters[0]->img->height) {
        ERROR("Wrong size of images.","");
    }
    int w = if1->imgs[0]->width-if2->filters[0]->img->width+1;
    int h = if1->imgs[0]->height-if2->filters[0]->img->height+1;
    Img * answer = newImgColor(w,h,0);
    for (int xoffset=0;xoffset<w;++xoffset) {
        for (int yoffset=0;yoffset<h;++yoffset) {
            int s=0;
            for (int i=0;i<if1->count;++i) {
                s+= imgScalar(if1->imgs[i],if2->filters[i]->img,xoffset,yoffset);
            }
            answer->data[xoffset+yoffset*w]=INBYTE(s/if1->count);
        }
    }
    return answer;
}

void imgFamPrint(ImgFam* ifa) {
    for (int i=0;i<ifa->count;++i) {
        printf("=-=-=-=-=-=-=-=-=-=-=-=-= %d/%d\n",i,ifa->count);
        imgPrint(ifa->imgs[i]);
    }
}

