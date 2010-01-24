/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: zcodec.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: tuubaaku $ $Date: 2009/02/14 02:51:13 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
   This library is free software; you can redistribute it and/or
   modify it under the terms of the Library GNU General Public
   version 2 of the License, or (at your option) version 3 or,
   at the discretion of KDE e.V (which shall act as a proxy as in
   section 14 of the GPLv3), any later version..

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.

 *
 ************************************************************************/

/*
 * Modified by Benjamin Cail for wv2
 */

// MARKER(update_precomp.py): autogen include statement, do not remove
//#include "precompiled_tools.hxx"

//#ifndef _STREAM_HXX
//#include <tools/stream.hxx>
//#endif
#include "olestream.h"

#ifndef _ZLIB_H
//#ifdef SYSTEM_ZLIB
#include "zlib.h"
//#else
//#include "zlib/zlib.h"
//#endif
#endif 

#ifndef _ZCODEC_HXX
#include "zcodec.hxx"
#endif
//#ifndef _RTL_CRC_H_
//#include <rtl/crc.h>
//#endif
//#ifndef _OSL_ENDIAN_H_
//#include <osl/endian.h>
//#endif

#include "wvlog.h" //give us wv2 logging capabilities

// -----------
// - Defines -
// -----------

//z_stream is a struct defined in zlib
//define PZSTREAM as a pointer to mpsC_Stream, which is
//cast as a pointer to a z_stream
#define PZSTREAM ((z_stream*) mpsC_Stream)

/* gzip flag byte */
#define GZ_ASCII_FLAG   0x01 /* bit 0 set: file probably ascii text */
#define GZ_HEAD_CRC     0x02 /* bit 1 set: header CRC present */
#define GZ_EXTRA_FIELD  0x04 /* bit 2 set: extra field present */
#define GZ_ORIG_NAME    0x08 /* bit 3 set: original file name present */
#define GZ_COMMENT      0x10 /* bit 4 set: file comment present */
#define GZ_RESERVED     0xE0 /* bits 5..7: reserved */

static int gz_magic[2] = { 0x1f, 0x8b }; /* gzip magic header */


// ----------
// - ZCodec -
// ----------

ZCodec::ZCodec( ULONG nInBufSize, ULONG nOutBufSize, ULONG nMemUsage )
    : mnCRC(0)
{
    wvlog << "Creating ZCodec object..." << std::endl;
    //initialize variables
    mnMemUsage = nMemUsage;
    mnInBufSize = nInBufSize;
    mnOutBufSize = nOutBufSize;
    //create a new z_stream
    mpsC_Stream = new z_stream;
}

ZCodec::ZCodec( void )
    : mnCRC(0)
{
    //set variables to defaults
    mnMemUsage = MAX_MEM_USAGE;
    mnInBufSize = DEFAULT_IN_BUFSIZE;
    mnOutBufSize = DEFAULT_OUT_BUFSIZE;
    //create new z_stream
    mpsC_Stream = new z_stream;
}

// ------------------------------------------------------------------------
														
ZCodec::~ZCodec()
{
	delete (z_stream*) mpsC_Stream;
}

// ------------------------------------------------------------------------

void ZCodec::BeginCompression( ULONG nCompressMethod )
{
	mbInit = 0;
	mbStatus = true;
	mbFinish = false;
	mpIStm = NULL;
        mpOStm = NULL;
	mnInToRead = 0xffffffff;
	mpInBuf = mpOutBuf = NULL;
	PZSTREAM->total_out = PZSTREAM->total_in = 0;
	mnCompressMethod = nCompressMethod;
	PZSTREAM->zalloc = ( alloc_func )0;
        PZSTREAM->zfree = ( free_func )0;
        PZSTREAM->opaque = ( voidpf )0;
	PZSTREAM->avail_out = PZSTREAM->avail_in = 0;
}

// ------------------------------------------------------------------------

long ZCodec::EndCompression(std::vector<U8>* outBuffer)
{
	long retvalue = 0;

        //various actions based on mbInit value...
	if ( mbInit != 0 )
	{
		if ( mbInit & 2 )	// 1->decompress, 3->compress
		{
			do
			{		
				ImplWriteBack(outBuffer);
			}
			while ( deflate( PZSTREAM, Z_FINISH ) != Z_STREAM_END );

			ImplWriteBack(outBuffer);

			retvalue = PZSTREAM->total_in;
			deflateEnd( PZSTREAM );
		}
		else 
		{
			retvalue = PZSTREAM->total_out;
			inflateEnd( PZSTREAM );
		}
		delete[] mpOutBuf;
		delete[] mpInBuf;
	}
	return ( mbStatus ) ? retvalue : -1;
}


// ------------------------------------------------------------------------

