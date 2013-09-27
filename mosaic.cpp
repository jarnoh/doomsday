#include "plattis.h"
#include <MATH.h>

#define m_pd3dDevice g_pd3dDevice
#define pd3dDevice g_pd3dDevice

#define WRAP(x) {\
	HRESULT hresult=x; \
	ASSERT(SUCCEEDED(hresult)); \
}

float mosaic_text_alpha=1;
// -tile 24x27 

const short tiles[]={
4686, 1529, 4686, 6328, 6535, 2108, 1096, 5182, 5682, 5813, 4163, 2358, 2358, 4908, 4908, 4716, 723, 3269,  5156, 1617, 1880, -1, -1,  -1,
6210, 4532, 6830, 1366, 6232, 6790, 4718, 5766, 2634, 5404, 1536, 4723, 2170, 3784, 2170, 3269, 2892, 541,  4905, 6264, 3164, -1, -1,  -1, 
2391, 2092, 1680, 6377, 2657, 5481, 1184, 2836, 6462, 1940, 6007,  718, 336, 3784, 3420,   108, 4570, 2892, 6336, 1821, 5684, -1, -1,  -1, 
2384, 2334, 2162, 3557, 6108, 2969, 3058, 203,  2657, 2519, 892,  3905, 4571, 4807, 3269, 6264, 2593, 6336, 6331, 1617, 4570, 2354, -1,  -1,
4409, 5134, 397,  2773, 4516, 1968, 5134, 5308, 2793, 615, 2921,  3279, 4163, 723, 1154,  1154, 2593, 6264, 6331, 2593, 1821, 3309, -1,  -1, 
2629, 1145, 6768, 5158, 2090, 435,  5012, 3691, 3094, 2519, 5281, 5520, 4634, 6382, 5440, 4807, 3501, 4729, 5156, 1617, 4905, 3309, -1,  -1, 
4002, 3506, 6157, 3680, 5664, 5037, 3572, 6232, 3506, 713, 4718,   892, 4723, 1935, 2817, 4807, 541, 6331,  2892, 1821, 4905, 2783, -1,  -1, 
2818, 1201, 4334, 1296, 5983, 4354, 5494, 1061, 1192, 5670, 5557, 2366, 3908, 2924, 7067, 6074, 4696, 2693, 1154, 177, 5156,  2765,  -1,  -1,
1366, 4837, 4021, 1976, 4737, 3817, 2497, 2479, 2506, 1287, 6192, 5174, 2497, 40, 5475,   2479, 5557, 6166, 6382, 3637, 5155, 4729, 6845,  -1,
5609, 3058, 3409, 2498, 4021, 4513, 4345, 873, 2227, 4016, 3564,  1887, 2737, 4491, 2773, 6115, 3045, 6006, 4571, 5676, 1096, 1940, 6511,  -1,
5193, 3398, 4409, 5187, 4085, 6996, 1633, 4029, 713, 5683, 3679,   906, 6428, 257, 6710,  1271, 1879, 1644, 2472, 3883, 2214, 1921, 1074,  -1, 
6528, 2636, 1326, 2636, 2838, 3725, 1339, 3306, 3680, 5954, 5024, 3616, 5749, 4906, 5323, 6881, 5503, 4504, 3637, 130, 3810,  -1, -1,  -1, 
5193, 520,  5590, 2921, 1130, 5590, 4831, 4029, 40, 1533, 4638,   5182, 4727, 1091, 4574, 4921, 842, 2447,  3030, 108, 4359,  -1,  -1,  -1, 
5664, 658,  3384, 5450, 2354, 6174, 5581, 1326, 4698, 2872, 4574, 889,   541, 5155, 933, 130,    720, 2443, 2187, 177,  -1, -1, -1,  -1, 
6878, 2384, 2108, 2379, 6610, 5105, 4504, 1498, 6760, 5524, 3536, 1533, 2693, 2472, 3501, 7067, 2629, 6874, 5118, 2693, 518,  -1, -1,  -1, 
1780, 4354, 3989, 4002, 400,  2090, 2002, 2097, 4354, 3398, 2062, 1536, 1537, 5440, 5503, 5834, 6996, 3083, 3858, 2817, 842,  -1, -1, -1,
1370, 435, 1753, 5195, 1937, 5158, 4738, 3679, 1450, 6768, 2366, 2379, 4391,  889, 6017, 3817, 2439, 574, 4972, 3315, 3164,   -1, -1,  -1, 
2391, 4737, 5921, 5105, 6610, 1976, 1040, 5404, 1022, 4974, 1827, 2020, 5042, 3420, 2662, 5147, 5163, 5163, 6760, 4716, 2486, -1, -1,  -1,
1633, 4864, 733,  2999, 4532, 2836, 4186, 5682, 2259, 644, 1921, 6166, 4634,  6707, 727, 1084, 5016,  1887, 6328, 914, -1, -1,  -1,  -1,
1201, 4974, 2443, 3572, 3577, 1680, 1937, 2335, 4457, 3741, 5475, 1503, 6835, 5071, 4572, 1644, 4502, 4933, 1923, -1, -1, -1, -1,  -1, 
6528, 100, 1390, 2304, 5992, 2092, 3585, 5118, 103, 4516, 6631, 5008, 6874, 2969,  4825, 7032, 4752, 3279,  4502, -1, -1, -1, -1,  -1, 
580, 580, 5834, 4841, 4864, 1521, 5494, 2187, 4548, 6548, 2032, 6631, 6395, 3133, 1753, 4841, 5013, 6490,   -1, -1, -1, -1, -1,  -1, 
3347, 82, 2334, 4815, 4815, 82,   6174, 1184, 4677, 2999, 5281, 4572, 5058, 3770, 5527, 1091, 749, 1044,    -1, -1, -1, -1,  -1,  -1, 
6830, 5657, 2858, 1737, 1130, 5037, 3143, 6815, 5249, 1549, 5988, 6176, 5527, 5474, 2621, 3905, 4696, 2891, -1, -1, -1, -1,  -1,  -1, 
4419, 3383, 2097, 1192, 1192, 1835, 1529, 5327, 4837, 6629, 5459, 6642, 4738, 6845, 6178, 2335, 1537, 1581, -1, -1, -1, -1,  -1,  -1, 
1498, 3985, 5660, 5660, 6916, 6115, 3083, 3383, 1737, 6311, 2062, 6707, 5683, 727,  4273, 4638, 6007, 1283, -1, -1, -1, -1,  -1,  -1, 
2783, 4955, 6395, 3293, 1521, 2793,  733, 6178, 4955, 733, 6462, 6054, 5676, 1777,  1579, 4921, 1013, 3315, 1283, -1, -1, -1,  -1,  -1, 

// 4686, 1529, 4686, 6328, 6535, 2108, 1096, 5182, 5682, 5813, 4163, 2358, 2358, 4908, 4908, 4716, 723, 3269, 5156, 1617, 1880, 520, 2858, 6535, 6210, 4532, 6830, 1366, 6232, 6790, 4718, 5766, 2634, 5404, 1536, 4723, 2170, 3784, 2170, 3269, 2892, 541, 4905, 6264, 3164, 4016, 6830, 1187, 2391, 2092, 1680, 6377, 2657, 5481, 1184, 2836, 6462, 1940, 6007, 718, 336, 3784, 3420, 108, 4570, 2892, 6336, 1821, 5684, 6210, 4491, 5609, 2384, 2334, 2162, 3557, 6108, 2969, 3058, 203, 2657, 2519, 892, 3905, 4571, 4807, 3269, 6264, 2593, 6336, 6331, 1617, 4570, 2354, 2162, 6377, 4409, 5134, 397, 2773, 4516, 1968, 5134, 5308, 2793, 615, 2921, 3279, 4163, 723, 1154, 1154, 2593, 6264, 6331, 2593, 1821, 3309, 1968, 5187, 2629, 1145, 6768, 5158, 2090, 435, 5012, 3691, 3094, 2519, 5281, 5520, 4634, 6382, 5440, 4807, 3501, 4729, 5156, 1617, 4905, 3309, 6141, 397, 4002, 3506, 6157, 3680, 5664, 5037, 3572, 6232, 3506, 713, 4718, 892, 4723, 1935, 2817, 4807, 541, 6331, 2892, 1821, 4905, 2783, 6157, 2496, 2818, 1201, 4334, 1296, 5983, 4354, 5494, 1061, 1192, 5670, 5557, 2366, 3908, 2924, 7067, 6074, 4696, 2693, 1154, 177, 5156, 2765, 978, 4815, 1366, 4837, 4021, 1976, 4737, 3817, 2497, 2479, 2506, 1287, 6192, 5174, 2497, 40, 5475, 2479, 5557, 6166, 6382, 3637, 5155, 4729, 6845, 5174, 5609, 3058, 3409, 2498, 4021, 4513, 4345, 873, 2227, 4016, 3564, 1887, 2737, 4491, 2773, 6115, 3045, 6006, 4571, 5676, 1096, 1940, 6511, 4513, 5193, 3398, 4409, 5187, 4085, 6996, 1633, 4029, 713, 5683, 3679, 906, 6428, 257, 6710, 1271, 1879, 1644, 2472, 3883, 2214, 1921, 1074, 4085, 6528, 2636, 1326, 2636, 2838, 3725, 1339, 3306, 3680, 5954, 5024, 3616, 5749, 4906, 5323, 6881, 5503, 4504, 3637, 130, 3810, 3725, 3006, 1145, 5193, 520, 5590, 2921, 1130, 5590, 4831, 4029, 40, 1533, 4638, 5182, 4727, 1091, 4574, 4921, 842, 2447, 3030, 108, 4359, 4831, 39, 1370, 5664, 658, 3384, 5450, 2354, 6174, 5581, 1326, 4698, 2872, 4574, 889, 541, 5155, 933, 130, 720, 2443, 2187, 177, 412, 2737, 987, 1835, 6878, 2384, 2108, 2379, 6610, 5105, 4504, 1498, 6760, 5524, 3536, 1533, 2693, 2472, 3501, 7067, 2629, 6874, 5118, 2693, 518, 2304, 5195, 1339, 1780, 4354, 3989, 4002, 400, 2090, 2002, 2097, 4354, 3398, 2062, 1536, 1537, 5440, 5503, 5834, 6996, 3083, 3858, 2817, 842, 2116, 1450, 2506, 1370, 435, 1753, 5195, 1937, 5158, 4738, 3679, 1450, 6768, 2366, 2379, 4391, 889, 6017, 3817, 2439, 574, 4972, 3315, 3164, 412, 6916, 3577, 2391, 4737, 5921, 5105, 6610, 1976, 1040, 5404, 1022, 4974, 1827, 2020, 5042, 3420, 2662, 5147, 5163, 5163, 6760, 4716, 2486, 5147, 4419, 5581, 1633, 4864, 733, 2999, 4532, 2836, 4186, 5682, 2259, 644, 1921, 6166, 4634, 6707, 727, 1084, 5016, 1887, 6328, 914, 30, 3337, 595, 39, 1201, 4974, 2443, 3572, 3577, 1680, 1937, 2335, 4457, 3741, 5475, 1503, 6835, 5071, 4572, 1644, 4502, 4933, 1923, 5170, 850, 4183, 574, 6878, 6528, 100, 1390, 2304, 5992, 2092, 3585, 5118, 103, 4516, 6631, 5008, 6874, 2969, 4825, 7032, 4752, 3279, 4502, 2707, 658, 4513, 5992, 3557, 580, 580, 5834, 4841, 4864, 1521, 5494, 2187, 4548, 6548, 2032, 6631, 6395, 3133, 1753, 4841, 5013, 6490, 5327, 4183, 978, 2838, 3006, 2334, 3347, 82, 2334, 4815, 4815, 82, 6174, 1184, 4677, 2999, 5281, 4572, 5058, 3770, 5527, 1091, 749, 1044, 1453, 3143, 3143, 3347, 1360, 1453, 6830, 5657, 2858, 1737, 1130, 5037, 3143, 6815, 5249, 1549, 5988, 6176, 5527, 5474, 2621, 3905, 4696, 2891, 3741, 3293, 6472, 5660, 6141, 5012, 4419, 3383, 2097, 1192, 1192, 1835, 1529, 5327, 4837, 6629, 5459, 6642, 4738, 6845, 6178, 2335, 1537, 1581, 2496, 2343, 1360, 5008, 5921, 5846, 1498, 3985, 5660, 5660, 6916, 6115, 3083, 3383, 1737, 6311, 2062, 6707, 5683, 727, 4273, 4638, 6007, 1283, 4992, 5657, 4992, 3337, 5661, 5657, 2783, 4955, 6395, 3293, 1521, 2793, 733, 6178, 4955, 733, 6462, 6054, 5676, 1777, 1579, 4921, 1013, 3315, 1283, 3293, 5170, 595, 3989, 5846, 2756, 3172, 1450, 5943, 5474, 3384, 30, 987, 6924, 601, 3133, 2214, 3782, 279, 6881, 1777, 1935, 933, 2765, 5016, 3985, 5160, 5661, 2116
};

