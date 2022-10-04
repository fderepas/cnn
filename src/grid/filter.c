#include "filter.h"
#include <math.h>

/**
 * @brief Allocates space for a new filter
 * @param img image on which the filter is based
 * @param percent : given maxVal, the maximum value we can get if 
 *        image img is used as a filter, threshold will be 
 *        percentage of maxVal.
 */
Filter * newFilter(Img* img,int percent) {
    Filter * answer = (Filter*) malloc(sizeof(struct filter));
    answer->img=img;
    answer->threshold=0;
    answer->percent=percent;
    answer->weight=0;
    answer->maxVal=0;
    answer->data=NULL;
    if (img==NULL) return answer;
    filterUpdateValues(answer);
    return answer;
}

/**
 * @brief Releases memory allocated for a filter.
 * @param f filter for which memory is going to be freed
 *        Nothing happens if f is NULL.
 */
void deleteFilter(Filter*f) {
    if (f==NULL) return;
    if (f->img)
        deleteImg(f->img);
    f->weight=0;
    f->threshold=0;
    f->maxVal=0;
    if (f->data!=NULL)
        free(f->data);
    memset(f,0,sizeof(Filter));
    free(f);
}

/**
 * @brief Copies a string in the data field of a filter.
 * @param f a filter
 * @param s a string to be copied in f->data
 */
void filterSetData(Filter * f , char *s) {
    f->data=stringCopy(s);
}

/**
 * @brief sets the filter image to a given weight
 * @param f filter which weight is going to be changed
 * @param w the new weight of the filter
 */
void filterSetWeight(Filter*f,int w) {
    long int ww=0;
    for(int i = 0; i < f->img->height*f->img->width; i++) {
        ww+=f->img->data[i];
    }
    float ratio = (w+0.)/(ww+0.);
    for(int i = 0; i < f->img->height*f->img->width; i++) {
        f->img->data[i]=INBYTE((int)round(ratio*f->img->data[i]));
    }
    filterUpdateValues(f);
}

/**
 * @brief Updates weight, threshold and maxVal values in a filter
 *        given the image and the percentage threshold should have.
 * @param f filter to update
 */
void filterUpdateValues(Filter*f) {
    f->weight=0;
    f->threshold=0;
    f->maxVal=0;
    if (f->percent>100 || f->percent<0) {
        ERROR("percent should be between 0 and 100.","");
    }
    for(int i = 0; i < f->img->height*f->img->width; i++) {
        f->weight+=f->img->data[i];
        f->maxVal+=f->img->data[i]*255;
    }
    f->threshold=f->maxVal*f->percent/100;
}

/**
 * @brief Saves as png files the filter
 * @param f filter to save.
 * @param basename the base name for files to save.
 *        Files basename+'.png' and basename+'.txt'
 *        are going to be created.
 * @see newFilterRead
 */
void filterWrite(Filter*f,char*basename) {
    char s[99];
    // save the picture part
    sprintf(s,"%s.png",basename);
    imgWrite(f->img,s);
    // save the data part
    sprintf(s,"%s.txt",basename);
    FILE * fi = fopen(s,"w");
    if (fi==NULL)
        ERROR("Could not open file ",s);
    fprintf(fi,"%d\n",f->percent);
    fprintf(fi,"%s\n",f->data);
    fclose(fi);
}

/**
 * @brief Reads a filter from the file system
 * @param basename the base name for files to read.
 *        Files basename+'.png' and basename+'.txt'
 *        are expected to be found.
 * @return the newly created Filter object from data read.
 * @see filterWrite
 */
Filter * newFilterRead(char *basename) {
    Filter * answer = (Filter*) malloc(sizeof(struct filter));
    char s[99];
    // read the picture part
    sprintf(s,"%s.png",basename);
    answer->img=newImgRead(s);
    // read the data part
    sprintf(s,"%s.txt",basename);
    FILE * fi = fopen(s,"r");
    if (fi==NULL)
        ERROR("Could not open file ",s);
    fscanf(fi,"%d",&(answer->percent));
    
    char * line = NULL;
    size_t len = 0;
    if (getline(&line, &len, fi)==-1) {
        WARNING("Error while reading ",s);
    } else {
        answer->data=stringCopy(line);
    }
    if (line) free(line);
    fclose(fi);
    filterUpdateValues(answer);
    return answer;
}
