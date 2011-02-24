/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: zcodec.hxx,v $
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
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

#ifndef _ZCODEC_HXX
#define _ZCODEC_HXX

//#ifndef INCLUDED_TOOLSDLLAPI_H
//#include "tools/toolsdllapi.h"
//#endif

//#ifndef _SOLAR_H
//#include <tools/solar.h>
//#endif

#include "global.h" //for U8
#include <vector> //for std::vector

// -----------
// - Defines -
// -----------

#define DEFAULT_IN_BUFSIZE			(0x00008000UL)
#define DEFAULT_OUT_BUFSIZE			(0x00008000UL)

#define MAX_MEM_USAGE 8

//
// memory requirement using compress:
//	[ INBUFFER ] + [ OUTBUFFER ] + 128KB + 1 << (MEM_USAGE+9)
//
// memory requirement using decompress:
//	[ INBUFFER ] + [ OUTBUFFER ] + 32KB
//

#define ZCODEC_NO_COMPRESSION		(0x00000000UL)
#define ZCODEC_BEST_SPEED			(0x00000001UL)
#define	ZCODEC_DEFAULT_COMPRESSION	(0x00000006UL)
#define ZCODEC_BEST_COMPRESSION		(0x00000009UL)

#define ZCODEC_DEFAULT_STRATEGY		(0x00000000UL)
#define ZCODEC_ZFILTERED			(0x00000100UL)
#define ZCODEC_ZHUFFMAN_ONLY		(0x00000200UL)

#define ZCODEC_UPDATE_CRC			(0x00010000UL)
#define ZCODEC_GZ_LIB				(0x00020000UL)

#define ZCODEC_PNG_DEFAULT ( ZCODEC_NO_COMPRESSION | ZCODEC_DEFAULT_STRATEGY | ZCODEC_UPDATE_CRC )
#define ZCODEC_DEFAULT	( ZCODEC_DEFAULT_COMPRESSION | ZCODEC_DEFAULT_STRATEGY )

// ----------
// - ZCodec -
// ----------

//class OLEStreamReader;
//class OLEStreamWriter;

using namespace wvWare;

typedef unsigned long ULONG;
typedef bool BOOL;
typedef U8 BYTE;

class ZCodec
{
private:

	ULONG			mbInit;
	BOOL			mbStatus; //status good or bad
	BOOL			mbFinish; //are we finished yet?
	ULONG			mnMemUsage; //total memory we can use?
	OLEStreamReader*	mpIStm; //in-stream
	BYTE*			mpInBuf; //in-buffer
	ULONG			mnInBufSize; //size of the in-buffer
	ULONG			mnInToRead; //how many bytes overall we still want to read
	OLEStreamWriter*	mpOStm; //out-stream
	BYTE*			mpOutBuf; //out-buffer
	ULONG			mnOutBufSize; //size of the out-buffer

	ULONG			mnCRC;
	ULONG			mnCompressMethod;
	void*			mpsC_Stream;

	void			ImplInitBuf( BOOL nIOFlag );
	void			ImplWriteBack( std::vector<U8>* outBuffer );

public:	
				ZCodec( ULONG nInBuf, ULONG nOutBuf, ULONG nMemUsage = MAX_MEM_USAGE );
				ZCodec( void );	
	virtual			~ZCodec();

	virtual void	BeginCompression( ULONG nCompressMethod = ZCODEC_DEFAULT );
	virtual long	EndCompression(std::vector<U8>* outBuffer);
	BOOL            IsFinished () const { return mbFinish; }

	long			Compress( OLEStreamReader& rIStm, OLEStreamWriter& rOStm );
	long			Decompress( OLEStreamReader& rIStm, std::vector<U8>* outBuffer );

	//long			Write( OLEStreamWriter& rOStm, const BYTE* pData, ULONG nSize );
	//long			Read( OLEStreamReader& rIStm, BYTE* pData, ULONG nSize );
	//long			ReadAsynchron( OLEStreamReader& rIStm, BYTE* pData, ULONG nSize );

	void			SetBreak( ULONG );
	//ULONG			GetBreak( void );
	//void			SetCRC( ULONG nCurrentCRC );
	//ULONG			UpdateCRC( ULONG nLatestCRC, ULONG nSource );	
	//ULONG			UpdateCRC( ULONG nLatestCRC, BYTE* pSource, long nDatSize );
	//ULONG			GetCRC();
};

class GZCodec : public ZCodec
{

public:
					GZCodec(){};
					~GZCodec(){};
	virtual void	BeginCompression( ULONG nCompressMethod = ZCODEC_DEFAULT );
};

#endif // _ZCODEC_HXX
