



#include "plattis.h"
#include"IFS.h"

IFS::Animation2			g_anim;

#define m_pd3dDevice g_pd3dDevice
#define pd3dDevice g_pd3dDevice

LPDIRECT3DTEXTURE8      bg= NULL; // Our texture
LPDIRECT3DTEXTURE8      m_pParticleTexture= NULL; // Our texture
LPDIRECT3DVERTEXBUFFER8 ifs_vb= NULL; // Buffer to hold vertices

int ifs_number=0;

#define POINTVERTEX_FVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)
struct POINTVERTEX
{
    D3DXVECTOR3	v;
    D3DCOLOR	color;
};
/*

int m_dwFlush=512;
int m_dwDiscard=2048;
int m_dwBase=2048;
*/

extern int m_dwDiscard;

float ifs_z=0;

int ifs_loaded=0;

char *ifs_setit[]={
	"d:\\media\\feathers-anim.fs2",
	"d:\\media\\kuul.fs2",
	"d:\\media\\kuuler.fs2",
	"d:\\media\\natinpi.fs2",
	"d:\\media\\qukka2.fs2",
	"d:\\media\\qukka3lyhyt.fs2",
	"d:\\media\\rinki.fs2",
	"d:\\media\\syke.fs2",
	"d:\\media\\ylvas-00.fs2",
	"d:\\media\\YRITELMA-00.fs2",
	"d:\\media\\YRITELMA-01.fs2",
};

void ifs_load(int c)
{
	ifs_loaded=c;
	g_anim.systems = new IFS::System2();
	g_anim.system_quantity = 1;
	g_anim.systems[0].time = 0;

	g_anim.loadFS2(ifs_setit[c%11]);
}

void ifs_init()
{
//	WRAP(D3DXCreateTextureFromFile( g_pd3dDevice, "d:\\media\\foo.bmp", &bg));
	WRAP(D3DXCreateTextureFromFile( g_pd3dDevice, "d:\\media\\particle.tga", &m_pParticleTexture));

//	printf("CreateVertexBuffer %d %d\n", m_dwDiscard, sizeof(POINTVERTEX));
    g_pd3dDevice->CreateVertexBuffer( m_dwDiscard * sizeof(POINTVERTEX), 
		D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY | D3DUSAGE_POINTS, 
        POINTVERTEX_FVF, 
		D3DPOOL_DEFAULT, 
		&ifs_vb);

	//	srand(time(0));

	ifs_load(0);
	// make initial animation

/*
	pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  FALSE );
	pd3dDevice->SetRenderState( D3DRS_POINTSIZE,     FtoDW(0.08f) );
	pd3dDevice->SetRenderState( D3DRS_POINTSIZE_MIN, FtoDW(0.00f) );
	pd3dDevice->SetRenderState( D3DRS_POINTSCALE_A,  FtoDW(0.00f) );
	pd3dDevice->SetRenderState( D3DRS_POINTSCALE_B,  FtoDW(0.00f) );
	pd3dDevice->SetRenderState( D3DRS_POINTSCALE_C,  FtoDW(1.00f) );
*/
	}




void render_bg(LPDIRECT3DDEVICE8 pd3dDevice, LPDIRECT3DTEXTURE8 bg)
{
    // Render the secondary color surface to the screen
    struct VERTEX { D3DXVECTOR4 p; FLOAT tu, tv; 
	};
    VERTEX v[4];
    v[0].p = D3DXVECTOR4(   0 - 0.5f,   0 - 0.5f, 0, 0 );  v[0].tu =   0; v[0].tv =   0;
    v[1].p = D3DXVECTOR4( 640 - 0.5f,   0 - 0.5f, 0, 0 );  v[1].tu = 640; v[1].tv =   0;
    v[2].p = D3DXVECTOR4( 640 - 0.5f, 480 - 0.5f, 0, 0 );  v[2].tu = 640; v[2].tv = 480;
    v[3].p = D3DXVECTOR4(   0 - 0.5f, 480 - 0.5f, 0, 0 );  v[3].tu =   0; v[3].tv = 480;
	pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_TEX1);

    pd3dDevice->SetTexture( 0, bg);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER , D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER , D3DTEXF_LINEAR);

	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xff202020);

	pd3dDevice->SetRenderState( D3DRS_ZENABLE,   FALSE );
    pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
/*
	int i=0;

//	for(int i=32;i>0;i=i-3)
	{
    v[0].tu =   0+i; v[0].tv =   0+i;
    v[1].tu = 640-i; v[1].tv =   0+i;
    v[2].tu = 640-i; v[2].tv = 480-i;
    v[3].tu =   0+i; v[3].tv = 480-i;
*/
    WRAP(pd3dDevice->DrawPrimitiveUP( D3DPT_QUADLIST, 1, v, 6*sizeof(FLOAT) ));

	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, -1);

    pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
}




