#ifndef GRID_H
#define GRID_H

typedef struct img Img;

int gridLocate(Img * img,
               int * xmin, int * ymin,
               int * xmax, int * ymax);
int gridMain(int argc,char**argv);

#endif
