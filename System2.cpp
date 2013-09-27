

#include "plattis.h"
#include"IFS.h"

/*
#include<time.h>
#include<math.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<stdarg.h>
#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include<windows.h>
#endif
#include<GL\gl.h>
#include<GL\glu.h>
*/
#define vlen(x) (sizeof(x)/sizeof(*x))
#define random_distrib(v) ((v)[rand()%vlen(v)])
#define SUB_BATCH_SIZE 1000
#define FUSE 15
#ifndef M_PI
#define M_PI 3.1415926539f
#endif
#define CHOOSE_XFORM_GRAIN (1<<14)
#define coef   xforms[fn].c
#define vari   xforms[fn].var


#define POINTVERTEX_FVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)
struct POINTVERTEX
{
    D3DXVECTOR3 v;
    D3DCOLOR    color;
};


#define WRAP(x) {\
	HRESULT hresult=x; \
	ASSERT(SUCCEEDED(hresult)); \
}


extern LPDIRECT3DVERTEXBUFFER8 ifs_vb; // Buffer to hold vertices

int m_dwFlush=SUB_BATCH_SIZE;
int m_dwDiscard=SUB_BATCH_SIZE*4;
int m_dwBase=SUB_BATCH_SIZE*4;

using namespace IFS;

#if 0
/*#define BEGIN_DRAW(center0, center1, scale) \
glBlendFunc(GL_SRC_ALPHA, GL_ONE); \
glLoadIdentity(); \
glTranslatef(center0,center1,0); \
glScalef(scale,scale,1); \
glBegin(GL_TRIANGLES)*/

#define BEGIN_DRAW(center0, center1, scale) \
glBlendFunc(GL_SRC_ALPHA, GL_ONE); \
glLoadIdentity(); \
glTranslatef(center0,center1,0); \
glScalef(scale,scale,1); \
glDisable(GL_TEXTURE_2D); \
glBegin(GL_POINTS)


/*#define DRAW_DOT(x,y,radius,r,g,b,a) \
glColor4f((r), (g), (b), (a)); \
glTexCoord2f(0.5f,0); \
glVertex3f(x,y+radius,1); \
glTexCoord2f(1,1); \
glVertex3f(x+radius,y-radius,1); \
glTexCoord2f(0,1); \
glVertex3f(x-radius,y-radius,1)*/
#define DRAW_DOT(x,y,radius,r,g,b,a) \
glColor4f((r), (g), (b), (a)); \
glVertex3f(x,y,1)


/*#define END_DRAW() \
glEnd()*/
#endif

#define BEGIN_DRAW(center0, center1, scale) \
{ \
	printf("%4.2f 4.2f \n",center0, center1); \
	D3DXMATRIX matWorld;\
	D3DXMatrixIdentity( &matWorld );\
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );\
	D3DXMatrixTranslation(&matWorld, center0, center1, 0);\
	g_pd3dDevice->MultiplyTransform(D3DTS_WORLD, &matWorld);\
	D3DXMatrixScaling(&matWorld, scale, scale, 1);\
	g_pd3dDevice->MultiplyTransform(D3DTS_WORLD, &matWorld);\
}

#define DRAW_DOT(x,y,radius,r,g,b,a)

#define xDRAW_DOT(x,y,radius,r,g,b,a) \
{ \
	float f[4]; \
	f[0]=x; \
	f[1]=y; \
	f[2]=0; \
	*((int*)&f[3])=((int)(a*255))<<24 | 0x0f0f0f; \
	g_pd3dDevice->DrawPrimitiveUP( D3DPT_POINTLIST, 1, f, 4*4); \
}


#define END_DRAW()

System2::System2()
{
	srand(12345678);
	memset(this, 0, sizeof(System2));
	wiggle_speed = 1;
}

#if 0

