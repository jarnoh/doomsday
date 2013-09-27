
/*
 * Prototypes for 3x3 matrix versions of Paul Haeberli's code from
 *
 *   "Matrix Operations for Image Processing", November 1993.
 *
 *  See:
 *    http://www.sgi.com/grafica/matrix/index.html
 *    http://www.sgi.com/grafica/matrix/matrix.c
 */

#ifdef __cplusplus
extern "C"
{
#endif

extern void printmat3(float mat[3][3]);
extern void matrixmult3(float a[3][3], float b[3][3], float c[3][3]);
extern void identmat3(float matrix[3][3]);
extern void xformpnt3(float matrix[3][3], float x, float y, float z, float *tx, float *ty, float *tz);
extern void cscalemat3(float mat[3][3], float rscale, float gscale, float bscale);
extern void lummat3(float mat[3][3]);
extern void saturatemat3(float mat[3][3], float sat);
extern void xrotatemat3(float mat[3][3], float rs, float rc);
extern void yrotatemat3(float mat[3][3], float rs, float rc);
extern void zrotatemat(float mat[3][3], float rs, float rc);
extern void zshearmat3(float mat[3][3], float dx, float dy);
extern void simplehuerotatemat3(float mat[3][3], float rot);
extern void huerotatemat3(float mat[3][3], float rot);



#ifdef __cplusplus
}
#endif
