#include <unistd.h>
#include "filterfam.h"
#include "imgfam.h"

/**
 * @brief creates a a new familly of filters.
 * @param c the size of the familly.
 */
FilterFam * newFilterFam(int c) {
    FilterFam * answer = (FilterFam*)malloc(sizeof(struct filterfam));
    answer->count=c;
    answer->filters=(Filter**)malloc(sizeof(Filter**)*c);
    memset(answer->filters,0,sizeof(Filter*)*c);
    return answer;
}

/** 
 * @brief Deletes a familly of filters.
 * @param ff the familly to delete.
 */
void deleteFilterFam(FilterFam * ff) {
    if (ff==NULL) return;
    for (int i=0;i<ff->count;++i) {
        deleteFilter(ff->filters[i]);
    }
    memset(ff->filters,0,sizeof(Filter*)*ff->count); 
    free(ff->filters);
    memset(ff,0,sizeof(FilterFam));
    free(ff);
}

/**
 * @brief counts the number of filters present in familly with prefix
 *        convFilterLoc
 * @param convFilterLoc a string to be used as a base for the name of
 *        the picture : convFilterLoc_<num>.png
 * @return number of elements in this filter
 */
int filterFamCount(char * convFilterLoc) {
    int answer =-1;
    char fname[strlen(convFilterLoc)+10];
    do {
        answer ++;
        snprintf(fname,strlen(convFilterLoc)+10,
                 "%s_%d.png",convFilterLoc,answer);
    } while (access(fname, F_OK) == 0);
    return answer;
}

/**
 * @brief set the i-th image of a familly of images.
 * @param filterFam the familly of images in which we are going 
 *        to set the image.
 * @param i index of the image to be in the collection.
 * @param filter the image that will be at i-th location in filterFam.
 */
void filterFamSetFilter(FilterFam*filterFam,int i,Filter*filter) {
    if (i<0 && i>=filterFam->count)
        ERROR("out of bounds.","");
    filterFam->filters[i]=filter;
}

/**
 * @brief Apply all filter of the familly on an image with 
 *        a positive convolution.
 * @param filters the familly of filters to apply.
 * @param img the image on which to apply these filters
 * @param the filters to apply.
 * @return the newly created familly of resulting images.
 */
ImgFam * filterFamApplyConvolution(FilterFam* filters,Img* img) {
    ImgFam * answer = newImgFam (filters->count);
    for (int i=0;i<filters->count;++i) {
        if (filters->filters[i]==NULL) {
            ERROR("Not all filters in familly have been initialized.","");
        }
        imgFamSetImg(answer,i,imgConvolution(img,filters->filters[i]));
    }
    return answer;
}

/**
 * @brief Apply all filter of the familly on an image.
 * @param filters the familly of filters to apply.
 * @param img the image on which to apply these filters
 * @param the filters to apply.
 * @return the newly created familly of resulting images.
 */
ImgFam * filterFamApplyConvolutionDiff(FilterFam* filters,Img* img) {
    ImgFam * answer = newImgFam (filters->count);
    for (int i=0;i<filters->count;++i) {
        if (filters->filters[i]==NULL) {
            ERROR("Not all filters in familly have been initialized.","");
        }
        imgFamSetImg(answer,i,imgConvolutionDiff(img,filters->filters[i]));
    }
    return answer;
}

/**
 * @brief Apply all filter of the familly on an image an keep original 
 *        image size.
 * @param filters the familly of filters to apply.
 * @param img the image on which to apply these filters
 * @param the filters to apply.
 * @return the newly created familly of resulting images.
 */
ImgFam * filterFamApplyConvolutionSameSize(FilterFam* filters,Img* img) {
    ImgFam * answer = newImgFam (filters->count);
    for (int i=0;i<filters->count;++i) {
        if (filters->filters[i]==NULL) {
            ERROR("Not all filters in familly have been initialized.","");
        }
        imgFamSetImg(answer,i,imgConvolutionSameSize(img,filters->filters[i]));
    }
    return answer;
}


/**
 * @brief Apply all filter of the familly on an image an keep original 
 *        image size.
 * @param filters the familly of filters to apply.
 * @param img the image on which to apply these filters
 * @param the filters to apply.
 * @return the newly created familly of resulting images.
 */
ImgFam * filterFamApplyConvolutionSameSizeDiff(FilterFam* filters,Img* img) {
    ImgFam * answer = newImgFam (filters->count);
    for (int i=0;i<filters->count;++i) {
        if (filters->filters[i]==NULL) {
            ERROR("Not all filters in familly have been initialized.","");
        }
        imgFamSetImg(answer,i,imgConvolutionSameSizeDiff(img,filters->filters[i]));
    }
    return answer;
}

/**
 * @brief Apply all filter of the familly on a familly of images.
 * @param filters the familly of filters to apply.
 * @param img the image on which to apply these filters
 * @param the filters to apply.
 * @return the newly created familly of resulting images.
 */
ImgFam * filterFamApplyConvolutionOnFam(FilterFam* filters,ImgFam* imgFam) {
    ImgFam * answer = newImgFam (filters->count*imgFam->count);
    for (int i=0;i<filters->count;++i) {
        for (int j=0;j<imgFam->count;++j) {
            int idx=j+i*imgFam->count;
            imgFamSetImg(answer,idx,imgConvolution(imgFam->imgs[j],
                                                   filters->filters[i]));
    }   }
    return answer;
}

/**
 * @brief Apply all filter of the familly on a familly of images,
 *        and keep same image size.
 * @param filters the familly of filters to apply.
 * @param img the image on which to apply these filters
 * @param the filters to apply.
 * @return the newly created familly of resulting images.
 */
ImgFam * filterFamApplyConvolutionSameSizeOnFam(FilterFam* filters,
                                                ImgFam* imgFam)
{
    ImgFam * answer = newImgFam (filters->count*imgFam->count);
    for (int i=0;i<filters->count;++i) {
        for (int j=0;j<imgFam->count;++j) {
            int idx=j+i*imgFam->count;
            imgFamSetImg(answer,idx,imgConvolution(imgFam->imgs[j],
                                                   filters->filters[i]));
    }   }
    return answer;
}

/**
 * @brief Saves as png files the image familly.
 * @param filterFam the familly to save.
 * @param basename the base name for all files save. The final
 *   name of each file will be basename_i.png where i
 *   is the number of the picture in the familly.
 * @see filterFamRead
 */
void filterFamWrite(FilterFam*filterFam,char*basename) {
    char s[99];
    for (int i=0;i<filterFam->count;++i) {
        snprintf(s,99,"%s_%d",basename,i);
        filterWrite(filterFam->filters[i],s);
    }
}

/**
 * @brief Read a set of png files to return a filter familly.
 * @param basename the base name for all files read.
 *   names read have the form basename_i.png where i
 *   is the number of the picture in the familly.
 * @return a newly allocated filter familly.
 * @see imgFamWrite
 */
FilterFam * filterFamRead(char*basename) {
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
    FilterFam * answer = newFilterFam(count);
    for (int i=0;i<answer->count;++i) {
        snprintf(s,99,"%s_%d",basename,i);
        filterFamSetFilter(answer,i,newFilterRead(s));
    }
    return answer;
}
