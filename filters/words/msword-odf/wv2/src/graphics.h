/* This file is part of the wvWare 2 project
   Copyright (C) 2003 Werner Trobin <trobin@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02111-1307, USA.
*/

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <string>

#include "word_helper.h"
#include "wv2_export.h"

using std::string;

namespace wvWare
{
    class OLEStreamReader;
    namespace Word97
    {
        struct FSPA;
        struct FIB;
        struct FTXBXS;
        struct BKD;
    }

    class Drawings
    {
    public:
        Drawings( OLEStreamReader* table, const Word97::FIB &fib );
        ~Drawings();

        PLCF<Word97::FSPA>* getSpaMom(){return m_plcfspaMom;};
        PLCF<Word97::FSPA>* getSpaHdr(){return m_plcfspaHdr;};
        PLCF<Word97::FTXBXS>* getTxbxTxt(){return m_plcftxbxTxt;};
        PLCF<Word97::FTXBXS>* getHdrTxbxTxt(){return m_plcfHdrtxbxTxt;};
    private:
        Drawings( const Drawings& rhs );
        Drawings& operator=( const Drawings& rhs );

        PLCF<Word97::FSPA>* m_plcfspaMom;
        PLCF<Word97::FSPA>* m_plcfspaHdr;

        PLCF<Word97::FTXBXS>* m_plcftxbxTxt;
        PLCF<Word97::FTXBXS>* m_plcfHdrtxbxTxt;

        PLCF<Word97::BKD>* m_plcftxbxBkd;
        PLCF<Word97::BKD>* m_plcfHdrtxbxBkd;
    };

    class Pictures
    {
    };

    typedef enum
    {
        msoblipUsageDefault,  // All non-texture fill blips get this.
        msoblipUsageTexture,
        msoblipUsageMax = 255 // Since this is stored in a byte
    } MSOBLIPUSAGE;

    typedef enum
    {                          // GEL provided types...
        msoblipERROR = 0,          // An error occured during loading
        msoblipUNKNOWN,            // An unknown blip type
        msoblipEMF,                // Windows Enhanced Metafile
        msoblipWMF,                // Windows Metafile
        msoblipPICT,               // Macintosh PICT
        msoblipJPEG,               // JFIF
        msoblipPNG,                // PNG
        msoblipDIB,                // Windows DIB
        msoblipFirstClient = 32,   // First client defined blip type
        msoblipLastClient  = 255   // Last client defined blip type
    } MSOBLIPTYPE;

    typedef enum
    {
        msobiUNKNOWN = 0,
        msobiWMF  = 0x216,      // Metafile header then compressed WMF
        msobiEMF  = 0x3D4,      // Metafile header then compressed EMF
        msobiPICT = 0x542,      // Metafile header then compressed PICT
        msobiPNG  = 0x6E0,      // One byte tag then PNG data
        msobiJFIF = 0x46A,      // One byte tag then JFIF data
        msobiJPEG = msobiJFIF,
        msobiDIB  = 0x7A8,      // One byte tag then DIB data
        msobiClient=0x800       // Clients should set this bit
    } MSOBI;                     // Blip signature as encoded in the MSOFBH.inst



    //this is a common header that every record
    //in Escher streams share
    class EscherHeader
    {
    public:
        EscherHeader( OLEStreamReader* stream );
        ~EscherHeader();

        bool isAtom();
        int recordSize();
        int recordInstance();
        string getRecordType();
        void dump();

    private:
        U32 recVer:4; //4 bits
        U32 recInstance:12; //12 bits
        U32 recType:16; //16 bits
        U32 recLen; //4 bytes
    }; //EscherHeader

    //msofbtSpContainer
    //msofbtSp
    //msofbtOPT
    //msoftbClientAnchor

    //this is a structure inside the msofbtBSE
    //record
    class FBSE
    {
    public:
        FBSE( OLEStreamReader* stream );
        ~FBSE();

        int recordSize();//size of the record without the Escher header
                    //(does NOT include actual picture data, either, which is in a
                    //new record)

        U8* getRgbUid();
        int getBlipType();
        int getStreamOffset();
        int getNameLength();
        void dump();

    private:
        MSOBLIPTYPE btWin32; //required type on Win32
        MSOBLIPTYPE btMacOS; //required type on Mac
        U8 rgbUid[ 16 ]; //identifier of the blip
        U16 tag; //unused
        U32 size; //blip size in the stream
        U32 cRef; //reference count on the blip
        U32 foDelay; //file offset in the delay stream
        MSOBLIPUSAGE usage; //how this blip is used
        U8 cbName; //length of blip name
        U8 unused2; //unused
        U8 unused3; //unused
    }; //FBSE

    //this is a structure that actually contains the
    //image data (it follows the FBSE in a new record)
    class Blip
    {
    public:
        Blip( OLEStreamReader* stream, string blipType );
        ~Blip();
        
        bool isMetafileBlip(); //is this an EMF, WMF, or PICT?
        bool isCompressed(); //is this blip compressed? (only applied to metafile blips)
        int recordSize(); //size of the record *without* the actual picture data
        int imageSize(); //size of the *uncompressed* image
        int compressedImageSize(); //size of the *compressed* image
        void dump();
    private:
        U8 m_rgbUid[16];
        U8 m_bTag;
        U8 m_rgbUidPrimary[16]; //not always present!
        U32 m_cb;
        U32 m_rcBounds;
        U32 m_ptSize;
        U32 m_cbSave;
        U8 m_fCompression;
        U8 m_fFilter;
        string m_blipType;
        unsigned int m_size; //store size of record (without actual picture data)
                //this is set in the constructor when the data is read in
        bool m_isMetafileBlip; //flag for remembering whether it's metafile or bitmap
    }; //Blip

} // namespace wvWare

#endif // GRAPHICS_H
