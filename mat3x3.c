
/*
 * 3x3 matrix versions of Paul Haeberli's code from
 *
 *   "Matrix Operations for Image Processing", November 1993.
 *
 *  See:
 *    http://www.sgi.com/grafica/matrix/index.html
 *    http://www.sgi.com/grafica/matrix/matrix.c
 */

#include <stdio.h>
#include <math.h>

//#include <GL/glut.h>

/* Some <math.h> files do not define M_PI... */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "mat3x3.h"

#define RLUM    (0.3086)
#define GLUM    (0.6094)
#define BLUM    (0.0820)

/* 
 *      printmat -      
 *              print a 3 by 3 matrix
 */
void
printmat3(float mat[3][3])
{
    int x, y;

    fprintf(stderr,"\n");
    for(y=0; y<3; y++) {
        for(x=0; x<3; x++) 
           fprintf(stderr,"%f ",mat[y][x]);
        fprintf(stderr,"\n");
    }
    fprintf(stderr,"\n");
}

/* 
 *      matrixmult3 -    
 *              multiply two 3x3 matricies
 */
void
matrixmult3(float a[3][3], float b[3][3], float c[3][3])
{
    int x, y;
    float temp[3][3];

    for(y=0; y<3 ; y++)
        for(x=0 ; x<3 ; x++) {
            temp[y][x] = b[y][0] * a[0][x]
                       + b[y][1] * a[1][x]
                       + b[y][2] * a[2][x];
        }
    for(y=0; y<3; y++)
        for(x=0; x<3; x++)
            c[y][x] = temp[y][x];
}

/* 
 *      identmat3 -      
 *              make an identity 3x3 matrix
 */
void
identmat3(float matrix[3][3])
{
    matrix[0][0] = 1.0;    /* row 1        */
    matrix[0][1] = 0.0;
    matrix[0][2] = 0.0;
    matrix[1][0] = 0.0;    /* row 2        */
    matrix[1][1] = 1.0;
    matrix[1][2] = 0.0;
    matrix[2][0] = 0.0;    /* row 3        */
    matrix[2][1] = 0.0;
    matrix[2][2] = 1.0;
}

/* 
 *      xformpnt3 -      
 *              transform a 3D point using a 3x3 matrix
 */
void
xformpnt3(float matrix[3][3], float x, float y, float z,
          float *tx, float *ty, float *tz)
{
    *tx = x*matrix[0][0] + y*matrix[1][0] + z*matrix[2][0];
    *ty = x*matrix[0][1] + y*matrix[1][1] + z*matrix[2][1];
    *tz = x*matrix[0][2] + y*matrix[1][2] + z*matrix[2][2];
}

/* 
 *      cscalemat3 -     
 *              make a color scale 3x3 matrix
 */
void
cscalemat3(float mat[3][3], float rscale, float gscale, float bscale)
{
    float mmat[3][3];

    mmat[0][0] = rscale;
    mmat[0][1] = 0.0;
    mmat[0][2] = 0.0;

    mmat[1][0] = 0.0;
    mmat[1][1] = gscale;
    mmat[1][2] = 0.0;

    mmat[2][0] = 0.0;
    mmat[2][1] = 0.0;
    mmat[2][2] = bscale;

    matrixmult3(mmat,mat,mat);
}

/* 
 *      lummat3 -        
 *              make a luminance 3x3 matrix
 */
void
lummat3(float mat[3][3])
{
    float mmat[3][3];
    float rwgt, gwgt, bwgt;

    rwgt = RLUM;
    gwgt = GLUM;
    bwgt = BLUM;
    mmat[0][0] = rwgt;
    mmat[0][1] = rwgt;
    mmat[0][2] = rwgt;

    mmat[1][0] = gwgt;
    mmat[1][1] = gwgt;
    mmat[1][2] = gwgt;

    mmat[2][0] = bwgt;
    mmat[2][1] = bwgt;
    mmat[2][2] = bwgt;

    matrixmult3(mmat,mat,mat);
}

/* 
 *      saturatemat3 -   
 *              make a saturation 3x3 matrix
 */
void
saturatemat3(float mat[3][3], float sat)
{
    float mmat[3][3];
    float a, b, c, d, e, f, g, h, i;
    float rwgt, gwgt, bwgt;

    rwgt = RLUM;
    gwgt = GLUM;
    bwgt = BLUM;

    a = (1.0-sat)*rwgt + sat;
    b = (1.0-sat)*rwgt;
    c = (1.0-sat)*rwgt;
    d = (1.0-sat)*gwgt;
    e = (1.0-sat)*gwgt + sat;
    f = (1.0-sat)*gwgt;
    g = (1.0-sat)*bwgt;
    h = (1.0-sat)*bwgt;
    i = (1.0-sat)*bwgt + sat;
    mmat[0][0] = a;
    mmat[0][1] = b;
    mmat[0][2] = c;

    mmat[1][0] = d;
    mmat[1][1] = e;
    mmat[1][2] = f;

    mmat[2][0] = g;
    mmat[2][1] = h;
    mmat[2][2] = i;

    matrixmult3(mmat,mat,mat);
}