void System2::appendFLA(FILE *f)
{
	fprintf(f,
		"FLA-%x {\n"
		" cmap %d\n",
		::time(0), ((colormap_index==-1)? rand()%50:colormap_index));

	for(int i=0; i < NXFORMS; i++)
	{
		if(xforms[i].density > 0)
		{
			fprintf(f,
				" xform %d density %f color %f\n"
				" var %f %f %f %f %f %f %f\n"
				" coefs %f %f %f %f %f %f\n",
				i, xforms[i].density, xforms[i].color,
				xforms[i].var[0], xforms[i].var[1], xforms[i].var[2],
				xforms[i].var[3], xforms[i].var[4], xforms[i].var[5],
				xforms[i].var[6],
				xforms[i].c[0][0], xforms[i].c[0][1], xforms[i].c[1][0],
				xforms[i].c[1][1], xforms[i].c[2][0], xforms[i].c[2][1]);
		}
	}
	fputs("}\n\n",f);
}

bool System2::saveFLA(char *fn)
{
	FILE *f = fopen(fn,"wt");
	if(!f) return false;
	fputs("\n",f);
	appendFLA(f);
	fclose(f);
	return true;
}

bool System2::loadFLA(char *fn, int index)
{
	int j;
	FILE *f = fopen(fn,"rt");
	char *buf2, *p;
	unsigned int d;

	if(!f) return 0;

	fseek(f, 0, SEEK_END);
	d = ftell(f);
	rewind(f);
	buf2 = (char*)malloc(d);
	fread(buf2, d, 1, f);
	fclose(f);

	p = buf2;
	for(j=0; j < index+1; j++)
	{
		p++;
		p = strchr(p, '}');
		if(!p) return 0;
	}
	*p=0;
	p = buf2;
	for(j=0; j < index+1; j++)
	{
		p+=strlen("cmap");
		p = strstr(p, "cmap");
		if(!p) return 0;
	}
	sscanf(p, "cmap %d", &colormap_index);
	for(j=0; j < NXFORMS; j++)
	{
		int i;
		char *buf = p;

		while(1)
		{
			buf = strstr(buf, "xform");

			if(buf == 0)
			{
				xforms[j].density = 0;
				break;
			}
			sscanf(buf, "xform %d", &i);
			buf+=strlen("xform");
			if(i == j)
			{
				buf = strstr(buf, "density");
				sscanf(buf, "density %f", &xforms[i].density);
				buf = strstr(buf, "color");
				sscanf(buf, "color %f", &xforms[i].color);
				buf = strstr(buf, "var");
				sscanf(buf, "var %f %f %f %f %f %f %f", &xforms[i].var[0],
					&xforms[i].var[1], &xforms[i].var[2], &xforms[i].var[3],
		  			&xforms[i].var[4], &xforms[i].var[5], &xforms[i].var[6]);
				buf = strstr(buf, "coefs");
				sscanf(buf, "coefs %f %f %f %f %f %f", &xforms[i].c[0][0],
					&xforms[i].c[0][1], &xforms[i].c[1][0], &xforms[i].c[1][1],
					&xforms[i].c[2][0], &xforms[i].c[2][1]);
				break;
			}
		}
	}

	hue_rotation = 0;
	if(colormap_index >= 0) colormap.loadPreset(colormap_index, hue_rotation);
	else colormap.loadRandomPreset(hue_rotation);

	delete[] buf2;
	return 1;
}


void System2::appendFS2(FILE *f)
{
	fputs("FS2S",f);
	fwrite(this, 1, sizeof(System2), f);
	/*
	fprintf(f,
		"FS2-%x {\n"
		" colormap %d hue_rotation %f\n"
		" point_quantity %u alpha %f radius %f\n"
		" scale %f center %f,%f\n"
		" time %f\n",
		::time(0), colormap_index, hue_rotation,
		point_quantity, alpha, radius,
		scale, center[0], center[1], time);
	fprintf(f, " wiggle_amount %f wiggle_speed %f\n",
		wiggle_amount, wiggle_speed);

	for(int i=0; i < NXFORMS; i++)
	{
		if(xforms[i].density > 0)
		{
			fprintf(f,
				" xform %d density %.9f color %f\n"
				" var %f %f %f %f %f %f %f\n"
				" coefs %f %f %f %f %f %f\n",
				i, xforms[i].density, xforms[i].color,
				xforms[i].var[0], xforms[i].var[1], xforms[i].var[2],
				xforms[i].var[3], xforms[i].var[4], xforms[i].var[5],
				xforms[i].var[6],
				xforms[i].c[0][0], xforms[i].c[0][1], xforms[i].c[1][0],
				xforms[i].c[1][1], xforms[i].c[2][0], xforms[i].c[2][1]);
		}
	}
	fputs("}\n\n",f);
	*/
}

