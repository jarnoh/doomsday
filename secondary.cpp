
#include "plattis.h"

#define m_pd3dDevice g_pd3dDevice

LPDIRECT3DTEXTURE8       m_pSecondaryTexture;
LPDIRECT3DSURFACE8       m_pSecondaryColorSurface;
LPDIRECT3DSURFACE8       m_pSecondaryDepthSurface;
VOID*                    m_pColorBufferMemory; 
VOID*                    m_pDepthBufferMemory;

float fade=1;

float motionblur=0;
float radialblur=0;


static void radialblur_zoom(DWORD dest[], double factor, double centerx, double centery)
{
	centery = (int)centery;
	if(centerx < 0.0)
		centerx = 0.0;
	if(centery < 0.0)
		centery = 0.0;
	int width = 640;
	int height = 480;
	int rowstride = 640;
	int off = 0;
	int cx = (int)centerx;
	int cy = (int)centery;

	factor=1;
	int ifactor = (int)(factor * 65536);
	int cx0 = (int)(centerx * (double)ifactor);
	int cy0 = (int)(centery * (double)ifactor);
	int scx = (int)(centerx * 65536);
	int scy = (int)(centery * 65536);
	for(int y = cy; y >= 0; y--)
	{
		int alpha=255<<24;
		int od = y * rowstride;
		for(int x = cx; x >= 0; x--)
		{
			int xx = scx + (x * ifactor - cx0);
			int yy = scy + (y * ifactor - cy0);
			int fx = xx >> 8 & 0xff;
			int fy = yy >> 8 & 0xff;
			int o = (yy >> 16) * rowstride + (xx >> 16);
			int c2 = (dest[o]&255) * (256 - fx) * (256 - fy) + 
				(dest[o + 1]&255) * fx * (256 - fy) + 
				(dest[o + rowstride]&255) * (256 - fx) * fy + 
				(dest[o + rowstride + 1]&255) * fx * fy;
			dest[od + x] = alpha;//|(255&((dest[od + x] << 16) + c2 * 31 >> 21));
		}
		/*
		for(int x = cx + 1; x < width; x++)
		{
		int xx = scx + (x * ifactor - cx0);
		int yy = scy + (y * ifactor - cy0);
		int fx = xx >> 8 & 0xff;
		int fy = yy >> 8 & 0xff;
		int o = (yy >> 16) * rowstride + (xx >> 16);

		int c2 = dest[o] * (256 - fx) * (256 - fy) + dest[o + 1] * fx * (256 - fy) + dest[o + 512] * (256 - fx) * fy + dest[o + 512 + 1] * fx * fy;
		dest[od + x] = alpha|((dest[od + x] << 16) + c2 * 31 >> 21);
		}
		*/
	}
#if 0
	for(int y = cy + 1; y < height; y++)
	{
		int od = y * rowstride;
		for(int x = cx; x >= 0; x--)
		{
			int xx = scx + (x * ifactor - cx0);
			int yy = scy + (y * ifactor - cy0);
			int o = (yy >> 16) * rowstride + (xx >> 16);
			int fx = xx >> 8 & 0xff;
			int fy = yy >> 8 & 0xff;
			int c2 = dest[o] * (256 - fx) * (256 - fy) + dest[o + 1] * fx * (256 - fy) + dest[o + 512] * (256 - fx) * fy + dest[o + 512 + 1] * fx * fy;
			dest[od + x] = (byte)((dest[od + x] << 16) + c2 * 31 >> 21);
		}

		for(int x = cx + 1; x < width; x++)
		{
			int xx = scx + (x * ifactor - cx0);
			int yy = scy + (y * ifactor - cy0);
			int fx = xx >> 8 & 0xff;
			int fy = yy >> 8 & 0xff;
			int o = (yy >> 16) * rowstride + (xx >> 16);
			int c2 = (dest[o]&255) * (256 - fx) * (256 - fy) + dest[o + 1] * fx * (256 - fy) + dest[o + 512] * (256 - fx) * fy + dest[o + 512 + 1] * fx * fy;
			dest[od + x] = (byte)((dest[od + x] << 16) + c2 * 31 >> 21);
		}

	}
#endif
}



void BlurSecondary()
{
	g_pd3dDevice->BlockUntilIdle();

	DWORD *p=(DWORD*)m_pColorBufferMemory;
	for(int i=0;i<640*480;i++)
	{
		p[i]+=i;
	}

	radialblur_zoom(p, .99, 320, 240);
}