// If we rendered the scene to a secondary render target, then we now need
// to render the texture to the primary backbuffer.
void ifs_clear()
{
    // Render the secondary color surface to the screen
    struct VERTEX { D3DXVECTOR4 p; FLOAT tu, tv; 
	};
    VERTEX v[4];
    v[0].p = D3DXVECTOR4(   0 - 0.5f,   0 - 0.5f, 0, 0 );  v[0].tu =   0; v[0].tv =   0;
    v[1].p = D3DXVECTOR4( 640 - 0.5f,   0 - 0.5f, 0, 0 );  v[1].tu = 640; v[1].tv =   0;
    v[2].p = D3DXVECTOR4( 640 - 0.5f, 480 - 0.5f, 0, 0 );  v[2].tu = 640; v[2].tv = 480;
    v[3].p = D3DXVECTOR4(   0 - 0.5f, 480 - 0.5f, 0, 0 );  v[3].tu =   0; v[3].tv = 480;
	m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_TEX1);

    m_pd3dDevice->SetTexture( 0, NULL);
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER , D3DTEXF_LINEAR);
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER , D3DTEXF_LINEAR);

	m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 48<<24);

	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,   FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

    WRAP(m_pd3dDevice->DrawPrimitiveUP( D3DPT_QUADLIST, 1, v, 6*sizeof(FLOAT) ));
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
}












void ifs_render(float t)
{

	if(ifs_loaded!=(int)(ifs_number)) ifs_load((int)ifs_number);
//	if(ifs_loaded!=(int)(angles[254])) ifs_load((int)angles[254]);

	g_pd3dDevice->BeginScene();
//	g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0xff000000, 1.0f, 0L );

	ifs_clear();


	g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	g_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_ALWAYS);

//	render_bg(g_pd3dDevice, bg);


	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
//	D3DXMatrixScaling(&matWorld, ifs_z,ifs_z,ifs_z); 
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	// Set up our view matrix. A view matrix can be defined given an eye point,
	// a point to lookat, and a direction for which way is up. Here, we set the
	// eye five units back along the z-axis and up three units, look at the
	// origin, and define "up" to be in the y-direction.
	D3DXMATRIX matView;
	D3DXMatrixLookAtLH( &matView, &D3DXVECTOR3( 0.0f, 0.0f,-3.0f+ifs_z),
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
	D3DXMatrixPerspectiveFovLH( &matProj, 1, 4.f/3.f, 0.1f, 100.0f );
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );


	g_pd3dDevice->SetTexture(3, m_pParticleTexture );

	int alpha=(int)(255*fade);
	m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0x01010101*(alpha));

	m_pd3dDevice->SetTextureStageState( 3, D3DTSS_TEXCOORDINDEX, 0);
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLORARG2, D3DTA_CURRENT);
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_ALPHAARG1, D3DTA_CURRENT );


	// Set the render states for using point sprites
	g_pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE);
	g_pd3dDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  FALSE );
	g_pd3dDevice->SetRenderState( D3DRS_POINTSIZE,     FtoDW(8.0f) );
	g_pd3dDevice->SetRenderState( D3DRS_POINTSIZE_MIN, FtoDW(0.01f) );
	g_pd3dDevice->SetRenderState( D3DRS_POINTSCALE_A,  FtoDW(0.00f) );
	g_pd3dDevice->SetRenderState( D3DRS_POINTSCALE_B,  FtoDW(0.00f) );
	g_pd3dDevice->SetRenderState( D3DRS_POINTSCALE_C,  FtoDW(1.00f) );


	// Set up the vertex buffer to be rendered
	//    pd3dDevice->SetStreamSource( 0, ifs_vb, 0, sizeof(POINTVERTEX) );
	g_pd3dDevice->SetVertexShader( D3DFVF_XYZ | D3DFVF_DIFFUSE );
/*
g_pd3dDevice->SetRenderState (D3DRS_POINTSCALEENABLE, FALSE);
// All textures must be turned off.
g_pd3dDevice->SetTexture (0, NULL); 
g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP,  D3DTOP_DISABLE);
// The point size render state must be set to any value between 0.0-1.0
g_pd3dDevice->SetRenderState(D3DRS_POINTSIZE, 1.0);
*/

	g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

    // Turn off culling
    g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

    // Turn off D3D lighting
    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	//	printf("%4.2f\n", t);
	g_anim.time = fmodf(t, g_anim.getEndTime());
	g_anim.renderFlame();

	g_pd3dDevice->SetTexture(3, NULL);
	g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	g_pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE);

	g_pd3dDevice->EndScene();
//	g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}

