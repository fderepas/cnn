#ifndef GRID_H
#define GRID_H

/**
 * @file grid.h
 * @brief Header for functions to detect the sudoku grid
 *        in a picture.
 */

typedef struct img Img;

int gridLocate(Img * img,
               int * xmin, int * ymin,
               int * xmax, int * ymax);
int gridMain(int argc,char**argv);

#endif
