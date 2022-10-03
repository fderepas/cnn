#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define INBYTE(x) ((x)<0?0:(x)>255?255:(x))

#define ERROR(x,y) {fprintf(stderr,"\033[31m%s:%d: %s%s\033[m\n",       \
                            __FILE__,                                   \
                            __LINE__,                                   \
                            x,y); exit(1);}
#define HERE(x) {printf("%s:%d: %s\n",                          \
                        __FILE__,                               \
                        __LINE__,                               \
                        x);}
#define HERED(x) {printf("%s:%d: %d\n",                          \
                         __FILE__,                               \
                         __LINE__,                               \
                         x);}
void writeRandomName(char*,int);
void createTmpDir(char*name,int n);
#endif
