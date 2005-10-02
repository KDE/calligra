/*
    Copyright (C) 2000, S.R.Haque <shaheedhaque@hotmail.com>.
    This file is part of the KDE project

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.

DESCRIPTION

    This file is a description of structures used in the storage of Microsoft
    Office art. The specification for this is the Microsoft Office 97 Drawing
    File Format published in MSDN.
*/

#ifndef MSDRAWING_H
#define MSDRAWING_H

#include <mswordgenerated.h>

class MsDrawing
{
public:
    typedef MsWordGenerated::U8 U8;
    typedef MsWordGenerated::U16 U16;
    typedef MsWordGenerated::U32 U32;

    // GEL provided types...

    typedef enum
    {
        msoblipERROR,               // An error occurred during loading.
        msoblipUNKNOWN,             // An unknown blip type.
        msoblipEMF,                 // Windows Enhanced Metafile.
        msoblipWMF,                 // Windows Metafile.
        msoblipPICT,                // Macintosh PICT.
        msoblipJPEG,                // JFIF.
        msoblipPNG,                 // PNG.
        msoblipDIB,                 // Windows DIB.
        msoblipFirstClient = 32,    // First client defined blip type.
        msoblipLastClient  = 255    // Last client defined blip type.
    } MSOBLIPTYPE;

    // Extract a picture from Drawing File by shapeId. If the picture is found,
    // the pictureLength will be a non-zero value.

    static void getDrawing(
        const U8 *in,
        U32 count,
        U32 shapeId,
        const U8 *pictureStream,
        MSOBLIPTYPE *pictureType,
        U32 *pictureLength,
        const U8 **pictureData);

private:

    // Error handling and reporting support.

    static const int s_area;
    static unsigned walkFile(
        const U8 *in,
        U32 count,
        U32 shapeId,
        const U8 *pictureStream,
        MSOBLIPTYPE *pictureType,
        U32 *pictureLength,
        const U8 **pictureData);

    typedef enum
    {
        msofbtDggContainer = 0xF000,
        msofbtBstoreContainer,
        msofbtDgContainer,
        msofbtSpgrContainer,
        msofbtSpContainer,
        msobftSolverContainer,
        msofbtDgg,
        msofbtBSE,
        msofbtDg,
        msofbtSpgr,
        msofbtSp,
        msofbtOPT,
        msofbtTextbox,
        msofbtClientTextbox,
        msofbtAnchor,
        msofbtChildAnchor,
        msofbtClientAnchor,
        msofbtClientData,
        msofbtConnectorRule,
        msofbtAlignRule,
        msofbtArcRule,
        msofbtClientRule,
        msofbtCLSID,
        msoftCalloutRule,
        msofbtBlipFirst, // 0xF018
        msofbtBlipLast = 0xF117,
        msofbtRegroupItems,
        msofbtSelection,
        msofbtColorMRU,
        msofbtDeletedPspl = 0xF11D,
        msofbtSplitMenuColors,
        msofbtOleObject
    } MSOFBT;

    // Blip signature as encoded in the MSOFBH.inst

    typedef enum
    {
        msobiUNKNOWN = 0,
        msobiWMF = 0x216,       // Metafile header then compressed WMF
        msobiEMF = 0x3D4,       // Metafile header then compressed EMF
        msobiPICT = 0x542,      // Metafile header then compressed PICT
        msobiPNG = 0x6E0,       // One byte tag then PNG data
        msobiJFIF = 0x46A,      // One byte tag then JFIF data
        msobiJPEG = msobiJFIF,
        msobiDIB = 0x7A8,       // One byte tag then DIB data
        msobiClient = 0x800     // Clients should set this bit
    } MSOBI;

    // Common Header (MSOBFH)

    typedef struct MSOBFH
    {
        U32 ver:4;
        U32 inst: 12;
        U32 fbt: 16;
        U32 cbLength;
    } MSOBFH;
    static unsigned read(const U8 *in, MSOBFH *out);

    // FBSE - File Blip Store Entry

    typedef struct
    {
        U8 btWin32;     // Required type on Win32.
        U8 btMacOS;     // Required type on Mac.
        U8 rgbUid[16];  // Identifier of blip.
        U16 tag;        // currently unused.
        U32 size;       // Blip size in stream.
        U32 cRef;       // Reference count on the blip.
        U32 foDelay;    // File offset in the delay stream.
        U8 usage;       // How this blip is used (MSOBLIPUSAGE).
        U8 cbName;      // length of the blip name.
        U8 unused2;     // for the future.
        U8 unused3;     // for the future.
    } FBSE;
    static unsigned read(const U8 *in, FBSE *out);

    // FDG - File DG

    typedef struct
    {
        U32 csp;        // The number pof shapes in this drawing.
        U32 spidCur;    // The last shape ID given to an SP in this DG.
    } FDG;
    static unsigned read(const U8 *in, FDG *out);

    // FDGG - File DGG

    typedef struct
    {
        U32 spidMax;    // The current maximum shape ID.
        U32 cidcl;      // The number of ID clusters (FIDCLs).
        U32 cspSaved;   // The total number of shapes saved.
                        // (including deleted shapes, if undo
                        // information was saved).
        U32 cdgSaved;   // The total number of drawings saved.
    } FDGG;
    static unsigned read(const U8 *in, FDGG *out);

    // File ID Cluster - used to save IDCLs

    typedef struct
    {
        U32 dgid;       // DG owning the SPIDs in this cluster
        U32 cspidCur;   // number of SPIDs used so far
    } FIDCL;
};

#endif