//-----------------------------------------------------------------------------
// Name: CreateSecondaryRenderTarget()
// Desc: Creates a texture and depth buffer to render into
//-----------------------------------------------------------------------------
HRESULT CreateSecondaryRenderTarget()
{
	DWORD dwWidth  = 640;
	DWORD dwHeight = 480;

	// Create a new 32-bit color surface using allocated tile-able memory
	m_pSecondaryTexture = new D3DTexture;
	//    XGSetTextureHeader( dwWidth, dwHeight, 1, 0, D3DFMT_LIN_A8R8G8B8, 0, m_pSecondaryTexture, 0, 0 );
	XGSetTextureHeader( dwWidth, dwHeight, 1, 0, D3DFMT_LIN_A8R8G8B8, 0, m_pSecondaryTexture, 0, 0 );
	m_pColorBufferMemory = D3D_AllocContiguousMemory( 4*dwWidth*dwHeight, D3DTILE_ALIGNMENT );
	m_pSecondaryTexture->Register( m_pColorBufferMemory );

	m_pSecondaryTexture->GetSurfaceLevel( 0, &m_pSecondaryColorSurface );

	// Create a new 32-bit depth surface using allocated tile-able memory
	m_pSecondaryDepthSurface = new D3DSurface;
	XGSetSurfaceHeader( dwWidth, dwHeight, D3DFMT_LIN_D24S8, m_pSecondaryDepthSurface, 0, 0 );
	m_pDepthBufferMemory = D3D_AllocContiguousMemory( 4*dwWidth*dwHeight, 
		D3DTILE_ALIGNMENT );
	m_pSecondaryDepthSurface->Register( m_pDepthBufferMemory );

	return S_OK;
}


HRESULT SetShader( LPDIRECT3DDEVICE8 pd3dDevice )
{
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	//   pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_ADDSMOOTH );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	/*
	DWORD dwNumPasses;
	return pd3dDevice->ValidateDevice( &dwNumPasses );
	*/

	return S_OK;
}


void SetSecondaryRenderTarget()
{
	m_pd3dDevice->SetRenderTarget( m_pSecondaryColorSurface, m_pSecondaryDepthSurface);
}

void SecondaryBlend(int a)
{

}

// If we rendered the scene to a secondary render target, then we now need
// to render the texture to the primary backbuffer.
void RenderSecondary(LPDIRECT3DSURFACE8 m_pBackBuffer, LPDIRECT3DSURFACE8 m_pDepthBuffer, int blend)
{
	// Set the render target back to be the app's main backbuffer
	m_pd3dDevice->SetRenderTarget( m_pBackBuffer, m_pDepthBuffer );

	// Render the secondary color surface to the screen
	struct VERTEX { D3DXVECTOR4 p; FLOAT tu, tv; 
	};
	VERTEX v[4];
	v[0].p = D3DXVECTOR4(   0 - 0.5f,   0 - 0.5f, 0, 0 );  v[0].tu =   0; v[0].tv =   0;
	v[1].p = D3DXVECTOR4( 640 - 0.5f,   0 - 0.5f, 0, 0 );  v[1].tu = 640; v[1].tv =   0;
	v[2].p = D3DXVECTOR4( 640 - 0.5f, 480 - 0.5f, 0, 0 );  v[2].tu = 640; v[2].tv = 480;
	v[3].p = D3DXVECTOR4(   0 - 0.5f, 480 - 0.5f, 0, 0 );  v[3].tu =   0; v[3].tv = 480;
	m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_TEX1);

	m_pd3dDevice->SetTexture( 0, m_pSecondaryTexture );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

	//   pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER , D3DTEXF_LINEAR);
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER , D3DTEXF_LINEAR);

	float mb=1-CLAMP(motionblur);
	m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, ((int)(mb*255))<<24);

	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,   FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );

	int i=0;

	int step=CLAMP(radialblur)*100+1;
/*
	if(motionblur==0)
		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
*/
	m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, ((int)(mb*255))<<24);
	g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	WRAP(m_pd3dDevice->DrawPrimitiveUP( D3DPT_QUADLIST, 1, v, 6*sizeof(FLOAT) ));

	g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	for(int i=step;i>0;i=i-7)
	{
		v[0].tu =   0+i; v[0].tv =   0+i;
		v[1].tu = 640-i; v[1].tv =   0+i;
		v[2].tu = 640-i; v[2].tv = 480-i/3;
		v[3].tu =   0+i; v[3].tv = 480-i/3;

		m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, (i/8+3)<<24);
		WRAP(m_pd3dDevice->DrawPrimitiveUP( D3DPT_QUADLIST, 1, v, 6*sizeof(FLOAT) ));
	}

	m_pd3dDevice->SetPixelShader(NULL);
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
}