long ZCodec::Compress( OLEStreamReader& rIStm, OLEStreamWriter& rOStm )
{	
	long nOldTotal_In = PZSTREAM->total_in;
	
	if ( mbInit == 0 )
	{
            //set streams
	    mpIStm = &rIStm;
	    mpOStm = &rOStm;
	    ImplInitBuf( false );
	    mpInBuf = new BYTE[ mnInBufSize ];
	}
	while (( PZSTREAM->avail_in = mpIStm->read( PZSTREAM->next_in = mpInBuf, mnInBufSize )) != 0 )
	{
            //fix this if I ever use this function...
            if ( PZSTREAM->avail_out == 0 ) {
                //ImplWriteBack();
            }
            if ( deflate( PZSTREAM, Z_NO_FLUSH ) < 0 ) {
                mbStatus = false;
                break;
            }
	}
	return ( mbStatus ) ? (long)(PZSTREAM->total_in - nOldTotal_In) : -1;
}

// ------------------------------------------------------------------------

long ZCodec::Decompress( OLEStreamReader& rIStm, std::vector<U8>* outBuffer )
{
    wvlog << "Decompressing... (mnInToRead=" << mnInToRead << ",avail_in=" 
        << PZSTREAM->avail_in << ")" << std::endl;
	int err;
	ULONG	nInToRead;
	long	nOldTotal_Out = PZSTREAM->total_out;

	if ( mbFinish )	
		return PZSTREAM->total_out - nOldTotal_Out;

	if ( mbInit == 0 )
	{
            wvlog << "  decompression initialization" << std::endl;
		mpIStm = &rIStm;
		//mpOStm = &rOStm;
		ImplInitBuf( true );
		PZSTREAM->next_out = mpOutBuf = new BYTE[ PZSTREAM->avail_out = mnOutBufSize ];
	}
        //loop through all the data to be decompressed
	do
	{
            wvlog << "top of do-while loop; PZSTREAM->avail_out=" << PZSTREAM->avail_out
                << "; PZSTREAM->avail_in=" << PZSTREAM->avail_in << "; mnInToRead=" << mnInToRead << std::endl;
            //replenish in-buffer if needed
		if ( PZSTREAM->avail_in == 0 && mnInToRead )
		{		
                    //figure out how many bytes to read - whatever's left that can be handled by in-buffer
			nInToRead = ( mnInBufSize > mnInToRead ) ? mnInToRead : mnInBufSize;
                        //read some more bytes
                        wvlog << " trying to read " << nInToRead << " bytes from stream at " 
                            << mpIStm->tell() << std::endl;
                        //read nInToRead bytes into the next_in, which is mpInBuf
                        //and put number of bytes read into avail_in
			//PZSTREAM->avail_in = 
                        //this read() function doesn't return the number of bytes read...
                        bool read = mpIStm->read( PZSTREAM->next_in = mpInBuf, nInToRead );
                        if(!read)
                            wvlog << "Error reading bytes!" << std::endl;
                        PZSTREAM->avail_in = nInToRead;
                        //update the number of bytes left to read
			mnInToRead -= nInToRead;

                        //TODO fix CRC handling
			//if ( mnCompressMethod & ZCODEC_UPDATE_CRC )
			//	mnCRC = UpdateCRC( mnCRC, mpInBuf, nInToRead );

		}
                //actually perform the decompression, storing error code in err
                wvlog << "  inflate()" << std::endl;
		err = inflate( PZSTREAM, Z_NO_FLUSH );
                wvlog << "inflate() return code: " << err << std::endl;
		if ( err < 0 )
		{
			mbStatus = false;
			break;
		}
                //now write that decompressed data to the data vector
		ImplWriteBack(outBuffer);
	}		
	while ( ( err != Z_STREAM_END)  && ( PZSTREAM->avail_in || mnInToRead ) );
	
        //set the "finished" flag if we got the stream-end signal?
	if ( err == Z_STREAM_END ) 
		mbFinish = true;	
        wvlog << "  total_in=" << PZSTREAM->total_in << ",total_out=" << PZSTREAM->total_out << std::endl;
        //return code: -1 if mbStatus is false, otherwise # of bytes decompressed
	return ( mbStatus ) ? (long)(PZSTREAM->total_out - nOldTotal_Out) : -1;
}

// ------------------------------------------------------------------------

void ZCodec::ImplWriteBack( std::vector<U8>* outBuffer )
{
    ULONG nAvail = mnOutBufSize - PZSTREAM->avail_out;
    wvlog << "ImplWriteBack() nAvail=" << nAvail << std::endl;
	
    if ( nAvail )
    {
                //TODO fix CRC handling
		//if ( mbInit & 2 && ( mnCompressMethod & ZCODEC_UPDATE_CRC ) )
		//	mnCRC = UpdateCRC( mnCRC, mpOutBuf, nAvail );
		//mpOStm->write( PZSTREAM->next_out = mpOutBuf, nAvail );
            for(uint i = 0; i < nAvail; i++) {
                outBuffer->push_back( (U8) mpOutBuf[i]);
            }
            //reset PZSTREAM settings
	    PZSTREAM->avail_out = mnOutBufSize;
            PZSTREAM->next_out = mpOutBuf;
    }
}

