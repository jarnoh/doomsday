#include"IFS.h"
#include<math.h>
#include<string.h>


namespace IFS
{
///////////////////////////////////////////////////////////////////////////////
// quaternions
///////////////////////////////////////////////////////////////////////////////
void tform2quat(float *q, const float a[4][3])
{
	float trace = a[0][0] + a[1][1] + a[2][2] + 1.0f;
	if(trace > 0.000001f)
	{
		float s = 0.5f / sqrtf(trace);
		q[0] = 0.25f / s;
		q[1] = ( a[2][1] - a[1][2] ) * s;
		q[2] = ( a[0][2] - a[2][0] ) * s;
		q[3] = ( a[1][0] - a[0][1] ) * s;
	}
	else
	{
		if( a[0][0] > a[1][1] && a[0][0] > a[2][2] )
		{
			float s = 2.0f * sqrtf( 1.0f + a[0][0] - a[1][1] - a[2][2]);
			q[0] = 0.25f * s;
			q[1] = (a[0][1] + a[1][0] ) / s;
			q[2] = (a[0][2] + a[2][0] ) / s;
			q[3] = (a[1][2] - a[2][1] ) / s;
		} else if (a[1][1] > a[2][2]) {
      float s = 2.0f * sqrtf( 1.0f + a[1][1] - a[0][0] - a[2][2]);
      q[0] = (a[0][1] + a[1][0] ) / s;
      q[1] = 0.25f * s;
      q[2] = (a[1][2] + a[2][1] ) / s;
      q[3] = (a[0][2] - a[2][0] ) / s;    } else {
      float s = 2.0f * sqrtf( 1.0f + a[2][2] - a[0][0] - a[1][1] );
      q[0] = (a[0][2] + a[2][0] ) / s;
      q[1] = (a[1][2] + a[2][1] ) / s;
      q[2] = 0.25f * s;
      q[3] = (a[0][1] - a[1][0] ) / s;
    }
  }
}


void quat2tform(const float *q, float m[4][3])
{
	float	x = q[1], y = q[2], z = q[3], w = q[0];
	float	tx  = 2.0f*x;
	float	ty  = 2.0f*y;
	float	tz  = 2.0f*z;
	float	twx = tx*w;
	float	twy = ty*w;
	float	twz = tz*w;
	float	txx = tx*x;
	float	txy = ty*x;
	float	txz = tz*x;
	float	tyy = ty*y;
	float	tyz = tz*y;
	float	tzz = tz*z;
	
	m[0][0] = 1-(tyy+tzz);
	m[0][1] = txy-twz;
	m[0][2] = txz+twy;
	m[1][0] = txy+twz;
	m[1][1] = 1-(txx+tzz);
	m[1][2] = tyz-twx;
	m[2][0] = txz-twy;
	m[2][1] = tyz+twx;
	m[2][2] = 1-(txx+tyy);
}

void interpolate_matrix43(float t, float m1[4][3], float m2[4][3], float m3[4][3])
{
}


///////////////////////////////////////////////////////////////////////////////
// maths
///////////////////////////////////////////////////////////////////////////////
/* args must be non-overlapping */
void mult_matrix(float s1[2][2], float s2[2][2], float d[2][2])
{
   d[0][0] = s1[0][0] * s2[0][0] + s1[1][0] * s2[0][1];
   d[1][0] = s1[0][0] * s2[1][0] + s1[1][0] * s2[1][1];
   d[0][1] = s1[0][1] * s2[0][0] + s1[1][1] * s2[0][1];
   d[1][1] = s1[0][1] * s2[1][0] + s1[1][1] * s2[1][1];
}


float det_matrix(float s[2][2])
{
   return s[0][0] * s[1][1] - s[0][1] * s[1][0];
}


void flip_matrix(float m[2][2], int h)
{
   float s, t;
   if (h) {
      /* flip on horizontal axis */
      s = m[0][0];
      t = m[0][1];
      m[0][0] = m[1][0];
      m[0][1] = m[1][1];
      m[1][0] = s;
      m[1][1] = t;
   } else {
      /* flip on vertical axis */
      s = m[0][0];
      t = m[1][0];
      m[0][0] = m[0][1];
      m[1][0] = m[1][1];
      m[0][1] = s;
      m[1][1] = t;
   }
}

void transpose_matrix(float m[2][2])
{
   float t;
   t = m[0][1];
   m[0][1] = m[1][0];
   m[1][0] = t;
}

void choose_evector(float m[3][2], float r, float v[2])
{
   float b = m[0][1];
   float d = m[1][1];
   float x = r - d;
   if (b > EPS) {
      v[0] = x;
      v[1] = b;
   } else if (b < -EPS) {
      v[0] = -x;
      v[1] = -b;
   } else {
      /* XXX */
      v[0] = 1.0;
      v[1] = 0.0;
   }
}


void interpolate_angle(float t, float s, float *v1, float *v2, float *v3, int tie, int cross)
{
   float x = *v1;
   float y = *v2;
   float d;
   static float lastx, lasty;

   // take the shorter way around the circle... 
   if (x > y) {
      d = x - y;
      if (d > M_PI + EPS || (d > M_PI - EPS && tie))
	 y += 2*M_PI;
   } else {
      d = y - x;
      if (d > M_PI + EPS || (d > M_PI - EPS && tie))
	 x += 2*M_PI;
   }
   // unless we are supposed to avoid crossing 
   if (cross) {
      if (lastx > x) {
	 if (lasty < y)
	    y -= 2*M_PI;
      } else {
	 if (lasty > y)
	    y += 2*M_PI;
      }
   } else {
      lastx = x;
      lasty = y;
   }

   *v3 = s * x + t * y;
}

void interpolate_complex(float t, float s, float r1[2], float r2[2], float r3[2],
	int flip, int tie, int cross)
{
   float c1[2], c2[2], c3[2];
   float a1, a2, a3, d1, d2, d3;

   c1[0] = r1[0];
   c1[1] = r1[1];
   c2[0] = r2[0];
   c2[1] = r2[1];

   if (flip) {
      float t = c1[0];
      c1[0] = c1[1];
      c1[1] = t;
      t = c2[0];
      c2[0] = c2[1];
      c2[1] = t;
   }

   // convert to log space 
   a1 = (float)atan2(c1[1], c1[0]);
   a2 = (float)atan2(c2[1], c2[0]);
	d1 = 0.5f * (float)log(c1[0] * c1[0] + c1[1] * c1[1]);
	d2 = 0.5f * (float)log(c2[0] * c2[0] + c2[1] * c2[1]);

   // interpolate linearly
   interpolate_angle(t, s, &a1, &a2, &a3, tie, cross);
   d3 = s * d1 + t * d2;

   // convert back
   d3 = (float)exp(d3);
   c3[0] = (float)cos(a3) * d3;
   c3[1] = (float)sin(a3) * d3;

   if (flip) {
      r3[1] = c3[0];
      r3[0] = c3[1];
   } else {
      r3[0] = c3[0];
      r3[1] = c3[1];
   }
}


void interpolate_matrix(float t, float m1[3][2], float m2[3][2], float m3[3][2])
{
   float s = 1.0f - t;
#if 0
   float r1[2][2], r2[2][2], r3[2][2];
   float v1[2][2], v2[2][2], v3[2][2];
   diagonalize_matrix(m1, r1, v1);
   diagonalize_matrix(m2, r2, v2);

   // handle the evectors 
   interpolate_complex(t, s, v1 + 0, v2 + 0, v3 + 0, 0, 0, 0);
   interpolate_complex(t, s, v1 + 1, v2 + 1, v3 + 1, 0, 0, 1);

   // handle the evalues 
   interpolate_complex(t, s, r1 + 0, r2 + 0, r3 + 0, 0, 0, 0);
   interpolate_complex(t, s, r1 + 1, r2 + 1, r3 + 1, 1, 1, 0);

   undiagonalize_matrix(r3, v3, m3);
#endif
   interpolate_complex(t, s, (float *)(m1 + 0), (float *)(m2 + 0), (float *)(m3 + 0), 0, 0, 0);
   interpolate_complex(t, s, (float *)(m1 + 1), (float *)(m2 + 1), (float *)(m3 + 1), 1, 1, 0);

   // handle the translation part of the xform linearly 
   m3[2][0] = s * m1[2][0] + t * m2[2][0];
   m3[2][1] = s * m1[2][1] + t * m2[2][1];
}





///////////////////////////////////////////////////////////////////////////////
// buffer parse
///////////////////////////////////////////////////////////////////////////////
float readFloat(char *buf, char *value, float def)
{
	float r;
	char b[512];
	char *p = strstr(buf, value);
	if(!p) return def;
	sprintf(b, "%s %%f", value);
	sscanf(p, b, &r);
	return r;
}


int readInt(char *buf, char *value, int def)
{
	int r;
	char b[512];
	char *p = strstr(buf, value);
	if(!p) return def;
	sprintf(b, "%s %%d", value);
	sscanf(p, b, &r);
	return r;
}



}




