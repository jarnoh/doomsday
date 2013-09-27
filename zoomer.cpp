#include "plattis.h"
#include "mat3x3.h"

#define MAXFRAMES 256
//#define MAXFRAMES 4

#define DDS_SIZE_256X256_DXT1 32896

int animCount=1;
int zoomer_blend=0;

float zoomer_bias=0;
float zoomer_biasr=0;
float zoomer_biasg=0;
float zoomer_biasb=0;

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
LPDIRECT3DVERTEXBUFFER8 g_pVB        = NULL; // Buffer to hold vertices
LPDIRECT3DTEXTURE8      g_pTexture   = NULL; // Our texture

//extern int zoomer0, zoomer1;
/*
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
};
*/
ANIMSET animSet[32];

double prevt=-1;
//double t0;

// A structure for our custom vertex type. We added texture coordinates
struct CUSTOMVERTEX
{
    D3DXVECTOR3 position; // The position
    D3DCOLOR    color;    // The color
    D3DXVECTOR2 uv;
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)


DWORD ps_color3x3;

void ps_set_colormatrix(float mat[3][3])
{
	for(int i=0;i<3;i++)
	{
		float v[4]={mat[0][i],mat[1][i],mat[2][i], 0}; 
		g_pd3dDevice->SetPixelShaderConstant(i, v, 1);
	}
	float bias[4]={zoomer_biasr+zoomer_bias,zoomer_biasg+zoomer_bias,zoomer_biasb+zoomer_bias,0};
	g_pd3dDevice->SetPixelShaderConstant(3, bias, 1);
}

void zoomer_none()
{
	prevt=0;
	for(int c=0;c<animCount;c++)
	{
		int i=c%animCount;
		animSet[i].zoomt=0;
		animSet[i].rott=0;
		animSet[i].flags=0;
	}
}

void zoomer_set(int layer, float speed, float rotate, float hue, float pos, float posrot)
{
	int i=layer%animCount;
	animSet[i].flags=1;
	animSet[i].speed=speed;
	animSet[i].rotate=rotate;
	animSet[i].hue=hue;
	animSet[i].huet=0;
	animSet[i].scale=1;
	animSet[i].zoomt=pos+(speed<0?-6:0)+(i==9?5:0);
	animSet[i].rott=posrot;
}

void zoomer_set2(int layer, float speed, float rotate, float hue, float pos, float posrot)
{
	int i=layer%animCount;
	animSet[i].flags=1;
	animSet[i].speed=speed;
	animSet[i].rotate=rotate;
	animSet[i].hue=hue;
	// animSet[i].huet=0;
	animSet[i].scale=1;
	// animSet[i].zoomt=pos+(speed<0?-6:0);
	// animSet[i].rott=posrot;
}


int zoomer_load_gzbunch(ANIMSET *set, const char *filename, int sizePerTexture)
{
		set->flags=1;//(a==0);
		int frames=0;
		set->speed=8;
		set->rotate=0;
		set->frames=new LPDIRECT3DTEXTURE8[256];


		gzFile file;
		file = gzopen(filename, "rb");
		if (file == NULL) return 0; // no gz file

#if 1
		byte *data=(byte*)malloc(sizePerTexture);
		for(int i=0;i<MAXFRAMES;i++)
		{
				int uncomprLen = gzread(file, data, sizePerTexture);
				if(uncomprLen==0) break;
				assert(uncomprLen==sizePerTexture);
				WRAP( D3DXCreateTextureFromFileInMemory( g_pd3dDevice, data, uncomprLen, &set->frames[i] ) );

			frames++;
		}
		free(data);
#else
		for(int i=0;i<MAXFRAMES;i++)
		{
			IDirect3DTexture8* pTexture = new IDirect3DTexture8;
			DWORD dwTextureSize = XGSetTextureHeader( dwWidth, dwHeight, 1, 0, D3DFMT_DXT1, 0, pTexture, 0, 0 );
			void* pBuffer = D3D_AllocContiguousMemory( dwTextureSize, D3DTEXTURE_ALIGNMENT );
			pTexture->Register( pBuffer );
			set->frames[i]=pTexture;
		}

#endif
		gzclose(file);

		set->frameCount=frames;
		set->rott=0;
		set->zoomt=0;

		return frames;
}



