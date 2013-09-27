//-----------------------------------------------------------------------------
// File: WMAStream.cpp
//
// Desc: Class for streaming wave file playback.
//
// Hist: 12.15.00 - New for December XDK release
//       03.15.01 - Updated for April XDK
//       07.12.02 - Updated for August 2002 XDK
//       10.04.02 - Cleaned up for the November 2002 XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <assert.h>
#include <xtl.h>
#include "WMAStream.h"




//-----------------------------------------------------------------------------
// Name: CWMAFileStream()
// Desc: Object constructor.
//-----------------------------------------------------------------------------
CWMAFileStream::CWMAFileStream()
{
    m_pSourceXMO        = NULL;
    m_pDestXMO          = NULL;
    m_pvSourceBuffer    = NULL;
    m_hrOpenResult      = E_PENDING;
    m_dwFileProgress    = 0;
}





//-----------------------------------------------------------------------------
// Name: ~CWMAFileStream()
// Desc: Object destructor.
//-----------------------------------------------------------------------------
CWMAFileStream::~CWMAFileStream()
{
    if( m_pSourceXMO )
        m_pSourceXMO->Release();

    if( m_pDestXMO )
        m_pDestXMO->Release();

    if( m_pvSourceBuffer )
        delete[] m_pvSourceBuffer;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initializes the WMA file streaming object
//-----------------------------------------------------------------------------
HRESULT CWMAFileStream::Initialize( const CHAR* strFileName )
{
    // Allocate data buffers.  The source XMO (WMA decoder) is synchronous, 
    // so we only need to worry about having enough buffer space for the 
    // destination XMO (DirectSound Stream).  
    m_pvSourceBuffer = new BYTE[ WMASTRM_SOURCE_PACKET_BYTES * WMASTRM_PACKET_COUNT ];
    if( NULL == m_pvSourceBuffer )
        return E_OUTOFMEMORY;

    // Create the decoder
    WMAXMODECODERPARAMETERS Params = {0};
    Params.pszFileName             = strFileName;
    Params.dwFileOffset            = 0;
    Params.dwLookaheadBufferSize   = WMASTRM_LOOKAHEAD_SIZE;

    // Note that this call could block, as it must call CreateFile.  To avoid
    // having this call block, the title can pass in an already-opened
    // file handle instead of a file name.
    HRESULT hr = XWmaDecoderCreateMediaObject( (LPCWMAXMODECODERPARAMETERS)&Params, &m_pSourceXMO );
    if( FAILED(hr) )
    {
        m_hrOpenResult = E_FAIL;
        return hr;
    }


    // Set the decoder status, so that Process() knows
    // to create the DSound stream with the correct
    // wave format when the decoder is ready
    m_dwLastDecoderStatus = 0;

    // Initialize packet status array
    ZeroMemory( m_adwStatus, sizeof(m_adwStatus) );

    return S_OK;
}


extern int startTime;

//-----------------------------------------------------------------------------
// Name: CreateStream()
// Desc: Creates the Direct Sound stream
//-----------------------------------------------------------------------------
HRESULT CWMAFileStream::CreateStream()
{
    // Get the total wave file duration.  We'll use this to calculate how far
    // along in the file we are during processing.
    HRESULT hr = m_pSourceXMO->GetFileHeader( &m_WmaFileHeader );
    if( FAILED( hr ) )
    {
        m_hrOpenResult = E_FAIL;
        return hr;
    }

    // get some info about the wma content
    WMAXMOFileContDesc desc = {0};
    desc.pTitle             = m_strWmaTitle;
    desc.wTitleLength       = sizeof( m_strWmaTitle );
    desc.pAuthor            = m_strWmaAuthor;
    desc.wAuthorLength      = sizeof( m_strWmaAuthor );
    desc.pDescription       = m_strWmaDescription;
    desc.wDescriptionLength = sizeof( m_strWmaDescription );

    hr = m_pSourceXMO->GetFileContentDescription( &desc );
    if( FAILED( hr ) )
    {
        m_hrOpenResult = E_FAIL;
        return hr;
    }

    // Create an appropriate WAVEFORMATEX struct
    XAudioCreatePcmFormat( (WORD)m_WmaFileHeader.dwNumChannels, m_WmaFileHeader.dwSampleRate,
                           (WORD)16, &m_wfxSourceFormat );

    // Create the stream
    DSSTREAMDESC   dssd;
    ZeroMemory( &dssd, sizeof( dssd ) );
    dssd.dwMaxAttachedPackets = WMASTRM_PACKET_COUNT;
    dssd.lpwfxFormat          = &m_wfxSourceFormat;

    hr = DirectSoundCreateStream( &dssd, &m_pDestXMO );
    if( FAILED(hr) )
    {
        m_hrOpenResult = E_FAIL;
        return hr;
    }

#if _DEBUG
    // We expect the source XMO to be synchronous and read-only and the 
    // destination XMO to be asynchronous write-only.  Assert that all 
    // of this is true and check the packet sizes for compatibility.
    XMEDIAINFO xmi = {0};
    hr = m_pSourceXMO->GetInfo( &xmi );
    if( FAILED(hr) )
    {
        m_hrOpenResult = E_FAIL;
        return hr;
    }

    assert( xmi.dwFlags & XMO_STREAMF_FIXED_SAMPLE_SIZE );
    assert( xmi.dwMaxLookahead == 0 );
    assert( xmi.dwOutputSize );
    assert( xmi.dwInputSize == 0 );
    assert( WMASTRM_SOURCE_PACKET_BYTES % xmi.dwOutputSize == 0 );

    hr = m_pDestXMO->GetInfo( &xmi );
    if( FAILED(hr) )
    {
        m_hrOpenResult = E_FAIL;
        return hr;
    }

    assert( xmi.dwFlags == ( XMO_STREAMF_FIXED_SAMPLE_SIZE | XMO_STREAMF_INPUT_ASYNC ) );
    assert( WMASTRM_SOURCE_PACKET_BYTES * WMASTRM_PACKET_COUNT >= xmi.dwMaxLookahead );
    assert( WMASTRM_SOURCE_PACKET_BYTES % xmi.dwInputSize == 0 );
    assert( xmi.dwOutputSize == 0 );
#endif // _DEBUG

    m_hrOpenResult = S_OK;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Process()
// Desc: Performs any work necessary to keep the stream playing.
//-----------------------------------------------------------------------------
HRESULT CWMAFileStream::Process( DWORD* pdwPercentCompleted /* = NULL */ )
{
    DWORD   dwPacketIndex;
    HRESULT hr;

    // Let the decoder do some work - this is where it schedules its
    // asynchronous file I/O.
    m_pSourceXMO->DoWork();

    // Since the WMA header is read asynchronously, we need to wait 
    // until the decoder says it's ready to produce output.  Once
    // the decoder says it's ready, we can create our stream and
    // start playback.  It makes things a little more complex, but
    // it's better than blocking every time we start streaming a
    // new WMA file.
    if( ( m_dwLastDecoderStatus & XMO_STATUSF_ACCEPT_OUTPUT_DATA ) == 0 )
    {
        hr = m_pSourceXMO->GetStatus( &m_dwLastDecoderStatus );
        if( FAILED( hr ) )
            return hr;

        // The decoder still isn't ready - bail
        if( ( m_dwLastDecoderStatus & XMO_STATUSF_ACCEPT_OUTPUT_DATA ) == 0 )
            return S_OK;

        // If we get here, the decoder is ready to produce output.  
        // Create our stream and start processing.
        if( FAILED( CreateStream() ) )
            return E_FAIL;
    }

    // Find a free packet, re-fill with decoded data and submit to the stream.  
    // If there are no free packets, we don't have anything to do.
    while( FindFreePacket( &dwPacketIndex ) )
    {
        // Grab data from the source XMO (WMA decoder)
        hr = ProcessSource( dwPacketIndex );
        if( FAILED(hr) )
            return hr;

        // Send the data to the destination XMO (DirectSound Stream)
        hr = ProcessRenderer( dwPacketIndex );
        if( FAILED(hr) )
            return hr;
    }

    // Calculate the completion percentage based on the total amount of
    // data we've played back.
    if( pdwPercentCompleted )
    {
        FLOAT fMsecCompleted = m_dwFileProgress * 1000.0f / m_wfxSourceFormat.nAvgBytesPerSec;
        (*pdwPercentCompleted) = DWORD( 100 * fMsecCompleted / m_WmaFileHeader.dwDuration );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FindFreePacket()
// Desc: Finds a render packet available for processing.
//-----------------------------------------------------------------------------
BOOL CWMAFileStream::FindFreePacket( DWORD* pdwPacketIndex )
{
    for( DWORD dwPacketIndex = 0; dwPacketIndex < WMASTRM_PACKET_COUNT; dwPacketIndex++ )
    {
        if( XMEDIAPACKET_STATUS_PENDING != m_adwStatus[ dwPacketIndex ] )
        {
            if( pdwPacketIndex )
                (*pdwPacketIndex) = dwPacketIndex;

            return TRUE;
        }
    }

    return FALSE;
}




//-----------------------------------------------------------------------------
// Name: ProcessSource()
// Desc: Reads data from the source XMO (WMA decoder).
//-----------------------------------------------------------------------------
HRESULT CWMAFileStream::ProcessSource( DWORD dwPacketIndex )
{
    DWORD        dwTotalSourceUsed = 0;
    DWORD        dwSourceUsed;
    HRESULT      hr;
    
    // We're going to read a full packet's worth of data into the source
    // buffer.  Since we're playing in an infinite loop, we'll just spin
    // until we've read enough data, even if that means wrapping around the
    // end of the file.
    XMEDIAPACKET xmp     = {0};
    xmp.pvBuffer         = (BYTE*)m_pvSourceBuffer + (dwPacketIndex * WMASTRM_SOURCE_PACKET_BYTES);
    xmp.dwMaxSize        = WMASTRM_SOURCE_PACKET_BYTES;
    xmp.pdwCompletedSize = &dwSourceUsed;
    xmp.pdwStatus        = &m_adwStatus[ dwPacketIndex ];

    while( dwTotalSourceUsed < WMASTRM_SOURCE_PACKET_BYTES )
    {
        // Read from the source
        hr = m_pSourceXMO->Process( NULL, &xmp );
        if( FAILED(hr) )
            return hr;

        // Add the amount read to the total
        dwTotalSourceUsed += dwSourceUsed;

        // If we read less than the amount requested, it's because we hit
        // the end of the file.  Seek back to the start and keep going.
        if( dwSourceUsed < xmp.dwMaxSize )
        {
            xmp.pvBuffer  = (BYTE*)xmp.pvBuffer + dwSourceUsed;
            xmp.dwMaxSize = xmp.dwMaxSize - dwSourceUsed;
            
            hr = m_pSourceXMO->Flush();
            if( FAILED(hr) )
                return hr;

            m_dwFileProgress = 0;
        };
    }

    // Update the file progress. We keep track in bytes and do the bytes->ms
    // and/or ms->percentage calculation at the last step, so as not to 
    // lose precision
    m_dwFileProgress += dwTotalSourceUsed;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ProcessRenderer()
// Desc: Sends data to the destination XMO (DirectSound stream).
//-----------------------------------------------------------------------------
HRESULT CWMAFileStream::ProcessRenderer( DWORD dwPacketIndex )
{
    // There's a full packet's worth of data ready for us to send to the
    // renderer.  We want to track the status of this packet since the
    // destination XMO is asynchronous and we need to know when the packet is
    // completed.
    XMEDIAPACKET xmp = {0};
    xmp.pvBuffer     = (BYTE*)m_pvSourceBuffer + (dwPacketIndex * WMASTRM_SOURCE_PACKET_BYTES );
    xmp.dwMaxSize    = WMASTRM_SOURCE_PACKET_BYTES;
    xmp.pdwStatus    = &m_adwStatus[ dwPacketIndex ];

    // Submit for playback
    return m_pDestXMO->Process( &xmp, NULL );
}




//-----------------------------------------------------------------------------
// Name: Pause()
// Desc: Pauses and resumes stream playback
//-----------------------------------------------------------------------------
VOID CWMAFileStream::Pause( DWORD dwPause )
{
    m_pDestXMO->Pause( dwPause );
}

VOID CWMAFileStream::Seek( DWORD time, LPDWORD timeActual )
{
/*    DWORD        dwTotalSourceUsed = 0;
    DWORD        dwSourceUsed;
    HRESULT      hr;

	XMEDIAPACKET xmp     = {0};
    xmp.pvBuffer         = (BYTE*)m_pvSourceBuffer + (0 * WMASTRM_SOURCE_PACKET_BYTES);
    xmp.dwMaxSize        = WMASTRM_SOURCE_PACKET_BYTES;
    xmp.pdwCompletedSize = &dwSourceUsed;
    xmp.pdwStatus        = &m_adwStatus[ 0];

	for(int i=0;i<100;i++)
	hr = m_pSourceXMO->Process( NULL, &xmp );
*/
//	m_pSourceXMO->Seek(1024*1024, FILE_BEGIN, &xx);
//	m_pSourceXMO->SeekToTime(time, timeActual);
}