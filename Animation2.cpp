#include"IFS.h"
#include<math.h>
#include<float.h>
#include<limits.h>
#include<memory.h>
#include<stdlib.h>


using namespace IFS;


#define INTERP(x)  result->x = c0 * systems[i1].x + c1 * systems[i2].x


// create a control point that interpolates between the control points
// passed in CPS.  for now just do linear.  in the future, add control
// point types and other things to the cps.  CPS must be sorted by time.
void Animation2::interpolate(System2 *result)
{
	int i, j, i1, i2;
	float c0, c1, t;

	// if only one, quit
	if(1 == system_quantity)
	{
		memcpy(result, systems, sizeof(System2));
		return;
	}

	if(systems[0].time >= time)
	{
		i1 = 0;
		i2 = 1;
	}
	else if(systems[system_quantity - 1].time <= time)
	{
		i1 = system_quantity - 2;
		i2 = system_quantity - 1;
	}
	else
	{
		i1 = 0;
		while(systems[i1].time < time) i1++;
		i1--;
		i2 = i1 + 1;
		if(time - systems[i1].time > -1e-7 && time - systems[i1].time < 1e-7)
		{
			memcpy(result, &systems[i1], sizeof(System2));
			return;
		}
	}

   c0 = (systems[i2].time - time) / (systems[i2].time - systems[i1].time);
   c1 = 1.0f - c0;

   result->time = time;

/*   if (cps[i1].cmap_inter) {
     for (i = 0; i < 256; i++) {
       float spread = 0.15;
       float d0, d1, e0, e1, c = 2 * M_PI * i / 256.0;
       c = cos(c * cps[i1].cmap_inter) + 4.0 * c1 - 2.0;
       if (c >  spread) c =  spread;
       if (c < -spread) c = -spread;
       d1 = (c + spread) * 0.5 / spread;
       d0 = 1.0 - d1;
       e0 = (d0 < 0.5) ? (d0 * 2) : (d1 * 2);
       e1 = 1.0 - e0;
       for (j = 0; j < 3; j++) {
	 result->cmap[i][j] = (d0 * cps[i1].cmap[i][j] +
			       d1 * cps[i2].cmap[i][j]);
#define bright_peak 2.0
#if 0
	 if (d0 < 0.5)
	   result->cmap[i][j] *= 1.0 + bright_peak * d0;
	 else
	   result->cmap[i][j] *= 1.0 + bright_peak * d1;
#else
	 result->cmap[i][j] = (e1 * result->cmap[i][j] +
			       e0 * 1.0);
#endif
       }
     }
   } else*/ {
     for(i = 0; i < 256; i++) {
       float t[3], s[3];
       ColorMap::rgb2hsv(systems[i1].colormap.data[i], s);
       ColorMap::rgb2hsv(systems[i2].colormap.data[i], t);
       for (j = 0; j < 3; j++)
	 t[j] = c0 * s[j] + c1 * t[j];
       ColorMap::hsv2rgb(t, result->colormap.data[i]);
     }
   }

	result->colormap_index = -1;
	INTERP(hue_rotation);
	INTERP(scale);
	INTERP(alpha);
	INTERP(radius);
	INTERP(center[0]);
	INTERP(center[1]);
	INTERP(point_quantity);
	INTERP(wiggle_amount);
	INTERP(wiggle_speed);

	// chew on xforms
	for(i = 0; i < NXFORMS; i++)
	{
		INTERP(xforms[i].density);
		INTERP(xforms[i].color);

		// interpolate variations
		for(j = 0; j < NVARS; j++) INTERP(xforms[i].var[j]);
		t = 0.0f;
		for(j = 0; j < NVARS; j++) t += result->xforms[i].var[j];
		t = 1.0f / t;
		for(j = 0; j < NVARS; j++) result->xforms[i].var[j] *= t;

		// interpolate the coefficents
		interpolate_matrix(((((float)sin((c1-0.5)*M_PI)*0.5f+0.5f)+c1)/2.0f), systems[i1].xforms[i].c, systems[i2].xforms[i].c,
			result->xforms[i].c);
	}

	 // apply wiggle
	 float st = result->wiggle_speed * time, ct;
	 ct = ((float)cos(st))*result->wiggle_amount;
	 st = ((float)sin(st))*result->wiggle_amount;
	 for(i = 0; i < NXFORMS; i++)
	 {
	    result->xforms[i].c[0][0] += ct;
	    result->xforms[i].c[1][0] -= st;
	    result->xforms[i].c[0][1] += st;
	    result->xforms[i].c[1][1] += ct;
	 }
}