void zoomer_init()
{
#ifdef _WINDOWS

	LPD3DXBUFFER pCode;                  // buffer with the assembled shader code
	LPD3DXBUFFER pErrorMsgs;             // buffer with error messages
	D3DXAssembleShaderFromFile( "color3x3.psh", 0, NULL, &pCode, NULL );

	if( FAILED( g_pd3dDevice->CreatePixelShader( (DWORD*)pCode->GetBufferPointer(), &ps_color3x3 )))
	{
		printf("shader compile failed\n");
	}
#endif

#ifdef _XBOX
    {
#include "color3x3.inl"
        g_pd3dDevice->CreatePixelShader( &psd, &ps_color3x3 );
    }
#endif


/*
	for(int a=0;a<animCount;a++)
	{
		ANIMSET *s=&animSet[a];

		s->flags=1;//(a==0);
		int frames=0;
		s->speed=8+a;
		s->rotate=0;
		s->frames=new LPDIRECT3DTEXTURE8[256];
		for(int i=0;i<256;i++)
		{

			char buf[256];
			sprintf(buf, path[a+1], i+1);
			printf("%2d - %2d: %s\n", a, i, buf);

			// Use D3DX to create a texture from a file based image
			if( FAILED( D3DXCreateTextureFromFile( g_pd3dDevice, buf, &s->frames[i] ) ) )
				break;

			frames++;
		}
		if(frames==0) return ;//E_FAIL;
		printf("-- %2d\n", frames);
		s->frameCount=frames;
		s->rott=0;
		s->zoomt=0;
	}
*/

	animCount=0;
	zoomer_load_gzbunch(&animSet[animCount++], "D:\\media\\anim.zz", DDS_SIZE_256X256_DXT1);
	zoomer_load_gzbunch(&animSet[animCount++], "D:\\media\\b.zz", DDS_SIZE_256X256_DXT1);
	zoomer_load_gzbunch(&animSet[animCount++], "D:\\media\\c.zz", DDS_SIZE_256X256_DXT1);
	zoomer_load_gzbunch(&animSet[animCount++], "D:\\media\\dd.zz", DDS_SIZE_256X256_DXT1);
	zoomer_load_gzbunch(&animSet[animCount++], "D:\\media\\j.zz", DDS_SIZE_256X256_DXT1);
	zoomer_load_gzbunch(&animSet[animCount++], "D:\\media\\k.zz", DDS_SIZE_256X256_DXT1);
	zoomer_load_gzbunch(&animSet[animCount++], "D:\\media\\l.zz", DDS_SIZE_256X256_DXT1);
	zoomer_load_gzbunch(&animSet[animCount++], "D:\\media\\nig.zz", DDS_SIZE_256X256_DXT1);
	zoomer_load_gzbunch(&animSet[animCount++], "D:\\media\\pp.zz", DDS_SIZE_256X256_DXT1);
	zoomer_load_gzbunch(&animSet[animCount++], "D:\\media\\r.zz", DDS_SIZE_256X256_DXT1);
	zoomer_load_gzbunch(&animSet[animCount++], "D:\\media\\voi.zz", DDS_SIZE_256X256_DXT1);
	zoomer_load_gzbunch(&animSet[animCount++], "D:\\media\\w2.zz", DDS_SIZE_256X256_DXT1);
	zoomer_load_gzbunch(&animSet[animCount++], "D:\\media\\x.zz", DDS_SIZE_256X256_DXT1);
	zoomer_load_gzbunch(&animSet[animCount++], "D:\\media\\y.zz", DDS_SIZE_256X256_DXT1);


	// Create the vertex buffer.
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( 50*2*sizeof(CUSTOMVERTEX),
                                                  0, D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_DEFAULT, &g_pVB ) ) )
    {
        return ;//E_FAIL;
    }

    // Fill the vertex buffer. We are setting the tu and tv texture
    // coordinates, which range from 0.0 to 1.0
    CUSTOMVERTEX* pVertices;
    if( FAILED( g_pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) ) return ;//E_FAIL;

	pVertices[0].position=D3DXVECTOR3(-1, 1,0);
	pVertices[1].position=D3DXVECTOR3( 1, 1,0);
	pVertices[3].position=D3DXVECTOR3( 1,-1,0);
	pVertices[2].position=D3DXVECTOR3(-1,-1,0);

	pVertices[0].uv=D3DXVECTOR2(0,0);
	pVertices[1].uv=D3DXVECTOR2(1,0);
	pVertices[3].uv=D3DXVECTOR2(1,1);
	pVertices[2].uv=D3DXVECTOR2(0,1);

	pVertices[0].color=-1;
	pVertices[1].color=-1;
	pVertices[2].color=-1;
	pVertices[3].color=-1;

    g_pVB->Unlock();

    return ;//S_OK;
}