bool System2::saveFS2(char *fn)
{
	//FILE *f = fopen(fn,"wt");
	FILE *f = fopen(fn,"wb");
	if(!f) return false;
	appendFS2(f);
	fclose(f);
	return true;
}

#endif


bool System2::loadFS2(char *fn, int index)
{
	FILE *f = fopen(fn,"rb");
	if(!f) return 0;
	fseek(f, 0, SEEK_END);
	if(index*(4+sizeof(System2))+4 >= ftell(f))
	{
		fclose(f);
		return 0;
	}
	fseek(f, index*(4+sizeof(System2))+4, SEEK_SET);
	fread(this, 1, sizeof(System2), f);
	fclose(f);

	if(colormap_index >= 0) colormap.loadPreset(colormap_index, hue_rotation);
	else colormap.loadRandomPreset(hue_rotation);
	return 1;

/*	int j;
	FILE *f = fopen(fn,"rt");
	char *buf2, *p;
	unsigned int d;

	if(!f) return 0;

	fseek(f, 0, SEEK_END);
	d = ftell(f);
	rewind(f);
	buf2 = (char*)malloc(d);
	fread(buf2, d, 1, f);
	fclose(f);

	p = buf2;
	for(j=0; j < index+1; j++)
	{
		p++;
		p = strchr(p, '}');
		if(!p) return 0;
	}
	*p=0;
	p = buf2;
	for(j=0; j < index+1; j++)
	{
		p+=strlen("colormap");
		p = strstr(p, "colormap");
		if(!p) return 0;
	}

	colormap_index = readInt(p, "colormap", 0);
	hue_rotation = readFloat(p, "hue_rotation", 0);
	point_quantity = readInt(p, "point_quantity", 5000);
	alpha = readFloat(p, "alpha", 0.1f);
	radius = readFloat(p, "radius", 0.02f);
	scale = readFloat(p, "scale", 1);
	wiggle_amount = readFloat(p, "wiggle_amount", 0);
	wiggle_speed = readFloat(p, "wiggle_speed", 1);
	time = readFloat(p, "time", 0);
	p = strstr(p, "center");
	sscanf(p, "center %f,%f\n", &center[0], &center[1]);

	for(j=0; j < NXFORMS; j++)
	{
		int i;
		char *buf = p;

		while(1)
		{
			buf = strstr(buf, "xform");

			if(buf == 0)
			{
				xforms[j].density = 0;
				break;
			}
			sscanf(buf, "xform %d", &i);
			buf+=strlen("xform");
			if(i == j)
			{
				buf = strstr(buf, "density");
				sscanf(buf, "density %f", &xforms[i].density);
				buf = strstr(buf, "color");
				sscanf(buf, "color %f", &xforms[i].color);
				buf = strstr(buf, "var");
				sscanf(buf, "var %f %f %f %f %f %f %f", &xforms[i].var[0],
					&xforms[i].var[1], &xforms[i].var[2], &xforms[i].var[3],
		  			&xforms[i].var[4], &xforms[i].var[5], &xforms[i].var[6]);
				buf = strstr(buf, "coefs");
				sscanf(buf, "coefs %f %f %f %f %f %f", &xforms[i].c[0][0],
					&xforms[i].c[0][1], &xforms[i].c[1][0], &xforms[i].c[1][1],
					&xforms[i].c[2][0], &xforms[i].c[2][1]);
				break;
			}
		}
	}

	if(colormap_index >= 0) colormap.loadPreset(colormap_index, hue_rotation);
	else colormap.loadRandomPreset(hue_rotation);

	delete[] buf2;
	return 1;
	*/
}