/*
class Spline
{
private:
	int		size;
public:
	float	*xs, *ys;
	int		n;

	void pushKey(float x, float y)
	{
		if(n+1 >= size)
		{
			float *tmpx = new float[size+1];
			float *tmpy = new float[size+1];
			memcpy(tmpx, xs, size*sizeof(float));
			memcpy(tmpy, ys, size*sizeof(float));
			delete[] xs;
			delete[] ys;
			xs = tmpx;
			ys = tmpy;
			size++;
		}
		xs[n] = x;
		ys[n] = y;
		n++;
	}
	void clear()
	{
		n = 0;
	}
	void sort()
	{
		for(int j=0; j < n; j++)
		{
			int xmin = j;
			for(int i=j+1; i < n; i++)
			{
				if(xs[xmin] > xs[i]) xmin = i;
			}
			if(xmin != j)
			{
				float tx=xs[j], ty=ys[j];
				xs[j]=xs[xmin]; ys[j]=ys[xmin];
				xs[xmin]=tx; ys[xmin]=ty;
			}
		}
	}
	float interpolate(float x)
	{
		float d1,d0,dd1,dd0,a,b,c,d,x0,x1,y0,y1,x2,y2,x_1,y_1;
		int i;

		if(n == 0) return 0;
		if(n == 1 || x <= xs[0]) return ys[0];
		if(x >= xs[n-1]) return ys[n-1];
		if(n == 2) return x*(ys[1]-ys[0])/xs[1]+ys[0];


		// search current index
		for(i = 0; i < n-1; i++)
		{
			if(x <= xs[i+1]) break;
		}*/