//-----------------------------------------------------------------------------
// Name: SetupMatrices()
// Desc: Sets up the world, view, and projection transform matrices.
//-----------------------------------------------------------------------------
VOID SetupMatrices()
{
    // For our world matrix, we will just leave it as the identity
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixRotationX( &matWorld, timeGetTime()/1000.0f );
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up. Here, we set the
    // eye five units back along the z-axis and up three units, look at the
    // origin, and define "up" to be in the y-direction.
    D3DXMATRIX matView;
    D3DXMatrixLookAtLH( &matView, &D3DXVECTOR3( 0.0f, 0.0f,-2.0f ),
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
    D3DXMatrixPerspectiveFovLH( &matProj, 1, 4.f/3.f, 1.0f, 100.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}

extern float angles[256];

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
void zoomer_render(float t)
{
	double dt;

	// printf("%2.2f\n", t-t0);

	if(prevt==-1) 
		dt=0;
	else
		dt=t-prevt;

	prevt=t;

	int layers=0;
	for(int c=0;c<animCount;c++)
	{
		int i=c;
#ifdef _TEST
		animSet[i].speed=angles[i*8+0];
		animSet[i].rotate=angles[i*8+1];
		animSet[i].hue=angles[i*8+2];
		animSet[i].scale=angles[i*8+3];
#endif
		animSet[i].zoomt += dt*(animSet[i].speed<0?-1:1)*(animSet[i].speed*animSet[i].speed)*.015;
		animSet[i].zoomt += dt*animSet[i].speed*.015;
		animSet[i].rott -= dt*animSet[i].rotate*.02;
		animSet[i].huet += dt*animSet[i].hue;

		if(animSet[i].flags) layers++;
	}



    // Begin the scene
    g_pd3dDevice->BeginScene();

    g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0xff000000, 1.0f, 0L );

    // Turn off culling
    g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

    // Turn off D3D lighting
    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

    // Turn on the zbuffer
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
//	g_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESS);
	g_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_ALWAYS);

//	t0=timeGetTime()/1000.0f;

    g_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,   TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_STENCILFUNC,     D3DCMP_EQUAL );
//    g_pd3dDevice->SetRenderState( D3DRS_STENCILFUNC,     D3DCMP_ALWAYS);
    g_pd3dDevice->SetRenderState( D3DRS_STENCILREF,      0 );
    g_pd3dDevice->SetRenderState( D3DRS_STENCILMASK,     0xffffffff );
    g_pd3dDevice->SetRenderState( D3DRS_STENCILWRITEMASK,0xffffffff );

    // Increment the stencil buffer for each pixel drawn
    g_pd3dDevice->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );
    g_pd3dDevice->SetRenderState( D3DRS_STENCILFAIL,  D3DSTENCILOP_KEEP );
    g_pd3dDevice->SetRenderState( D3DRS_STENCILPASS,  D3DSTENCILOP_INCRSAT );



    // Setup the world, view, and projection matrices
    SetupMatrices();
    // Setup our texture. Using textures introduces the texture stage states,
    // which govern how textures get blended together (in the case of multiple
    // textures) and lighting information. In this case, we are modulating
    // (blending) our texture with the diffuse color of the vertices.
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
    //g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER , D3DTEXF_LINEAR);
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER , D3DTEXF_LINEAR);

	g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
	g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCALPHA );
//	g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );

	g_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE, TRUE);
	g_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xffffffff);
	//g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	if(layers)
		g_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xffffff+(255/layers<<24));

	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE|D3DTA_COMPLEMENT );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

    g_pd3dDevice->SetVertexShader( D3DFVF_CUSTOMVERTEX );

	g_pd3dDevice->SetPixelShader( ps_color3x3 );

//	saturatemat3(colortrans, .5f*(1+sin(t*.2)));
//	lummat3(colortrans);
//	printf("%4.2f\n", t);
//	simplehuerotatemat3(colortrans, t*180);




	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