void System2::randomize()
{
	int i, nxforms, var;
	static int xform_distrib[] ={
		2, 2, 2,
		3, 3, 3,
		4, 4,
		5};
	static int var_distrib[] = {
		-1, -1, -1,
		0, 0, 0, 0,
		1, 1, 1,
		2, 2, 2,
		3, 3,
		4, 4,
		5};

	static int mixed_var_distrib[] = {
		0, 0, 0,
		1, 1, 1,
		2, 2, 2,
		3, 3,
		4, 4,
		5, 5};

	hue_rotation = (rand()/(float)RAND_MAX);
	colormap_index = colormap.loadRandomPreset(hue_rotation);
	nxforms = random_distrib(xform_distrib);
	var = random_distrib(var_distrib);
	for(i = 0; i < nxforms; i++)
	{
		int j, k;
		xforms[i].density = 1.0f / nxforms;
		xforms[i].color = i == 0;
		for (j = 0; j < 3; j++)
			for (k = 0; k < 2; k++)
				xforms[i].c[j][k] = ((rand() - RAND_MAX/2)/(float)(RAND_MAX/2));
		for (j = 0; j < NVARS; j++)
			xforms[i].var[j] = 0.0;
		if (var >= 0)
			xforms[i].var[var] = 1.0;
		else
			xforms[i].var[random_distrib(mixed_var_distrib)] = 1.0;
	}

	for(; i < NXFORMS; i++)
		xforms[i].density = 0.0;
}

void System2::renderFlameLinear()
{
}




