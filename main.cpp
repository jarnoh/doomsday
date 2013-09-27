#include <XBApp.h>
#include <XBFont.h>
#include <XBSound.h>
#include <XBUtil.h>
#include "WMAStream.h"
//#include "dsstdfx.h"

#include "plattis.h"


LPDIRECT3DSURFACE8 main_back;
LPDIRECT3DSURFACE8 main_depth;

int nosound=-1;


//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Application class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
	CWMAFileStream m_Stream;        // The WMA stream
	CXBFont        m_Font;          // A font to render text
	// CXBHelp        m_Help;          // Help object
	BOOL           m_bDrawHelp;     // Should we draw help?
	DWORD          m_dwPercentCompleted;    // Percentage of file processed

	BOOL           m_bPaused;       // Paused?

	LPDIRECTSOUND8 m_pDSound;       // DirectSound object

	HRESULT DownloadEffectsImage( CHAR* strScratchFile );  // downloads a default DSP image to the GP

public:
	virtual HRESULT Initialize();
	virtual HRESULT Render();
	virtual HRESULT FrameMove();

	CXBoxSample();
};




//-----------------------------------------------------------------------------
// Name: Main()
// Desc: Entry point to the program.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{
	CXBoxSample xbApp;
	if( FAILED( xbApp.Create() ) )
		return;
	xbApp.Run();
}

float angles[256];


//-----------------------------------------------------------------------------
// Name: CXBoxSample()
// Desc: Constructor
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
{
//	m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	//    m_d3dpp.Flags            = D3DPRESENTFLAG_FIELD;
	//	m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_TWO ;
	/*
	if( XGetVideoStandard() == XC_VIDEO_STANDARD_PAL_I )
	{
	m_d3dpp.BackBufferWidth  = 640;
	m_d3dpp.BackBufferHeight = 288;
	}
	else
	{
	m_d3dpp.BackBufferWidth  = 640;
	m_d3dpp.BackBufferHeight = 240;
	}
	*/

}


//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Performs whatever initialization the sample needs
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{


	main_back=this->m_pBackBuffer;
	main_depth=this->m_pDepthBuffer;

	for(int i=0;i<128;i++)
	{
		angles[i]=10+i*.1;
	}

	//return E_FAIL;

	// Create a font
	if( FAILED( m_Font.Create( "Font.xpr" ) ) )
		return XBAPPERR_MEDIANOTFOUND;
	// Create the DirectSound object
	if( FAILED( DirectSoundCreate( NULL, &m_pDSound, NULL ) ) )
		return E_FAIL;
	m_bDrawHelp = FALSE;
	m_bPaused = FALSE;

	m_dwPercentCompleted = 0;

	demo_init();

	g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0xff000000, 1.0f, 0L );
	m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0xff000000, 1.0f, 0L );

	WRAP(CreateSecondaryRenderTarget());

	SetSecondaryRenderTarget();
	g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0xff000000, 1.0f, 0L );
	m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0xff000000, 1.0f, 0L );
	RenderSecondary(main_back, main_depth);


	MEMORYSTATUS stat;
    // Get the memory status.
    GlobalMemoryStatus( &stat );

//	m_Stream.Seek(startTime, (LPDWORD)&startTime);

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame to update state
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{

	// Create a WMA stream
	if(nosound==-1)
	{
		WRAP( m_Stream.Initialize( "d:\\media\\demo.wma" ) );
		nosound=0;
	}

	// Let the WMA decoder do some work
	if(nosound==0)
	{
		m_Stream.Process( &m_dwPercentCompleted );
		DirectSoundDoWork();
	}

	/*
	// Toggle help
	if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK ) 
	{
	m_bDrawHelp = !m_bDrawHelp;
	}

	// Toggle pause
	if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_A ] )
	{
	m_bPaused = !m_bPaused;
	m_Stream.Pause( m_bPaused ? DSSTREAMPAUSE_PAUSE : DSSTREAMPAUSE_RESUME );
	}
	*/

	return S_OK;
}




extern void BlurSecondary();