LPDIRECT3DTEXTURE8 tex[24*27];

//LPDIRECT3DTEXTURE8      mosaic_tex= NULL;
LPDIRECT3DTEXTURE8      tex_rip= NULL;


struct CUSTOMVERTEX
{
	FLOAT x, y, z;
    D3DCOLOR    color;    // The color
	FLOAT u, v;
};
#define CUSTOMVERTEX_FVF (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

#include "xbmesh.h"

CXBMesh cube;

float *tiletime;

void mosaic_init()
{
	char buf[256];
//	WRAP(D3DXCreateTextureFromFile( g_pd3dDevice, "d:\\media\\mosaic\\987.dds", &mosaic_tex));
	WRAP(D3DXCreateTextureFromFile( g_pd3dDevice, "d:\\media\\rip.dds", &tex_rip));

	tiletime=new float[24*27];

	int *p=new int[10000];
	memset(p, -1, sizeof(int)*10000);
	for(int i=0;i<24*27;i++)
	{
		tex[i]=NULL;
		tiletime[i]=(rand()/(float)RAND_MAX);
		int t=tiles[i];
		if(t==-1) continue;

		if(p[t]==-1)
		{
			sprintf(buf, "d:\\media\\mosaic\\%d.dds", t);
			WRAP(D3DXCreateTextureFromFile( g_pd3dDevice, buf, &tex[i]));
			g_pd3dDevice->SetTexture(0, tex[i] );
			p[t]=i;
		}
		else
			tex[i]=tex[p[t]];
	}

	delete p;
	WRAP(cube.Create("D:\\media\\noppa-96faces-ok.xbg"));
}

