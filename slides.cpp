#include "plattis.h"
#include <MATH.h>

#define m_pd3dDevice g_pd3dDevice
#define pd3dDevice g_pd3dDevice

LPDIRECT3DTEXTURE8      slides_tex= NULL;
LPDIRECT3DTEXTURE8      tex_scroll= NULL;

float scroller_flash=0;

int slides_texture=0;
int slides_current_texture=-1;

char *slideshow[]={
	"d:\\media\\credits01.png",
	"d:\\media\\credits02.png",
	"d:\\media\\sleng1.jpg",
	"d:\\media\\sleng2.jpg",
	"d:\\media\\sleng3.jpg",
	"d:\\media\\sleng4.jpg",
	"d:\\media\\krediitit.jpg",
};

void slides_init()
{
	slides_current_texture=-1;
	WRAP(D3DXCreateTextureFromFile( g_pd3dDevice, "d:\\media\\sarvihhh.dds", &tex_scroll));
}


DWORD WINAPI slides_decode( LPVOID lpParam ) 
{ 
	WRAP(D3DXCreateTextureFromFile( g_pd3dDevice, slideshow[slides_current_texture%7], &slides_tex));
    return 0; 
} 


void slides_render(float t)
{
	Sleep(10);

	if(slides_current_texture!=slides_texture)
	{
		if(slides_tex)
		{
			slides_tex->Release();
			slides_tex=NULL;
		}
		slides_current_texture=slides_texture;

		CreateThread(NULL, 0, slides_decode, NULL, 0, NULL);
	}


	g_pd3dDevice->BeginScene();
	g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0xff000000, 1.0f, 0L );

    // Render the secondary color surface to the screen
    struct VERTEX { D3DXVECTOR4 p; FLOAT tu, tv; 
	};
    VERTEX v[4];
    v[0].p = D3DXVECTOR4(   0 - 0.5f,  48 - 0.5f, 0, 0 );  v[0].tu =   0; v[0].tv =   0;
    v[1].p = D3DXVECTOR4( 640 - 0.5f,  48 - 0.5f, 0, 0 );  v[1].tu = 1; v[1].tv =   0;
    v[2].p = D3DXVECTOR4( 640 - 0.5f, 432 - 0.5f, 0, 0 );  v[2].tu = 1; v[2].tv =1;
    v[3].p = D3DXVECTOR4(   0 - 0.5f, 432 - 0.5f, 0, 0 );  v[3].tu =   0; v[3].tv = 1;
	m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_TEX1);

    m_pd3dDevice->SetTexture( 0, slides_tex );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER , D3DTEXF_LINEAR);
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER , D3DTEXF_LINEAR);

	int color=CLAMP(fade)*255;
	m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xff<<24|(0x010101*color));

	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,   FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);

	if(slides_tex)
	    WRAP(m_pd3dDevice->DrawPrimitiveUP( D3DPT_QUADLIST, 1, v, 6*sizeof(FLOAT) ));

    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
    m_pd3dDevice->SetTexture( 0, NULL );


	g_pd3dDevice->EndScene();


	//   g_pd3dDevice->Present( NULL, NULL, NULL, NULL );


}


void scroll_render(float t)
{
	g_pd3dDevice->BeginScene();
	g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0xff000000, 1.0f, 0L );

    // Render the secondary color surface to the screen
    struct VERTEX { D3DXVECTOR4 p; FLOAT tu, tv; 
	};
    VERTEX v[4];
	float p=(5-t)*.05f+.1f;
    v[0].p = D3DXVECTOR4(   0 - 0.5f,   0 - 0.5f, 0, 0 );  v[0].tu = p; v[0].tv = 0;
    v[1].p = D3DXVECTOR4( 640 - 0.5f,   0 - 0.5f, 0, 0 );  v[1].tu = .3f+p; v[1].tv = 0;
    v[2].p = D3DXVECTOR4( 640 - 0.5f, 480 - 0.5f, 0, 0 );  v[2].tu = .3f+p; v[2].tv = 1;
    v[3].p = D3DXVECTOR4(   0 - 0.5f, 480 - 0.5f, 0, 0 );  v[3].tu = p; v[3].tv = 1;
	m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_TEX1);

    m_pd3dDevice->SetTexture( 0, tex_scroll );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);

	if(scroller_flash>0)
	{
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_ADD );
	int color=CLAMP(scroller_flash)*255;
	m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xff<<24|(0x010101*color));
	}
	else
	{
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	int color=CLAMP(fade)*255;
	m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xff<<24|(0x010101*color));
	}
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER , D3DTEXF_LINEAR);
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER , D3DTEXF_LINEAR);


	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,   FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);

    WRAP(m_pd3dDevice->DrawPrimitiveUP( D3DPT_QUADLIST, 1, v, 6*sizeof(FLOAT) ));

    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
    m_pd3dDevice->SetTexture( 0, NULL );


	g_pd3dDevice->EndScene();


}