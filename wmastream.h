//-----------------------------------------------------------------------------
// File: WMAStream.h
//
// Desc: Streaming wave file playback - demonstrates how to run the WMA
//       decoder synchronously on a separate thread
//
// Hist: 12.15.00 - New for December XDK release
//       03.15.01 - Updated for April XDK 
//       10.04.02 - Cleaned up for November 2002 XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef WMASTREAM_H
#define WMASTREAM_H

// the WMA decoder will work properly in async mode, with all formats
// only when using a look ahead size of at least 64k.
const DWORD WMASTRM_LOOKAHEAD_SIZE = 64 * 1024 *2;

// Define the maximum amount of packets we will ever submit to the renderer
const DWORD WMASTRM_PACKET_COUNT = 2;

// Define the source packet size:
// This value is hard-coded assuming a WMA file of stereo, 16bit resolution.  If
// this Value can by dynamically set based on the WMA format, keeping in mind
// that WMA needs enough buffer for a minimum of 2048 samples worth of PCM data
const DWORD WMASTRM_SOURCE_PACKET_BYTES = 2048 * 2 * 2 * 2;

const DWORD WMACONTENTDESC_STRING_SIZE = 256;

//-----------------------------------------------------------------------------
// Name: class CWMAFileStream
// Desc: WMA file streaming object
//-----------------------------------------------------------------------------
class CWMAFileStream
{
protected:
    XWmaFileMediaObject*    m_pSourceXMO;                       // Source (WMA file) XMO
    IDirectSoundStream*     m_pDestXMO;                         // Destination (DirectSoundStream) XMO
    LPVOID                  m_pvSourceBuffer;                   // Source data buffer
    DWORD                   m_adwStatus[WMASTRM_PACKET_COUNT];  // Packet status array
    DWORD                   m_dwFileProgress;                   // File progress, in bytes
    DWORD                   m_dwLastDecoderStatus;              // last known wma decoder status

    HRESULT                 m_hrOpenResult;                     // Result of opening file
    WAVEFORMATEX            m_wfxSourceFormat;                  // Wave format

    // Packet processing
    BOOL    FindFreePacket(DWORD* pdwPacketIndex );
    HRESULT ProcessSource(DWORD dwPacketIndex);
    HRESULT ProcessRenderer( DWORD dwPacketIndex );

    // Stream creation
    HRESULT CreateStream();

public:

    WMAXMOFileHeader    m_WmaFileHeader;                         // basic information about the WMA file

    WCHAR               m_strWmaTitle[WMACONTENTDESC_STRING_SIZE];          // WMA Title
    WCHAR               m_strWmaAuthor[WMACONTENTDESC_STRING_SIZE];         // WMA Author
    WCHAR               m_strWmaDescription[WMACONTENTDESC_STRING_SIZE];    // WMA Description

    // Processing
    HRESULT Process( DWORD* pdwPercentCompleted = NULL );

    // Initialization
    HRESULT Initialize( const CHAR* strFileName );
    HRESULT GetOpenResult() { return m_hrOpenResult; }
    
    // Play control
    VOID Pause( DWORD dwPause );
    VOID Seek( DWORD time, LPDWORD timeActual );

    CWMAFileStream();
    ~CWMAFileStream();
};

#endif // WMASTREAM_H