/* 
 *      xrotate3 -       
 *              rotate about the x (red) axis
 */
void
xrotatemat3(float mat[3][3], float rs, float rc)
{
    float mmat[3][3];

    mmat[0][0] = 1.0;
    mmat[0][1] = 0.0;
    mmat[0][2] = 0.0;

    mmat[1][0] = 0.0;
    mmat[1][1] = rc;
    mmat[1][2] = rs;

    mmat[2][0] = 0.0;
    mmat[2][1] = -rs;
    mmat[2][2] = rc;

    matrixmult3(mmat,mat,mat);
}

/* 
 *      yrotate -       
 *              rotate about the y (green) axis
 */
void
yrotatemat3(float mat[3][3], float rs, float rc)
{
    float mmat[3][3];

    mmat[0][0] = rc;
    mmat[0][1] = 0.0;
    mmat[0][2] = -rs;

    mmat[1][0] = 0.0;
    mmat[1][1] = 1.0;
    mmat[1][2] = 0.0;

    mmat[2][0] = rs;
    mmat[2][1] = 0.0;
    mmat[2][2] = rc;

    matrixmult3(mmat,mat,mat);
}

/* 
 *      zrotate3 -       
 *              rotate about the z (blue) axis
 */
void
zrotatemat3(float mat[3][3], float rs, float rc)
{
    float mmat[3][3];

    mmat[0][0] = rc;
    mmat[0][1] = rs;
    mmat[0][2] = 0.0;

    mmat[1][0] = -rs;
    mmat[1][1] = rc;
    mmat[1][2] = 0.0;

    mmat[2][0] = 0.0;
    mmat[2][1] = 0.0;
    mmat[2][2] = 1.0;

    matrixmult3(mmat,mat,mat);
}

/* 
 *      zshear3 -        
 *              shear z using x and y.
 */
void
zshearmat3(float mat[3][3], float dx, float dy)
{
    float mmat[3][3];

    mmat[0][0] = 1.0;
    mmat[0][1] = 0.0;
    mmat[0][2] = dx;

    mmat[1][0] = 0.0;
    mmat[1][1] = 1.0;
    mmat[1][2] = dy;

    mmat[2][0] = 0.0;
    mmat[2][1] = 0.0;
    mmat[2][2] = 1.0;

    matrixmult3(mmat,mat,mat);
}

/* 
 *      simplehuerotatemat -    
 *              simple hue rotation. This changes luminance 
 */
void
simplehuerotatemat3(float mat[3][3], float rot)
{
    float mag;
    float xrs, xrc;
    float yrs, yrc;
    float zrs, zrc;

/* rotate the grey vector into positive Z */
    mag = sqrt(2.0);
    xrs = 1.0/mag;
    xrc = 1.0/mag;
    xrotatemat3(mat,xrs,xrc);

    mag = sqrt(3.0);
    yrs = -1.0/mag;
    yrc = sqrt(2.0)/mag;
    yrotatemat3(mat,yrs,yrc);

/* rotate the hue */
    zrs = sin(rot*M_PI/180.0);
    zrc = cos(rot*M_PI/180.0);
    zrotatemat3(mat,zrs,zrc);

/* rotate the grey vector back into place */
    yrotatemat3(mat,-yrs,yrc);
    xrotatemat3(mat,-xrs,xrc);
}

/* 
 *      huerotatemat -  
 *              rotate the hue, while maintaining luminance.
 */
void
huerotatemat3(float mat[3][3], float rot)
{
    float mmat[3][3];
    float mag;
    float lx, ly, lz;
    float xrs, xrc;
    float yrs, yrc;
    float zrs, zrc;
    float zsx, zsy;

    identmat3(mmat);

/* rotate the grey vector into positive Z */
    mag = sqrt(2.0);
    xrs = 1.0/mag;
    xrc = 1.0/mag;
    xrotatemat3(mmat,xrs,xrc);
    mag = sqrt(3.0);
    yrs = -1.0/mag;
    yrc = sqrt(2.0)/mag;
    yrotatemat3(mmat,yrs,yrc);

/* shear the space to make the luminance plane horizontal */
    xformpnt3(mmat,RLUM,GLUM,BLUM,&lx,&ly,&lz);
    zsx = lx/lz;
    zsy = ly/lz;
    zshearmat3(mmat,zsx,zsy);

/* rotate the hue */
    zrs = sin(rot*M_PI/180.0);
    zrc = cos(rot*M_PI/180.0);
    zrotatemat3(mmat,zrs,zrc);

/* unshear the space to put the luminance plane back */
    zshearmat3(mmat,-zsx,-zsy);

/* rotate the grey vector back into place */
    yrotatemat3(mmat,-yrs,yrc);
    xrotatemat3(mmat,-xrs,xrc);

    matrixmult3(mmat,mat,mat);
}