/*
		// calculate
		x0 = xs[i];
		y0 = ys[i];
		x1 = xs[i+1];
		y1 = ys[i+1];
		if(i == 0)
		{
			//return (y1-y0)*((x-x0)/(x1-x0))+y0;
			x2 = xs[i+2];
			y2 = ys[i+2];
			d1 = 2.0f / (((x2-x1) / (y2-y1)) + ((x1-x0) / (y1-y0)));
			if(d1 >= FLT_MAX || d1 <= FLT_MIN) d1 = 0;
			d0 = ((3.0f*(y1-y0))/(2.0f*(x1-x0))) - (d1/2.0f);
			if(d0 >= FLT_MAX || d0 <= FLT_MIN) d0 = 0;
			if(d1*d0 < 0)
			{
				//d0 = 0;
				//d1 = 0;
			}
		}
		else if(i >= n-2)
		{
			//return (y1-y0)*((x-x0)/(x1-x0))+y0;
			x_1 = xs[i-1];
			y_1 = ys[i-1];
			d0 = 2 / (((x1-x0) / (y1-y0)) + ((x0-x_1) / (y0-y_1)));
			if(d0 >= FLT_MAX || d0 <= FLT_MIN) d0 = 0;
			d1 = ((3.0f*(y1-y0))/(2.0f*(x1-x0))) - (d0/2.0f);
			if(d1 >= FLT_MAX || d1 <= FLT_MIN) d1 = 0;
			if(d1*d0 < 0)
			{
				//d0 = 0;
				//d1 = 0;
			}
		}
		else
		{
			x_1 = xs[i-1];
			y_1 = ys[i-1];
			x2 = xs[i+2];
			y2 = ys[i+2];
			d1 = 2 / (((x2-x1) / (y2-y1)) + ((x1-x0) / (y1-y0)));
			if(d1 >= FLT_MAX || d1 <= FLT_MIN) d1 = 0;
			d0 = 2 / (((x1-x0) / (y1-y0)) + ((x0-x_1) / (y0-y_1)));
			if(d0 >= FLT_MAX || d0 <= FLT_MIN) d0 = 0;

			if(d1*d0 < 0)
			{
				//d0 = 0;
				//d1 = 0;
			}
		}
		dd0 = -2*((d1+2*d0)/(x1-x0)) + 6*((y1-y0)/((x1-x0)*(x1-x0)));
		if(dd0 >= FLT_MAX) dd0 = 0;
		dd1 = 2*((2*d1+d0)/(x1-x0)) - 6*((y1-y0)/((x1-x0)*(x1-x0)));
		if(dd1 >= FLT_MAX) dd1 = 0;
		d = (dd1-dd0)/(6*(x1-x0));
		c = (x1*dd0-x0*dd1)/(2*(x1-x0));
		b = ((y1-y0) - c*(x1*x1-x0*x0) - d*(x1*x1*x1-x0*x0*x0)) / (x1-x0);
		a = y0 - b*x0 - c*x0*x0 - d*x0*x0*x0;
		return a + b*x + c*x*x + d*x*x*x;
		*//*
		float dx = x-xs[i];
		return d + c*dx + d*dx*dx + a*dx*dx*dx;
	}
	Spline()
	{
		memset(this,0,sizeof(Spline));
	}
};

void Animation2::interpolateSpline(System2 *result)
{
	static Spline s_center[2];
	static Spline s_c[3][2];
	int i, j, i1, i2;
	float c0, c1, t;

	// if only one, quit
	if(1 == system_quantity)
	{
		memcpy(result, systems, sizeof(System2));
		return;
	}

	if(systems[0].time >= time)
	{
		i1 = 0;
		i2 = 1;
	}
	else if(systems[system_quantity - 1].time <= time)
	{
		i1 = system_quantity - 2;
		i2 = system_quantity - 1;
	}
	else
	{
		i1 = 0;
		while(systems[i1].time < time) i1++;
		i1--;
		i2 = i1 + 1;
		if(time - systems[i1].time > -1e-7 && time - systems[i1].time < 1e-7)
		{
			memcpy(result, &systems[i1], sizeof(System2));
			return;
		}
	}

   c0 = (systems[i2].time - time) / (systems[i2].time - systems[i1].time);
   c1 = 1.0f - c0;

   result->time = time;

	s_center[0].clear();
	s_center[1].clear();

	for(i=0; i < system_quantity; i++)
	{
		s_center[0].pushKey(systems[i].time, systems[i].center[0]);
		s_center[1].pushKey(systems[i].time, systems[i].center[1]);
	}
*/
/*   if (cps[i1].cmap_inter) {
     for (i = 0; i < 256; i++) {
       float spread = 0.15;
       float d0, d1, e0, e1, c = 2 * M_PI * i / 256.0;
       c = cos(c * cps[i1].cmap_inter) + 4.0 * c1 - 2.0;
       if (c >  spread) c =  spread;
       if (c < -spread) c = -spread;
       d1 = (c + spread) * 0.5 / spread;
       d0 = 1.0 - d1;
       e0 = (d0 < 0.5) ? (d0 * 2) : (d1 * 2);
       e1 = 1.0 - e0;
       for (j = 0; j < 3; j++) {
	 result->cmap[i][j] = (d0 * cps[i1].cmap[i][j] +
			       d1 * cps[i2].cmap[i][j]);
#define bright_peak 2.0
#if 0
	 if (d0 < 0.5)
	   result->cmap[i][j] *= 1.0 + bright_peak * d0;
	 else
	   result->cmap[i][j] *= 1.0 + bright_peak * d1;
#else
	 result->cmap[i][j] = (e1 * result->cmap[i][j] +
			       e0 * 1.0);
#endif
       }
     }
   } else*/ /*{
     for(i = 0; i < 256; i++) {
       float t[3], s[3];
       ColorMap::rgb2hsv(systems[i1].colormap.data[i], s);
       ColorMap::rgb2hsv(systems[i2].colormap.data[i], t);
       for (j = 0; j < 3; j++)
	 t[j] = c0 * s[j] + c1 * t[j];
       ColorMap::hsv2rgb(t, result->colormap.data[i]);
     }
   }

	result->colormap_index = -1;
	INTERP(hue_rotation);
	INTERP(scale);
	INTERP(alpha);
	INTERP(radius);
	result->center[0] = s_center[0].interpolate(time);
	result->center[1] = s_center[1].interpolate(time);
	INTERP(point_quantity);

	for(i = 0; i < NXFORMS; i++)
	{
		INTERP(xforms[i].density);
		INTERP(xforms[i].color);
		for(j = 0; j < NVARS; j++) INTERP(xforms[i].var[j]);
		t = 0.0f;
		for (j = 0; j < NVARS; j++) t += result->xforms[i].var[j];
		t = 1.0f / t;
		for (j = 0; j < NVARS; j++) result->xforms[i].var[j] *= t;

		//interpolate_matrix(c1, systems[i1].xforms[i].c, systems[i2].xforms[i].c,
		//	result->xforms[i].c);
		s_c[0][0].clear();
		s_c[0][1].clear();
		s_c[1][0].clear();
		s_c[1][1].clear();
		s_c[2][0].clear();
		s_c[2][1].clear();
		for(j=0; j < system_quantity; j++)
		{
			s_c[0][0].pushKey(systems[j].time, systems[j].xforms[i].c[0][0]);
			s_c[0][1].pushKey(systems[j].time, systems[j].xforms[i].c[0][1]);
			s_c[1][0].pushKey(systems[j].time, systems[j].xforms[i].c[1][0]);
			s_c[1][1].pushKey(systems[j].time, systems[j].xforms[i].c[1][1]);
			s_c[2][0].pushKey(systems[j].time, systems[j].xforms[i].c[2][0]);
			s_c[2][1].pushKey(systems[j].time, systems[j].xforms[i].c[2][1]);
		}
		result->xforms[i].c[0][0] = s_c[0][0].interpolate(time);
		result->xforms[i].c[0][1] = s_c[0][1].interpolate(time);
		result->xforms[i].c[1][0] = s_c[1][0].interpolate(time);
		result->xforms[i].c[1][1] = s_c[1][1].interpolate(time);
		result->xforms[i].c[2][0] = s_c[2][0].interpolate(time);
		result->xforms[i].c[2][1] = s_c[2][1].interpolate(time);
	}
}
*/

