
#include <assert.h>

#define ASSERT(x) assert(x)

#ifdef WIN32

#include <stdio.h>
#include <d3d8.h>
#include <d3dx8.h>
#include <mmsystem.h>

#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"d3d8.lib")
#ifdef _DEBUG
    #pragma comment(lib, "d3dx8d.lib")
#else
    #pragma comment(lib, "d3dx8.lib")
#endif

#endif


#ifdef _XBOX
#include <xtl.h>
#include <xgraphics.h>
#include <tchar.h> 
#include <cstdio>
#include <cassert>
#include "XBUtil.h"
#endif

#include "../zlib/zlib.h"

void demo_init();
void demo_render(float t);

void mosaic_init();
void mosaic_render(float t);
void zoomer_init();
void zoomer_render(float t);
void slides_init();
void slides_render(float t);
void ifs_init();
void ifs_render(float t);
void meta_init();
void meta_render(float t);

extern float angles[256];


extern LPDIRECT3DDEVICE8       g_pd3dDevice;

extern HRESULT CreateSecondaryRenderTarget();
extern void SetSecondaryRenderTarget();
extern void RenderSecondary(LPDIRECT3DSURFACE8 m_pBackBuffer, LPDIRECT3DSURFACE8 m_pDepthBuffer, int blend=0);

#define WRAP(x) {\
	HRESULT hresult=x; \
	ASSERT(SUCCEEDED(hresult)); \
}

extern	LPDIRECT3DSURFACE8 main_back;
extern	LPDIRECT3DSURFACE8 main_depth;


#define CLAMP(x) (max(min(1,x),0))

extern void zoomer_none();
extern void zoomer_set(int layer, float speed, float rotate, float hue, float pos=0.0, float posrot=0.0);
struct ANIMSET
{
	int flags;
	int frameCount;
	LPDIRECT3DTEXTURE8 *frames;
	double rott;
	double zoomt;
	double speed;
	double rotate;
	double hue;
	double huet;
	double scale;
	int colorop;
};

extern ANIMSET animSet[32];
extern float motionblur;
extern float radialblur;
extern float fade;