//	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
//    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);

	int stencil=0;

//	animCount=1;

	switch(zoomer_blend&3)
	{
	case 0:
	g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		break;
	case 1:
	g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
		break;
	case 2:
	g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR );
		break;
	default:
	g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		break;
	}

	for(int aa=0;aa<animCount;aa++)
	{
		int a=(int)(angles[254])+aa;
		a=(a)%animCount;
		
//		if(stencil>=1) break;
//		int a=aa;
		if(!animSet[a].flags) continue;


		float d=(t+1+sin(a+t))*.31;
		float colortrans[3][3];
		identmat3(colortrans);
		
		t=animSet[a].zoomt;

		huerotatemat3(colortrans, animSet[a].huet);
		float sc=animSet[a].scale*CLAMP(fade);
		cscalemat3(colortrans, sc, sc, sc);
//		cscalemat3(colortrans, (1+sin(d))*.5, (1+sin(d*1.2))*.5, (1+sin(d*1.3))*.5);
//		printmat3(colortrans);
		ps_set_colormatrix(colortrans);
//		g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0xff000000, 1.0f, 0L );

		// TODO remove modulo ???
		t=fmodf(t+animSet[a].frameCount, animSet[a].frameCount);
		t=fmodf(t+animSet[a].frameCount, animSet[a].frameCount);
		int idx=(int)(t);
		t=pow(2.0f,t-idx);

//	    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0 );

//		printf("%2d %2d %2d\n", a, idx, animSet[a].frameCount);

		for(int i=6;i>1;i--)
		{
			D3DXMATRIX m;
			D3DXMatrixIdentity( &m);

			D3DXMATRIX r;
			D3DXMatrixRotationZ(&r, animSet[a].rott);

			double v=1.0/pow(2,(i)-1);
			double s=t*v*4;

			D3DXMatrixScaling(&m, s, s, 0);
			D3DXMatrixMultiply(&m, &r, &m);

			g_pd3dDevice->SetTransform( D3DTS_WORLD, &m);

		    g_pd3dDevice->SetRenderState( D3DRS_STENCILREF, stencil);
			g_pd3dDevice->SetTexture( 0, animSet[a].frames[((idx+i-1)%animSet[a].frameCount)] );
			g_pd3dDevice->SetStreamSource( 0, g_pVB, sizeof(CUSTOMVERTEX) );
			g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
		}
		stencil++;
		switch(zoomer_blend&3)
		{
		case 0:
		g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
		g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO);
			break;
		case 1:
		g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
		g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCALPHA );
			break;
		case 2:
		g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR );
			break;
		default:
			break;
		}
	}

	g_pd3dDevice->SetPixelShader( NULL );
    g_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE);
    g_pd3dDevice->EndScene();

//    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}


#ifdef _WINDOWS


void adjust(int ch)
{
	char *idx;
	char *qwerty="QqWwEeRrTtYyUuIiOoPp";
	char *asdf="AaSsDdFfGgHhJjKkLlÖö";

	idx=strchr(qwerty, ch);
	if(idx) 
	{
		int i=idx-qwerty;
		animSet[i/2].speed += (i&1)*2-1;
	}

	idx=strchr(asdf, ch);
	if(idx) 
	{
		int i=idx-asdf;
		animSet[i/2].rotate += (i&1)*2-1;
	}

}

//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
		case WM_CHAR:
			adjust(wParam);
			break;
	
		case WM_KEYDOWN:
			switch(wParam)
			{
			case VK_ESCAPE:
				PostQuitMessage( 0 );
				break;
			case VK_SPACE:
				//t0=timeGetTime()/1000.0f;
				animSet[0].speed--;
				break;
				
				break;
/*			case 'S':case 's':
				speed++;
				break;
			case 'Z':case 'z':
				rotate--;
				break;
			case 'X':case 'x':
				rotate++;
				break;
*/
			case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':case '0':
				
				{
					int idx=wParam-'1';
					if(idx<0) idx=9;
					animSet[idx].flags^=1;
					animSet[idx].rott=0;
					animSet[idx].zoomt=0;
				}

				break;
			}

//			printf("rot %2d speed %2d\n", (int)rotate, (int)speed);

			//TranslateMessage(&msg);
            return 0;
			
        case WM_DESTROY:
            PostQuitMessage( 0 );
            return 0;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}


#endif