void mosaic_render(float t)
{
    g_pd3dDevice->BeginScene();
    g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0xff000000, 1.0f, 0L );

	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	g_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESS);

//	g_pd3dDevice->SetTexture(0, mosaic_tex );

    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER ,   D3DTEXF_LINEAR  );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER ,   D3DTEXF_LINEAR  );


    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );


	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER , D3DTEXF_LINEAR);
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER , D3DTEXF_LINEAR);

	g_pd3dDevice->SetVertexShader(CUSTOMVERTEX_FVF);


    // For our world matrix, we will just leave it as the identity
    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up. Here, we set the
    // eye five units back along the z-axis and up three units, look at the
    // origin, and define "up" to be in the y-direction.
    D3DXMATRIX matView;
//    D3DXMatrixLookAtLH( &matView, &D3DXVECTOR3(16.0f, 4.0f,-20.0f-t*4), //sin(t*.03)*20 ),
    D3DXMatrixLookAtLH( &matView, &D3DXVECTOR3(-16.0f, 0.0f,-23.0f-t*.9f), //sin(t*.03)*20 ),
                                  &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ),
                                  &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    // For the projection matrix, we set up a perspective transform (which
    // transforms geometry from 3D view space to 2D viewport space, with
    // a perspective divide making objects smaller in the distance). To build
    // a perpsective transform, we need the field of view (1/4 pi is common),
    // the aspect ratio, and the near and far clipping planes (which define at
    // what distances geometry should be no longer be rendered).
    D3DXMATRIX matProj;
    //D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 100.0f );
    D3DXMatrixPerspectiveFovLH( &matProj, 1, 4.f/3.f, 1.0f, 2000.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	for(int y=0;y<24;y++)
	for(int i=0;i<24;i++)
	{
		int off=y*24+i;
		if(tex[off]==NULL) continue;

		float dt=tiletime[off]*10-t;

		if(dt>0) continue;
		dt=CLAMP(-dt*.16);

		float dx=i-12.f;
		float dy=y-12.f;
		float sc=sqrt(dx*dx+dy*dy)/17.0;
		sc=1-pow(CLAMP(sc),1.1f);

		float q=.25f*(2.4+sin(i*.431+t*.5)+cos(y*.244+t*.78));
		sc=sc*dt*CLAMP(q);
		int alpha=255*fade*CLAMP(sc*1.7f);

//		alpha=255;
	    g_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, alpha*0x01010101);

		float z=i*.1+y*0.08;
		D3DXMATRIX matWorld, mat ;
		D3DXMatrixIdentity( &matWorld );
		
		/*D3DXMatrixRotationZ( &mat, t*.3+z*3 ); D3DXMatrixMultiply(&matWorld, &matWorld, &mat);*/
		D3DXMatrixRotationX( &mat, sin(t+off*.3) ); D3DXMatrixMultiply(&matWorld, &matWorld, &mat);
		D3DXMatrixRotationY( &mat, t*.1+z ); D3DXMatrixMultiply(&matWorld, &matWorld, &mat);
		D3DXMatrixTranslation(&mat, (i-12)*2.05, -(y-12)*2.0, 2*(sin(i*.361+t*.2)+cos(y*.144+t*.28))); D3DXMatrixMultiply(&matWorld, &matWorld, &mat);

		dt=pow(dt,.3f);

		D3DXMatrixTranslation(&mat, 
			(tiletime[(off*3)&511]-.5f)*(1-dt)*130, 
			(tiletime[(off*3+1)&511]-.5f)*(1-dt)*130, 
			(tiletime[(off*3+2)&511]-.5f)*(1-dt)*130); D3DXMatrixMultiply(&matWorld, &matWorld, &mat);
//		D3DXMatrixRotationZ( &mat, t*.3+z*3 ); D3DXMatrixMultiply(&matWorld, &matWorld, &mat); D3DXMatrixMultiply(&matWorld, &matWorld, &mat);
		g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
 
		g_pd3dDevice->SetTexture(0, tex[off] );

		cube.Render(XBMESH_NOTEXTURES);
	}