// ------------------------------------------------------------------------

void ZCodec::SetBreak( ULONG nInToRead )
{
	mnInToRead = nInToRead;
}

// ------------------------------------------------------------------------

void ZCodec::ImplInitBuf ( BOOL nIOFlag )
{
	if ( mbInit == 0 )
	{
		if ( nIOFlag )
		{	
			mbInit = 1;
			if ( mbStatus && ( mnCompressMethod & ZCODEC_GZ_LIB ) )
			{
				U8 n1, n2, j, nMethod, nFlags;
				for ( int i = 0; i < 2; i++ )	// gz - magic number
				{
					//*mpIStm >> j;
                                        mpIStm->read(&j, 1);
					if ( j != gz_magic[ i ] )
						mbStatus = false;
				}
				//*mpIStm >> nMethod;
                                mpIStm->read(&nMethod, 1);
				//*mpIStm >> nFlags;
                                mpIStm->read(&nFlags, 1);
				if ( nMethod != Z_DEFLATED )
					mbStatus = false;
				if ( ( nFlags & GZ_RESERVED ) != 0 )
					mbStatus = false;
				/* Discard time, xflags and OS code: */
				//mpIStm->SeekRel( 6 );
                                mpIStm->seek( 6, G_SEEK_CUR );
			    /* skip the extra field */
				if ( nFlags & GZ_EXTRA_FIELD )
				{
					//*mpIStm >> n1 >> n2;
                                        mpIStm->read(&n1, 1);
                                        mpIStm->read(&n2, 1);
					//mpIStm->SeekRel( n1 + ( n2 << 8 ) );
                                        mpIStm->seek( n1 + ( n2 << 8 ), G_SEEK_CUR );
				}
				/* skip the original file name */
			    if ( nFlags & GZ_ORIG_NAME)
				{
					do
					{
						//*mpIStm >> j;
                                                mpIStm->read(&j, 1);
					}									
					//while ( j && !mpIStm->IsEof() );
                                        while ( j && mpIStm->isValid() ); //TODO check this!
				}
				/* skip the .gz file comment */
				if ( nFlags & GZ_COMMENT )
				{
					do
					{
						//*mpIStm >> j;
                                                mpIStm->read(&j, 1);
					}
					//while ( j && !mpIStm->IsEof() );
                                        while ( j && mpIStm->isValid() ); //TODO check this!
				}
				/* skip the header crc */
				if ( nFlags & GZ_HEAD_CRC ) 
					//mpIStm->SeekRel( 2 );
                                        mpIStm->seek( 2, G_SEEK_CUR);
				if ( mbStatus )
				    mbStatus = ( inflateInit2( PZSTREAM, -MAX_WBITS) != Z_OK ) ? false : true;
			}
			else
			{
				mbStatus = ( inflateInit( PZSTREAM ) >= 0 );
			}
			mpInBuf = new BYTE[ mnInBufSize ];
		}
		else
		{	
			mbInit = 3;

			mbStatus = ( deflateInit2_( PZSTREAM, mnCompressMethod & 0xff, Z_DEFLATED, 
				MAX_WBITS, mnMemUsage, ( mnCompressMethod >> 8 ) & 0xff, 
					ZLIB_VERSION, sizeof( z_stream ) ) >= 0 );

			PZSTREAM->next_out = mpOutBuf = new BYTE[ PZSTREAM->avail_out = mnOutBufSize ];
		}
	}
}