void System2::renderFlame()
{
	unsigned int			i, j;
	unsigned int			sub_batch;
	double					t, r, dr;
	static unsigned char	xform_distrib[CHOOSE_XFORM_GRAIN];

	dr = 0.0f;
	for (i = 0; i < NXFORMS; i++) dr += xforms[i].density;
	dr = dr / CHOOSE_XFORM_GRAIN;

	j = 0;
	t = xforms[0].density;
	r = 0.0f;
	for(i = 0; i < CHOOSE_XFORM_GRAIN; i++)
	{
		while (r >= t)
		{
			j++;
			t += xforms[j].density;
		}
		xform_distrib[i] = j;
		r += dr;
	}

	DWORD palette[256];

	ColorMap tmpcmap;
	for(i=0; i < 256; i++)
	{
		float tc[4];
		ColorMap::rgb2hsv(colormap.data[i],tc);
		tc[0] += hue_rotation*6;
		ColorMap::hsv2rgb(tc,tmpcmap.data[i]);

		palette[i]=
			((int)(alpha*105))<<24 | 
			((int)(tmpcmap.data[i][0]*255))<<16 |
			((int)(tmpcmap.data[i][1]*255))<<8 |
			((int)(tmpcmap.data[i][2]*255));
	}

	dr = radius;
	BEGIN_DRAW(center[0],center[1],scale);

/*------------------------*/
    POINTVERTEX* pVertices;
    DWORD        dwNumParticlesToRender = 0;

	// Set up the vertex buffer to be rendered
    g_pd3dDevice->SetStreamSource( 0, ifs_vb, sizeof(POINTVERTEX) );
    g_pd3dDevice->SetVertexShader( POINTVERTEX_FVF );

	m_dwBase += m_dwFlush;
	if(m_dwBase >= m_dwDiscard) m_dwBase = 0;

#define D3DLOCK_DISCARD 0

	WRAP(ifs_vb->Lock( m_dwBase * sizeof(POINTVERTEX), m_dwFlush * sizeof(POINTVERTEX),
		(BYTE**)&pVertices, m_dwBase ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD ));

    POINTVERTEX* p=pVertices;
//				printf("p pVertices[m_dwBase]\n");
//	printf("debug %d %d ... %d/%d\n",m_dwBase, dwNumParticlesToRender, m_dwFlush, m_dwDiscard);
/*------------------------*/


	for(sub_batch = 0; sub_batch < point_quantity; sub_batch += SUB_BATCH_SIZE*5)
	{
		register float p0 = ((rand() - RAND_MAX/2)/(float)(RAND_MAX/2));//random_uniform11();
		register float p1 = ((rand() - RAND_MAX/2)/(float)(RAND_MAX/2));//random_uniform11();
		register float p2 = (rand()/(float)RAND_MAX);//random_uniform01();
		i = FUSE;
		do
		{
			int fn = xform_distrib[rand() % CHOOSE_XFORM_GRAIN];
			float tx, ty, v;

			p2 = (p2+xforms[fn].color)*0.5f;

			tx = coef[0][0] * p0 + coef[1][0] * p1 + coef[2][0];
			ty = coef[0][1] * p0 + coef[1][1] * p1 + coef[2][1];

			p0 = p1 = 0;

			v = vari[0];
			if(v > 0)
			{
				// linear 
				p0 += v * tx;
				p1 += v * ty;
			}
      
			v = vari[1];
			if(v > 0)
			{
				// sinusoidal
				float nx, ny;
				nx = (float)sin(tx);
				ny = (float)sin(ty);
				p0 += v * nx;
				p1 += v * ny;
			}
      
			v = vari[2];
			if(v > 0)
			{
				// complex
				float nx, ny;
				float r2 = 1.0f/(tx * tx + ty * ty + 1e-6f);
				nx = tx * r2;
				ny = ty * r2;
				p0 += v * nx;
				p1 += v * ny;
			}

			v = vari[3];
			if(v > 0)
			{
				// swirl 
				float r2 = tx * tx + ty * ty;  // /k here is fun 
				float c1 = (float)sin(r2);
				float c2 = (float)cos(r2);
				float nx = c1 * tx - c2 * ty;
				float ny = c2 * tx + c1 * ty;
				p0 += v * nx;
				p1 += v * ny;
			}
      
			v = vari[4];
			if(v > 0)
			{
				// horseshoe 
				float a, c1, c2, nx, ny;
				if(tx < -EPS || tx > EPS || ty < -EPS || ty > EPS)
					a = (float)atan2(tx, ty);  // times k here is fun
				else a = 0;
				c1 = (float)sin(a);
				c2 = (float)cos(a);
				nx = c1 * tx - c2 * ty;
				ny = c2 * tx + c1 * ty;
				p0 += v * nx;
				p1 += v * ny;
			}

			v = vari[5];
			if(v > 0)
			{
				// spherical
				float nx, ny;
				if(tx < -EPS || tx > EPS || ty < -EPS || ty > EPS)
					nx = (float)atan2(tx, ty) / M_PI;
				else nx = 0;
				ny = (float)sqrt(tx * tx + ty * ty) - 1;
				p0 += v * nx;
				p1 += v * ny;
			}

			v = vari[6];
			if(v > 0)
			{
				// bent 
				float nx, ny;
				nx = tx;
				ny = ty;
				if(nx < 0) nx *= 2.0f;
				if(ny < 0) ny *= 0.5f;
				p0 += v * nx;
				p1 += v * ny;
			}
		} while(--i);

		i = SUB_BATCH_SIZE;
		do
		{
			int fn = xform_distrib[rand() % CHOOSE_XFORM_GRAIN];
			float tx, ty, v;

			p2 = (p2+xforms[fn].color)*0.5f;

			tx = coef[0][0] * p0 + coef[1][0] * p1 + coef[2][0];
			ty = coef[0][1] * p0 + coef[1][1] * p1 + coef[2][1];

			p0 = p1 = 0;

			v = vari[0];
			if(v > 0)
			{
				// linear 
				p0 += v * tx;
				p1 += v * ty;
			}

			v = vari[1];
			if(v > 0)
			{
				// sinusoidal
				p0 += v * (float)sin(tx);
				p1 += v * (float)sin(ty);
			}
      
			v = vari[2];
			if(v > 0)
			{
				// complex
				float nx, ny;
				float r2 = 1.0f/(tx * tx + ty * ty + 1e-6f);
				nx = tx * r2;
				ny = ty * r2;
				p0 += v * nx;
				p1 += v * ny;
			}

			v = vari[3];
			if(v > 0)
			{
				// swirl 
				float r2 = tx * tx + ty * ty;  // /k here is fun 
				float c1 = (float)sin(r2);
				float c2 = (float)cos(r2);
				float nx = c1 * tx - c2 * ty;
				float ny = c2 * tx + c1 * ty;
				p0 += v * nx;
				p1 += v * ny;
			}
      
			v = vari[4];
			if(v > 0)
			{
				// horseshoe 
				float a, c1, c2, nx, ny;
				if(tx < -EPS || tx > EPS || ty < -EPS || ty > EPS)
					a = (float)atan2(tx, ty);  // times k here is fun
				else a = 0;
				c1 = (float)sin(a);
				c2 = (float)cos(a);
				nx = c1 * tx - c2 * ty;
				ny = c2 * tx + c1 * ty;
				p0 += v * nx;
				p1 += v * ny;
			}

			v = vari[5];
			if(v > 0)
			{
				// spherical
				float nx, ny;
				if(tx < -EPS || tx > EPS || ty < -EPS || ty > EPS)
					nx = (float)atan2(tx, ty) * (1 / M_PI);
				else nx = 0;
				ny = (float)sqrt(tx * tx + ty * ty) - 1;
				p0 += v * nx;
				p1 += v * ny;
			}

			v = vari[6];
			if(v > 0)
			{
				// bent 
				float nx, ny;
				nx = tx;
				ny = ty;
				if (nx < 0) nx *= 2.0f;
				if (ny < 0) ny *= 0.5f;
				p0 += v * nx;
				p1 += v * ny;
			}

			int k = 255&(int)(p2*256.0f);
/*			if(k < 0) k = 0;
			else if(k > 255) k = 255;
*/
/*			DRAW_DOT(p0,p1,dr,
				tmpcmap.data[k][0], tmpcmap.data[k][1], tmpcmap.data[k][2], alpha);
*/

			/*
			{ \
				float f[4];
				f[0]=p0;
				f[1]=p1;
				f[2]=0;
				*((int*)&f[3])=palette[k];
				g_pd3dDevice->DrawPrimitiveUP( D3DPT_POINTLIST, 1, f, 4*4);
			}
*/
//			printf("\t%d\n", dwNumParticlesToRender);
			p->v.x=p0;
			p->v.y=p1;
//			p->v.z=0;
			p->color=palette[k];
			p++;

			//dwNumParticlesToRender++;
            if( ++dwNumParticlesToRender == m_dwFlush )
            {
                // Done filling this chunk of the vertex buffer.  Lets unlock and
                // draw this portion so we can begin filling the next chunk.

                ifs_vb->Unlock();

//				printf("draw %d %d ... %d/%d\n",m_dwBase, dwNumParticlesToRender, m_dwFlush, m_dwDiscard);
                WRAP(g_pd3dDevice->DrawPrimitive( D3DPT_POINTLIST, m_dwBase, dwNumParticlesToRender));

                // Lock the next chunk of the vertex buffer.  If we are at the 
                // end of the vertex buffer, DISCARD the vertex buffer and start
                // at the beginning.  Otherwise, specify NOOVERWRITE, so we can
                // continue filling the VB while the previous chunk is drawing.
				m_dwBase += m_dwFlush;

				if(m_dwBase >= m_dwDiscard) m_dwBase = 0;

				WRAP(ifs_vb->Lock( m_dwBase * sizeof(POINTVERTEX), m_dwFlush * sizeof(POINTVERTEX),
		            (BYTE**) &pVertices, m_dwBase ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD ) );

//				printf("p pVertices[m_dwBase]\n");
				p=pVertices;

                dwNumParticlesToRender = 0;
            }

	 	} while(--i);
//		printf("---\n");
	}
	END_DRAW();
/*------------------------*/
//				printf("enddraw\n");
    // Unlock the vertex buffer
    ifs_vb->Unlock();

    // Render any remaining particles
    if( dwNumParticlesToRender )
    {
        WRAP(g_pd3dDevice->DrawPrimitive( D3DPT_POINTLIST, m_dwBase, dwNumParticlesToRender ));
    }
//    return S_OK;
/*------------------------*/
}