/*

	g_pd3dDevice->SetIndices(mosaic_ib, 0);
	g_pd3dDevice->SetStreamSource( 0, mosaic_vb, sizeof(CUSTOMVERTEX) );
	g_pd3dDevice->DrawPrimitive( D3DPT_POINTLIST, 0, 8);
*/
    g_pd3dDevice->EndScene();


 //   g_pd3dDevice->Present( NULL, NULL, NULL, NULL );


}



void mosaic_text()
{
	// Render the secondary color surface to the screen
	struct VERTEX { D3DXVECTOR4 p; FLOAT tu, tv; 
	};
	VERTEX v[4];
	v[0].p = D3DXVECTOR4( 480 - 0.5f, 378 - 0.5f, 0, 0 );  v[0].tu =   0; v[0].tv =   0;
	v[1].p = D3DXVECTOR4( 600 - 0.5f, 378 - 0.5f, 0, 0 );  v[1].tu = 1; v[1].tv =   0;
	v[2].p = D3DXVECTOR4( 600 - 0.5f, 438 - 0.5f, 0, 0 );  v[2].tu = 1; v[2].tv = 1;
	v[3].p = D3DXVECTOR4( 480 - 0.5f, 438 - 0.5f, 0, 0 );  v[3].tu =   0; v[3].tv = 1;
	m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_TEX1);

	m_pd3dDevice->SetTexture( 0, tex_rip);
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );

	int a=(int)(255*CLAMP(mosaic_text_alpha));
    g_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, a*0x01010101);

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER , D3DTEXF_LINEAR);
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER , D3DTEXF_LINEAR);

	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,   FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	WRAP(m_pd3dDevice->DrawPrimitiveUP( D3DPT_QUADLIST, 1, v, 6*sizeof(FLOAT) ));
}