// ------------------------------------------------------------------------
/*
long ZCodec::Write( OLEStreamWriter& rOStm, const BYTE* pData, ULONG nSize )
{		
	if ( mbInit == 0 )
	{
		mpOStm = &rOStm;
		ImplInitBuf( false );
	}
		
	PZSTREAM->avail_in = nSize;
	PZSTREAM->next_in = (unsigned char*)pData;
	
    while ( PZSTREAM->avail_in || ( PZSTREAM->avail_out == 0 ) )
    {
        if ( PZSTREAM->avail_out == 0 )
			ImplWriteBack();

		if ( deflate( PZSTREAM, Z_NO_FLUSH ) < 0 )
		{	
			mbStatus = false;
			break;
		}
    }
	return ( mbStatus ) ? (long)nSize : -1;
}

// ------------------------------------------------------------------------

long ZCodec::Read( OLEStreamReader& rIStm, BYTE* pData, ULONG nSize )
{
	int err;
	ULONG	nInToRead;

	if ( mbFinish )	
		return 0;			// PZSTREAM->total_out;

	mpIStm = &rIStm;
	if ( mbInit == 0 )
	{
		ImplInitBuf( true );
	}
	PZSTREAM->avail_out = nSize;
	PZSTREAM->next_out = pData;
	do
	{
		if ( PZSTREAM->avail_in == 0 && mnInToRead )
		{
			nInToRead = (mnInBufSize > mnInToRead) ? mnInToRead : mnInBufSize;
			PZSTREAM->avail_in = mpIStm->read (
				PZSTREAM->next_in = mpInBuf, nInToRead);
			mnInToRead -= nInToRead;

                        //TODO fix CRC handling
			//if ( mnCompressMethod & ZCODEC_UPDATE_CRC )
			//	mnCRC = UpdateCRC( mnCRC, mpInBuf, nInToRead );

		}
		err = inflate( PZSTREAM, Z_NO_FLUSH );
		if ( err < 0 )
		{
			// Accept Z_BUF_ERROR as EAGAIN or EWOULDBLOCK.
			mbStatus = (err == Z_BUF_ERROR);
			break;
		}
	}
	while ( (err != Z_STREAM_END) &&
			(PZSTREAM->avail_out != 0) &&
			(PZSTREAM->avail_in || mnInToRead) );
	if ( err == Z_STREAM_END ) 
		mbFinish = true;

	return (mbStatus ? (long)(nSize - PZSTREAM->avail_out) : -1);
}

// ------------------------------------------------------------------------

long ZCodec::ReadAsynchron( OLEStreamReader& rIStm, BYTE* pData, ULONG nSize )
{
	int err = 0;
	ULONG	nInToRead;

	if ( mbFinish )	
		return 0;			// PZSTREAM->total_out;

	if ( mbInit == 0 )
	{
		mpIStm = &rIStm;
		ImplInitBuf( true );
	}
	PZSTREAM->avail_out = nSize;
	PZSTREAM->next_out = pData;
	do
	{
		if ( PZSTREAM->avail_in == 0 && mnInToRead )
		{
			nInToRead = (mnInBufSize > mnInToRead) ? mnInToRead : mnInBufSize;

			ULONG nStreamPos = rIStm.tell();
			rIStm.seek( G_SEEK_END );
			ULONG nMaxPos = rIStm.tell();
			rIStm.seek( nStreamPos );
			if ( ( nMaxPos - nStreamPos ) < nInToRead )
			{
                            //TODO figure out the replacement code for this
			    //rIStm.SetError( ERRCODE_IO_PENDING );
			    err= ! Z_STREAM_END; // TODO What is appropriate code for this?
			    break;
			}

			PZSTREAM->avail_in = mpIStm->read (
				PZSTREAM->next_in = mpInBuf, nInToRead);
			mnInToRead -= nInToRead;

                        //TODO fix CRC handling
			//if ( mnCompressMethod & ZCODEC_UPDATE_CRC )
			//	mnCRC = UpdateCRC( mnCRC, mpInBuf, nInToRead );

		}
		err = inflate( PZSTREAM, Z_NO_FLUSH );
		if ( err < 0 )
		{
			// Accept Z_BUF_ERROR as EAGAIN or EWOULDBLOCK.
			mbStatus = (err == Z_BUF_ERROR);
			break;
		}
	}
	while ( (err != Z_STREAM_END) &&
			(PZSTREAM->avail_out != 0) &&
			(PZSTREAM->avail_in || mnInToRead) );
	if ( err == Z_STREAM_END ) 
		mbFinish = true;

	return (mbStatus ? (long)(nSize - PZSTREAM->avail_out) : -1);
}

// ------------------------------------------------------------------------

ULONG ZCodec::GetBreak( void )
{
	return ( mnInToRead + PZSTREAM->avail_in );
}

// ------------------------------------------------------------------------

void ZCodec::SetCRC( ULONG nCRC )
{
	mnCRC = nCRC;
}

// ------------------------------------------------------------------------

ULONG ZCodec::GetCRC()
{
	return mnCRC;
}

// ------------------------------------------------------------------------

/*ULONG ZCodec::UpdateCRC ( ULONG nLatestCRC, ULONG nNumber )
{

#ifdef OSL_LITENDIAN
	nNumber = SWAPLONG( nNumber );
#endif
	return rtl_crc32( nLatestCRC, &nNumber, 4 );
}

// ------------------------------------------------------------------------

ULONG ZCodec::UpdateCRC ( ULONG nLatestCRC, BYTE* pSource, long nDatSize)
{
	return rtl_crc32( nLatestCRC, pSource, nDatSize );
}

// ------------------------------------------------------------------------

void GZCodec::BeginCompression( ULONG nCompressMethod )
{
	ZCodec::BeginCompression( nCompressMethod | ZCODEC_GZ_LIB );
}*/