extern int animCount;
extern int zoomer_blend;

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Renders the scene
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
	static float tt=0;

	if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_START )
	{
//		tt=this->m_fAppTime;
		angles[250]++;
	}

	if( m_DefaultGamepad.bPressedAnalogButtons [ XINPUT_GAMEPAD_WHITE ] )
	{ zoomer_blend++; angles[251]++; }

	if( m_DefaultGamepad.bPressedAnalogButtons [ XINPUT_GAMEPAD_BLACK] )
		angles[252]++;

	if( m_DefaultGamepad.bPressedAnalogButtons [ XINPUT_GAMEPAD_X ] )
		angles[253]++;

	if( m_DefaultGamepad.bPressedAnalogButtons [ XINPUT_GAMEPAD_B ] )
		angles[254]++;

	if( m_DefaultGamepad.bPressedAnalogButtons [ XINPUT_GAMEPAD_A ] )
		angles[255]++;

#if 1

	float h0=
		-1*m_DefaultGamepad.bPressedAnalogButtons [ XINPUT_GAMEPAD_LEFT_TRIGGER ]+
		1*m_DefaultGamepad.bPressedAnalogButtons [ XINPUT_GAMEPAD_RIGHT_TRIGGER ];

	float h1=
		-1*m_DefaultGamepad.bPressedAnalogButtons [ XINPUT_GAMEPAD_Y ]+
		1*m_DefaultGamepad.bPressedAnalogButtons [ XINPUT_GAMEPAD_X ];
/*
	int i=((int)angles[254])%animCount;
	animSet[i].speed=angles[0*8+0]+=m_DefaultGamepad.fY1*.2f;
	animSet[i].rotate=angles[0*8+1]+=m_DefaultGamepad.fX1*.2f;
	animSet[i].hue=angles[0*8+2]+=h0;

	i=((int)angles[255])%animCount;
	animSet[i].speed=angles[1*8+0]+=m_DefaultGamepad.fY2*.2f;
	animSet[i].rotate=angles[1*8+1]+=m_DefaultGamepad.fX2*.2f;
	animSet[i].hue=angles[1*8+2]+=h1;
*/
#endif
	demo_render(this->m_fAppTime-tt);
#if 0
	WCHAR buf1[256];
	WCHAR buf2[256];
	WCHAR buf3[256];


	swprintf(buf1, L"blend:%4d first %4d  second%4d ", zoomer_blend&3, ((int)angles[254])%animCount, ((int)angles[255])%animCount);
	swprintf(buf2, L"%4d %4d %4d", // %2x %2x %2x %2x %2x ", 
		(int)(4*angles[0*8+0]), 
		(int)(4*angles[0*8+1]), 
		(int)(4*angles[0*8+2]));
	swprintf(buf3, L"%4d %4d %4d", // %2x %2x %2x %2x %2x ", 
		(int)(4*angles[1*8+0]), 
		(int)(4*angles[1*8+1]), 
		(int)(4*angles[1*8+2]));

#endif
#if 0
	m_Font.Begin();
	//        m_Font.SetScaleFactors( 1.2f, 1.2f );
	//        m_Font.DrawText( 62, 45, 0xffffffff,  L"Magic Meat" );
	m_Font.SetScaleFactors( 1.0f, 1.0f );
	m_Font.DrawText( 62, 360, 0xffffffff, buf1, XBFONT_LEFT);
	m_Font.DrawText( 62, 400, 0xffff0000+0xff00, buf2, XBFONT_LEFT);
	m_Font.DrawText( 62, 440, 0xffff0000+0xff00, buf3, XBFONT_LEFT);

	swprintf(buf1, L"%4.4f", this->m_fAppTime);

	m_Font.DrawText( 568, 400, 0xffffff00, buf1, XBFONT_RIGHT );
	m_Font.DrawText( 568, 440, 0xffffff00, m_strFrameRate, XBFONT_RIGHT );
	m_Font.End();




	// Draw the on-screen audio level meters
	XBSound_DrawLevelMeters( m_pDSound, 0, 00.0f, 60.0f, 480.0f );
#endif
	// Present the scene
	m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

	return S_OK;
}