void Animation2::renderFlame()
{
	System2 res;
	interpolate(&res);
	res.renderFlame();
}


void Animation2::renderFlameLinear()
{
	System2 res;
	interpolate(&res);
	res.renderFlameLinear();
}


void Animation2::sort()
{
	for(int j=0; j < system_quantity; j++)
	{
		int sysmin = j;
		for(int i=j+1; i < system_quantity; i++)
		{
			if(systems[sysmin].time > systems[i].time) sysmin = i;
		}
		if(sysmin != j)
		{
			System2 tmp;
			memcpy(&tmp, &systems[j], sizeof(System2));
			memcpy(&systems[j], &systems[sysmin], sizeof(System2));
			memcpy(&systems[sysmin], &tmp, sizeof(System2));
		}
	}
}


void Animation2::deleteSystem(int n)
{
	if(system_quantity == 1)
	{
		delete[] systems;
		system_quantity = 0;
		return;
	}
	if(n >= system_quantity) n = system_quantity-1;
	else if(n < 0) n = 0;

	System2 *s = new System2[system_quantity-1];
	if(n > 0) memcpy(s, systems, n*sizeof(System2));
	if(n < system_quantity-1) memcpy(s+n, systems+n+1,
		(system_quantity-n-1)*sizeof(System2));
	system_quantity--;
	delete[] systems;
	systems = s;

	// fix time
	sort();
	if(n == 0)
	{
		float t = systems[0].time;
		for(int i=0; i < system_quantity; i++)
		{
			systems[i].time -= t;
		}
	}
}


void Animation2::insertSystem(int n, const System2* sys)
{
	System2 *s = new System2[system_quantity+1];
	memcpy(s, systems, (n+1)*sizeof(System2));
	if(n < system_quantity-1)
		memcpy(s+n+2, systems+n+1,
			(system_quantity-n-1)*sizeof(System2));
	memcpy(&s[n+1], sys, sizeof(System2));
	delete[] systems;
	systems = s;
	system_quantity++;
}


Animation2::Animation2()
{
	memset(this, 0, sizeof(Animation2));
}

#if 0

bool Animation2::loadFLA(char *fn)
{
	int i;
	System2 tmp;

	delete[] systems;
	system_quantity = 0;
	
	while(tmp.loadFLA(fn, system_quantity)) system_quantity++;
	if(!system_quantity) return 0;

	systems = new System2[system_quantity];

	for(i=0; i < system_quantity; i++) systems[i].loadFLA(fn, i);
	return 1;
}

#endif

bool Animation2::loadFS2(char *fn)
{
	int i;
	System2 tmp;

	time = 0;

	delete[] systems;
	system_quantity = 0;

	while(tmp.loadFS2(fn, system_quantity)) system_quantity++;
	if(!system_quantity) return 0;

	systems = new System2[system_quantity];

	for(i=0; i < system_quantity; i++) systems[i].loadFS2(fn, i);
	sort();
	return 1;
}

#if 0

bool Animation2::saveFS2(char *fn)
{
	FILE *f = fopen(fn,"wb");
	if(!f) return false;
	for(int i=0; i < system_quantity; i++)
		systems[i].appendFS2(f);
	fclose(f);
	return true;
}

#endif