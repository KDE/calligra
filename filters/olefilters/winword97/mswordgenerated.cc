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
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

DESCRIPTION

    This file is a description of most structures used in the on-disk format
    of Microsoft Word 97 documents. The only structures missing are those
    which are tricky to autogenerate.
*/

#include <mswordgenerated.h>
#include <string.h>

unsigned MsWordGenerated::read(const U8 *in, __UNAL U8 *out, unsigned count)
{
    memcpy((void*) out, in, count);
    return count;
}

unsigned MsWordGenerated::read(const U8 *in, __UNAL S8 *out, unsigned count)
{
    return read(in, (U8 *)out, count);
}

unsigned MsWordGenerated::read(const U8 *in, __UNAL U16 *out, unsigned count)
{
    U16 tmp;

    for (unsigned i = 0; i < count; i++)
    {
        tmp = (U16)((*(in + 1) << 8) + (*in));
        memcpy((void*) out, &tmp, sizeof(tmp));
        out++;
        in += sizeof(tmp);
    }
    return count + count;
}

unsigned MsWordGenerated::read(const U8 *in, __UNAL S16 *out, unsigned count)
{
    return read(in, (U16 *)out, count);
}

unsigned MsWordGenerated::read(const U8 *in, __UNAL U32 *out, unsigned count)
{
    U32 tmp;

    for (unsigned i = 0; i < count; i++)
    {
        tmp = (U32)((*(in + 3) << 24) + (*(in + 2) << 16) + (*(in + 1) << 8) + (*in));
        memcpy((void*) out, &tmp, sizeof(tmp));
        out++;
        in += sizeof(tmp);
    }
    return count + count + count + count;
}

unsigned MsWordGenerated::read(const U8 *in, __UNAL S32 *out, unsigned count)
{
    return read(in, (U32 *)out, count);
}

unsigned MsWordGenerated::read(const U8 *in, __UNAL DTTM *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, &shifterU16);
        out->mint = shifterU16;
        shifterU16 >>= 6;
        out->hr = shifterU16;
        shifterU16 >>= 5;
        out->dom = shifterU16;
        shifterU16 >>= 5;
        bytes += read(in + bytes, &shifterU16);
        out->mon = shifterU16;
        shifterU16 >>= 4;
        out->yr = shifterU16;
        shifterU16 >>= 9;
        out->wdy = shifterU16;
        shifterU16 >>= 3;
        out++;
    }
    return bytes;
} // DTTM

unsigned MsWordGenerated::read(const U8 *in, __UNAL SHD *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, &shifterU16);
        out->icoFore = shifterU16;
        shifterU16 >>= 5;
        out->icoBack = shifterU16;
        shifterU16 >>= 5;
        out->ipat = shifterU16;
        shifterU16 >>= 6;
        out++;
    }
    return bytes;
} // SHD

unsigned MsWordGenerated::read(const U8 *in, __UNAL DOPTYPOGRAPHY *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, &shifterU16);
        out->fKerningPunct = shifterU16;
        shifterU16 >>= 1;
        out->iJustification = shifterU16;
        shifterU16 >>= 2;
        out->iLevelOfKinsoku = shifterU16;
        shifterU16 >>= 2;
        out->f2on1 = shifterU16;
        shifterU16 >>= 1;
        out->unused0_6 = shifterU16;
        shifterU16 >>= 10;
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 154);
        out++;
    }
    return bytes;
} // DOPTYPOGRAPHY

unsigned MsWordGenerated::read(const U8 *in, __UNAL PRM *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, &shifterU16);
        out->fComplex = shifterU16;
        shifterU16 >>= 1;
        out->isprm = shifterU16;
        shifterU16 >>= 7;
        out->val = shifterU16;
        shifterU16 >>= 8;
        out++;
    }
    return bytes;
} // PRM

unsigned MsWordGenerated::read(const U8 *in, __UNAL ATRD *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 11);
        bytes += read(in + bytes, &shifterU16);
        out->ak = shifterU16;
        shifterU16 >>= 2;
        out->unused22_2 = shifterU16;
        shifterU16 >>= 14;
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        out++;
    }
    return bytes;
} // ATRD

unsigned MsWordGenerated::read(const U8 *in, __UNAL ANLD *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 3);
        bytes += read(in + bytes, &shifterU8);
        out->jc = shifterU8;
        shifterU8 >>= 2;
        out->fPrev = shifterU8;
        shifterU8 >>= 1;
        out->fHang = shifterU8;
        shifterU8 >>= 1;
        out->fSetBold = shifterU8;
        shifterU8 >>= 1;
        out->fSetItalic = shifterU8;
        shifterU8 >>= 1;
        out->fSetSmallCaps = shifterU8;
        shifterU8 >>= 1;
        out->fSetCaps = shifterU8;
        shifterU8 >>= 1;
        bytes += read(in + bytes, &shifterU8);
        out->fSetStrike = shifterU8;
        shifterU8 >>= 1;
        out->fSetKul = shifterU8;
        shifterU8 >>= 1;
        out->fPrevSpace = shifterU8;
        shifterU8 >>= 1;
        out->fBold = shifterU8;
        shifterU8 >>= 1;
        out->fItalic = shifterU8;
        shifterU8 >>= 1;
        out->fSmallCaps = shifterU8;
        shifterU8 >>= 1;
        out->fCaps = shifterU8;
        shifterU8 >>= 1;
        out->fStrike = shifterU8;
        shifterU8 >>= 1;
        bytes += read(in + bytes, &shifterU8);
        out->kul = shifterU8;
        shifterU8 >>= 3;
        out->ico = shifterU8;
        shifterU8 >>= 5;
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 5);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 4);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 32);
        out++;
    }
    return bytes;
} // ANLD

unsigned MsWordGenerated::read(const U8 *in, __UNAL ANLV *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 3);
        bytes += read(in + bytes, &shifterU8);
        out->jc = shifterU8;
        shifterU8 >>= 2;
        out->fPrev = shifterU8;
        shifterU8 >>= 1;
        out->fHang = shifterU8;
        shifterU8 >>= 1;
        out->fSetBold = shifterU8;
        shifterU8 >>= 1;
        out->fSetItalic = shifterU8;
        shifterU8 >>= 1;
        out->fSetSmallCaps = shifterU8;
        shifterU8 >>= 1;
        out->fSetCaps = shifterU8;
        shifterU8 >>= 1;
        bytes += read(in + bytes, &shifterU8);
        out->fSetStrike = shifterU8;
        shifterU8 >>= 1;
        out->fSetKul = shifterU8;
        shifterU8 >>= 1;
        out->fPrevSpace = shifterU8;
        shifterU8 >>= 1;
        out->fBold = shifterU8;
        shifterU8 >>= 1;
        out->FItalic = shifterU8;
        shifterU8 >>= 1;
        out->fSmallCaps = shifterU8;
        shifterU8 >>= 1;
        out->fCaps = shifterU8;
        shifterU8 >>= 1;
        out->fStrike = shifterU8;
        shifterU8 >>= 1;
        bytes += read(in + bytes, &shifterU8);
        out->kul = shifterU8;
        shifterU8 >>= 3;
        out->ico = shifterU8;
        shifterU8 >>= 5;
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 5);
        out++;
    }
    return bytes;
} // ANLV

unsigned MsWordGenerated::read(const U8 *in, __UNAL ASUMY *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        out++;
    }
    return bytes;
} // ASUMY

unsigned MsWordGenerated::read(const U8 *in, __UNAL ASUMYI *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, &shifterU16);
        out->fValid = shifterU16;
        shifterU16 >>= 1;
        out->fView = shifterU16;
        shifterU16 >>= 1;
        out->iViewBy = shifterU16;
        shifterU16 >>= 2;
        out->fUpdateProps = shifterU16;
        shifterU16 >>= 1;
        out->unused0_5 = shifterU16;
        shifterU16 >>= 11;
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 2);
        out++;
    }
    return bytes;
} // ASUMYI

unsigned MsWordGenerated::read(const U8 *in, __UNAL BTE *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        out++;
    }
    return bytes;
} // BTE

unsigned MsWordGenerated::read(const U8 *in, __UNAL BKD *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 3);
        bytes += read(in + bytes, &shifterU16);
        out->icol = shifterU16;
        shifterU16 >>= 8;
        out->fTableBreak = shifterU16;
        shifterU16 >>= 1;
        out->fColumnBreak = shifterU16;
        shifterU16 >>= 1;
        out->fMarked = shifterU16;
        shifterU16 >>= 1;
        out->fUnk = shifterU16;
        shifterU16 >>= 1;
        out->fTextOverflow = shifterU16;
        shifterU16 >>= 1;
        out++;
    }
    return bytes;
} // BKD

unsigned MsWordGenerated::read(const U8 *in, __UNAL BKF *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, &shifterU16);
        out->itcFirst = shifterU16;
        shifterU16 >>= 7;
        out->fPub = shifterU16;
        shifterU16 >>= 1;
        out->itcLim = shifterU16;
        shifterU16 >>= 7;
        out->fCol = shifterU16;
        shifterU16 >>= 1;
        out++;
    }
    return bytes;
} // BKF

unsigned MsWordGenerated::read(const U8 *in, __UNAL BKL *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        out++;
    }
    return bytes;
} // BKL

unsigned MsWordGenerated::read(const U8 *in, __UNAL BRC *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, &shifterU16);
        out->dptLineWidth = shifterU16;
        shifterU16 >>= 8;
        out->brcType = shifterU16;
        shifterU16 >>= 8;
        bytes += read(in + bytes, &shifterU16);
        out->ico = shifterU16;
        shifterU16 >>= 8;
        out->dptSpace = shifterU16;
        shifterU16 >>= 5;
        out->fShadow = shifterU16;
        shifterU16 >>= 1;
        out->fFrame = shifterU16;
        shifterU16 >>= 1;
        out->unused2_15 = shifterU16;
        shifterU16 >>= 1;
        out++;
    }
    return bytes;
} // BRC

unsigned MsWordGenerated::read(const U8 *in, __UNAL BRC10 *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, &shifterU16);
        out->dxpLine2Width = shifterU16;
        shifterU16 >>= 3;
        out->dxpSpaceBetween = shifterU16;
        shifterU16 >>= 3;
        out->dxpLine1Width = shifterU16;
        shifterU16 >>= 3;
        out->dxpSpace = shifterU16;
        shifterU16 >>= 5;
        out->fShadow = shifterU16;
        shifterU16 >>= 1;
        out->fSpare = shifterU16;
        shifterU16 >>= 1;
        out++;
    }
    return bytes;
} // BRC10

unsigned MsWordGenerated::read(const U8 *in, __UNAL CHP *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, &shifterU8);
        out->fBold = shifterU8;
        shifterU8 >>= 1;
        out->fItalic = shifterU8;
        shifterU8 >>= 1;
        out->fRMarkDel = shifterU8;
        shifterU8 >>= 1;
        out->fOutline = shifterU8;
        shifterU8 >>= 1;
        out->fFldVanish = shifterU8;
        shifterU8 >>= 1;
        out->fSmallCaps = shifterU8;
        shifterU8 >>= 1;
        out->fCaps = shifterU8;
        shifterU8 >>= 1;
        out->fVanish = shifterU8;
        shifterU8 >>= 1;
        bytes += read(in + bytes, &shifterU8);
        out->fRMark = shifterU8;
        shifterU8 >>= 1;
        out->fSpec = shifterU8;
        shifterU8 >>= 1;
        out->fStrike = shifterU8;
        shifterU8 >>= 1;
        out->fObj = shifterU8;
        shifterU8 >>= 1;
        out->fShadow = shifterU8;
        shifterU8 >>= 1;
        out->fLowerCase = shifterU8;
        shifterU8 >>= 1;
        out->fData = shifterU8;
        shifterU8 >>= 1;
        out->fOle2 = shifterU8;
        shifterU8 >>= 1;
        bytes += read(in + bytes, &shifterU16);
        out->fEmboss = shifterU16;
        shifterU16 >>= 1;
        out->fImprint = shifterU16;
        shifterU16 >>= 1;
        out->fDStrike = shifterU16;
        shifterU16 >>= 1;
        out->fUsePgsuSettings = shifterU16;
        shifterU16 >>= 1;
        out->unused2_4 = shifterU16;
        shifterU16 >>= 12;
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 5);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, &shifterU8);
        out->iss = shifterU8;
        shifterU8 >>= 3;
        out->kul = shifterU8;
        shifterU8 >>= 4;
        out->fSpecSymbol = shifterU8;
        shifterU8 >>= 1;
        bytes += read(in + bytes, &shifterU8);
        out->ico = shifterU8;
        shifterU8 >>= 5;
        out->unused23_5 = shifterU8;
        shifterU8 >>= 1;
        out->fSysVanish = shifterU8;
        shifterU8 >>= 1;
        out->hpsPos = shifterU8;
        shifterU8 >>= 1;
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 4);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (DTTM *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 6);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
        bytes += read(in + bytes, &shifterU16);
        out->icoHighlight = shifterU16;
        shifterU16 >>= 5;
        out->fHighlight = shifterU16;
        shifterU16 >>= 1;
        out->kcd = shifterU16;
        shifterU16 >>= 3;
        out->fNavHighlight = shifterU16;
        shifterU16 >>= 1;
        out->fChsDiff = shifterU16;
        shifterU16 >>= 1;
        out->fMacChs = shifterU16;
        shifterU16 >>= 1;
        out->fFtcAsciSym = shifterU16;
        shifterU16 >>= 1;
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (DTTM *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 3);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 16);
        bytes += read(in + bytes, (SHD *)(ptr + bytes), 1);
        bytes += read(in + bytes, (BRC *)(ptr + bytes), 1);
        out++;
    }
    return bytes;
} // CHP

//unsigned MsWordGenerated::read(const U8 *in, __UNAL CHPX *out, unsigned count)
//{
//    U32 shifterU32;
//    U16 shifterU16;
//    U8 shifterU8;
//    U8 *ptr;
//    unsigned bytes = 0;
//
//    ptr = (U8 *)out;
//    shifterU32 = shifterU16 = shifterU8 = 0;
//    for (unsigned i = 0; i < count; i++)
//    {
//        bytes += read(in + bytes, (U8 *)(ptr + bytes), 1);
//        bytes += read(in + bytes, (U8 *)(ptr + bytes), cb);
//        out++;
//    }
//    return bytes;
//} // CHPX

unsigned MsWordGenerated::read(const U8 *in, __UNAL DCS *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, &shifterU8);
        out->fdct = shifterU8;
        shifterU8 >>= 3;
        out->unused0_3 = shifterU8;
        shifterU8 >>= 5;
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        out++;
    }
    return bytes;
} // DCS

unsigned MsWordGenerated::read(const U8 *in, __UNAL DOGRID *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 4);
        bytes += read(in + bytes, &shifterU16);
        out->dyGridDisplay = shifterU16;
        shifterU16 >>= 7;
        out->fTurnItOff = shifterU16;
        shifterU16 >>= 1;
        out->dxGridDisplay = shifterU16;
        shifterU16 >>= 7;
        out->fFollowMargins = shifterU16;
        shifterU16 >>= 1;
        out++;
    }
    return bytes;
} // DOGRID

unsigned MsWordGenerated::read(const U8 *in, __UNAL DOP *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, &shifterU16);
        out->fFacingPages = shifterU16;
        shifterU16 >>= 1;
        out->fWidowControl = shifterU16;
        shifterU16 >>= 1;
        out->fPMHMainDoc = shifterU16;
        shifterU16 >>= 1;
        out->grfSuppression = shifterU16;
        shifterU16 >>= 2;
        out->fpc = shifterU16;
        shifterU16 >>= 2;
        out->unused0_7 = shifterU16;
        shifterU16 >>= 1;
        out->grpfIhdt = shifterU16;
        shifterU16 >>= 8;
        bytes += read(in + bytes, &shifterU16);
        out->rncFtn = shifterU16;
        shifterU16 >>= 2;
        out->nFtn = shifterU16;
        shifterU16 >>= 14;
        bytes += read(in + bytes, &shifterU8);
        out->fOutlineDirtySave = shifterU8;
        shifterU8 >>= 1;
        out->unused4_1 = shifterU8;
        shifterU8 >>= 7;
        bytes += read(in + bytes, &shifterU8);
        out->fOnlyMacPics = shifterU8;
        shifterU8 >>= 1;
        out->fOnlyWinPics = shifterU8;
        shifterU8 >>= 1;
        out->fLabelDoc = shifterU8;
        shifterU8 >>= 1;
        out->fHyphCapitals = shifterU8;
        shifterU8 >>= 1;
        out->fAutoHyphen = shifterU8;
        shifterU8 >>= 1;
        out->fFormNoFields = shifterU8;
        shifterU8 >>= 1;
        out->fLinkStyles = shifterU8;
        shifterU8 >>= 1;
        out->fRevMarking = shifterU8;
        shifterU8 >>= 1;
        bytes += read(in + bytes, &shifterU8);
        out->fBackup = shifterU8;
        shifterU8 >>= 1;
        out->fExactCWords = shifterU8;
        shifterU8 >>= 1;
        out->fPagHidden = shifterU8;
        shifterU8 >>= 1;
        out->fPagResults = shifterU8;
        shifterU8 >>= 1;
        out->fLockAtn = shifterU8;
        shifterU8 >>= 1;
        out->fMirrorMargins = shifterU8;
        shifterU8 >>= 1;
        out->fDfltTrueType = shifterU8;
        shifterU8 >>= 1;
        bytes += read(in + bytes, &shifterU8);
        out->fPagSuppressTopSpacing = shifterU8;
        shifterU8 >>= 1;
        out->fProtEnabled = shifterU8;
        shifterU8 >>= 1;
        out->fDispFormFldSel = shifterU8;
        shifterU8 >>= 1;
        out->fRMView = shifterU8;
        shifterU8 >>= 1;
        out->fRMPrint = shifterU8;
        shifterU8 >>= 1;
        out->fLockRev = shifterU8;
        shifterU8 >>= 1;
        out->fEmbedFonts = shifterU8;
        shifterU8 >>= 1;
        bytes += read(in + bytes, &shifterU16);
        out->copts_fNoTabForInd = shifterU16;
        shifterU16 >>= 1;
        out->copts_fNoSpaceRaiseLower = shifterU16;
        shifterU16 >>= 1;
        out->copts_fSuppressSpbfAfterPageBreak = shifterU16;
        shifterU16 >>= 1;
        out->copts_fWrapTrailSpaces = shifterU16;
        shifterU16 >>= 1;
        out->copts_fMapPrintTextColor = shifterU16;
        shifterU16 >>= 1;
        out->copts_fNoColumnBalance = shifterU16;
        shifterU16 >>= 1;
        out->copts_fConvMailMergeEsc = shifterU16;
        shifterU16 >>= 1;
        out->copts_fSupressTopSpacing = shifterU16;
        shifterU16 >>= 1;
        out->copts_fOrigWordTableRules = shifterU16;
        shifterU16 >>= 1;
        out->copts_fTransparentMetafiles = shifterU16;
        shifterU16 >>= 1;
        out->copts_fShowBreaksInFrames = shifterU16;
        shifterU16 >>= 1;
        out->copts_fSwapBordersFacingPgs = shifterU16;
        shifterU16 >>= 1;
        out->unused8_12 = shifterU16;
        shifterU16 >>= 4;
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 5);
        bytes += read(in + bytes, (DTTM *)(ptr + bytes), 3);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 3);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, &shifterU16);
        out->rncEdn = shifterU16;
        shifterU16 >>= 2;
        out->nEdn = shifterU16;
        shifterU16 >>= 14;
        bytes += read(in + bytes, &shifterU16);
        out->epc = shifterU16;
        shifterU16 >>= 2;
        out->nfcFtnRef = shifterU16;
        shifterU16 >>= 4;
        out->nfcEdnRef = shifterU16;
        shifterU16 >>= 4;
        out->fPrintFormData = shifterU16;
        shifterU16 >>= 1;
        out->fSaveFormData = shifterU16;
        shifterU16 >>= 1;
        out->fShadeFormData = shifterU16;
        shifterU16 >>= 1;
        out->unused54_13 = shifterU16;
        shifterU16 >>= 2;
        out->fWCFtnEdn = shifterU16;
        shifterU16 >>= 1;
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 3);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 3);
        bytes += read(in + bytes, &shifterU16);
        out->wvkSaved = shifterU16;
        shifterU16 >>= 3;
        out->wScaleSaved = shifterU16;
        shifterU16 >>= 9;
        out->zkSaved = shifterU16;
        shifterU16 >>= 2;
        out->fRotateFontW6 = shifterU16;
        shifterU16 >>= 1;
        out->iGutterPos = shifterU16;
        shifterU16 >>= 1;
        out++;
    }
    return bytes;
} // DOP

unsigned MsWordGenerated::read(const U8 *in, __UNAL DOP102 *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, &shifterU32);
        out->fNoTabForInd = shifterU32;
        shifterU32 >>= 1;
        out->fNoSpaceRaiseLower = shifterU32;
        shifterU32 >>= 1;
        out->fSupressSpbfAfterPageBreak = shifterU32;
        shifterU32 >>= 1;
        out->fWrapTrailSpaces = shifterU32;
        shifterU32 >>= 1;
        out->fMapPrintTextColor = shifterU32;
        shifterU32 >>= 1;
        out->fNoColumnBalance = shifterU32;
        shifterU32 >>= 1;
        out->fConvMailMergeEsc = shifterU32;
        shifterU32 >>= 1;
        out->fSupressTopSpacing = shifterU32;
        shifterU32 >>= 1;
        out->fOrigWordTableRules = shifterU32;
        shifterU32 >>= 1;
        out->fTransparentMetafiles = shifterU32;
        shifterU32 >>= 1;
        out->fShowBreaksInFrames = shifterU32;
        shifterU32 >>= 1;
        out->fSwapBordersFacingPgs = shifterU32;
        shifterU32 >>= 1;
        out->unused84_12 = shifterU32;
        shifterU32 >>= 4;
        out->fSuppressTopSpacingMac5 = shifterU32;
        shifterU32 >>= 1;
        out->fTruncDxaExpand = shifterU32;
        shifterU32 >>= 1;
        out->fPrintBodyBeforeHdr = shifterU32;
        shifterU32 >>= 1;
        out->fNoLeading = shifterU32;
        shifterU32 >>= 1;
        out->unused84_20 = shifterU32;
        shifterU32 >>= 1;
        out->fMWSmallCaps = shifterU32;
        shifterU32 >>= 1;
        out->unused84_22 = shifterU32;
        shifterU32 >>= 10;
        out++;
    }
    return bytes;
} // DOP102

unsigned MsWordGenerated::read(const U8 *in, __UNAL DOP105 *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (DOPTYPOGRAPHY *)(ptr + bytes), 1);
        bytes += read(in + bytes, (DOGRID *)(ptr + bytes), 1);
        bytes += read(in + bytes, &shifterU16);
        out->reserved = shifterU16;
        shifterU16 >>= 1;
        out->lvl = shifterU16;
        shifterU16 >>= 4;
        out->fGramAllDone = shifterU16;
        shifterU16 >>= 1;
        out->fGramAllClean = shifterU16;
        shifterU16 >>= 1;
        out->fSubsetFonts = shifterU16;
        shifterU16 >>= 1;
        out->fHideLastVersion = shifterU16;
        shifterU16 >>= 1;
        out->fHtmlDoc = shifterU16;
        shifterU16 >>= 1;
        out->unused410_11 = shifterU16;
        shifterU16 >>= 1;
        out->fSnapBorder = shifterU16;
        shifterU16 >>= 1;
        out->fIncludeHeader = shifterU16;
        shifterU16 >>= 1;
        out->fIncludeFooter = shifterU16;
        shifterU16 >>= 1;
        out->fForcePageSizePag = shifterU16;
        shifterU16 >>= 1;
        out->fMinFontSizePag = shifterU16;
        shifterU16 >>= 1;
        bytes += read(in + bytes, &shifterU16);
        out->fHaveVersions = shifterU16;
        shifterU16 >>= 1;
        out->fAutoVersion = shifterU16;
        shifterU16 >>= 1;
        out->unused412_2 = shifterU16;
        shifterU16 >>= 14;
        bytes += read(in + bytes, (ASUMYI *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 3);
        bytes += read(in + bytes, &shifterU32);
        out->fVirusPrompted = shifterU32;
        shifterU32 >>= 1;
        out->fVirusLoadSafe = shifterU32;
        shifterU32 >>= 1;
        out->KeyVirusSession30 = shifterU32;
        shifterU32 >>= 30;
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 30);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 5);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 4);
        out++;
    }
    return bytes;
} // DOP105

unsigned MsWordGenerated::read(const U8 *in, __UNAL FLD *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, &shifterU8);
        out->ch = shifterU8;
        shifterU8 >>= 5;
        out->unused0_5 = shifterU8;
        shifterU8 >>= 3;
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 1);
        out++;
    }
    return bytes;
} // FLD

unsigned MsWordGenerated::read(const U8 *in, __UNAL FSPA *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 5);
        bytes += read(in + bytes, &shifterU16);
        out->fHdr = shifterU16;
        shifterU16 >>= 1;
        out->bx = shifterU16;
        shifterU16 >>= 2;
        out->by = shifterU16;
        shifterU16 >>= 2;
        out->wr = shifterU16;
        shifterU16 >>= 4;
        out->wrk = shifterU16;
        shifterU16 >>= 4;
        out->fRcaSimple = shifterU16;
        shifterU16 >>= 1;
        out->fBelowText = shifterU16;
        shifterU16 >>= 1;
        out->fAnchorLock = shifterU16;
        shifterU16 >>= 1;
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        out++;
    }
    return bytes;
} // FSPA

//unsigned MsWordGenerated::read(const U8 *in, __UNAL FFN *out, unsigned count)
//{
//    U32 shifterU32;
//    U16 shifterU16;
//    U8 shifterU8;
//    U8 *ptr;
//    unsigned bytes = 0;
//
//    ptr = (U8 *)out;
//    shifterU32 = shifterU16 = shifterU8 = 0;
//    for (unsigned i = 0; i < count; i++)
//    {
//        bytes += read(in + bytes, (U8 *)(ptr + bytes), 1);
//        bytes += read(in + bytes, &shifterU8);
//        out->prq = shifterU8;
//        shifterU8 >>= 2;
//        out->fTrueType = shifterU8;
//        shifterU8 >>= 1;
//        out->unused1_3 = shifterU8;
//        shifterU8 >>= 1;
//        out->ff = shifterU8;
//        shifterU8 >>= 3;
//        out->unused1_7 = shifterU8;
//        shifterU8 >>= 1;
//        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
//        bytes += read(in + bytes, (U8 *)(ptr + bytes), 36);
//        out++;
//    }
//    return bytes;
//} // FFN

unsigned MsWordGenerated::read(const U8 *in, __UNAL FIB *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 5);
        bytes += read(in + bytes, &shifterU16);
        out->fDot = shifterU16;
        shifterU16 >>= 1;
        out->fGlsy = shifterU16;
        shifterU16 >>= 1;
        out->fComplex = shifterU16;
        shifterU16 >>= 1;
        out->fHasPic = shifterU16;
        shifterU16 >>= 1;
        out->cQuickSaves = shifterU16;
        shifterU16 >>= 4;
        out->fEncrypted = shifterU16;
        shifterU16 >>= 1;
        out->fWhichTblStm = shifterU16;
        shifterU16 >>= 1;
        out->fReadOnlyRecommended = shifterU16;
        shifterU16 >>= 1;
        out->fWriteReservation = shifterU16;
        shifterU16 >>= 1;
        out->fExtChar = shifterU16;
        shifterU16 >>= 1;
        out->fLoadOverride = shifterU16;
        shifterU16 >>= 1;
        out->fFarEast = shifterU16;
        shifterU16 >>= 1;
        out->fCrypto = shifterU16;
        shifterU16 >>= 1;
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 1);
        bytes += read(in + bytes, &shifterU8);
        out->fMac = shifterU8;
        shifterU8 >>= 1;
        out->fEmptySpecial = shifterU8;
        shifterU8 >>= 1;
        out->fLoadOverridePage = shifterU8;
        shifterU8 >>= 1;
        out->fFutureSavedUndo = shifterU8;
        shifterU8 >>= 1;
        out->fWord97Saved = shifterU8;
        shifterU8 >>= 1;
        out->fSpare0 = shifterU8;
        shifterU8 >>= 3;
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 16);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 22);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 186);
        out++;
    }
    return bytes;
} // FIB

unsigned MsWordGenerated::read(const U8 *in, __UNAL FIBFCLCB *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 2);
        out++;
    }
    return bytes;
} // FIBFCLCB

unsigned MsWordGenerated::read(const U8 *in, __UNAL FRD *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        out++;
    }
    return bytes;
} // FRD

//unsigned MsWordGenerated::read(const U8 *in, __UNAL CHPXFKP *out, unsigned count)
//{
//    U32 shifterU32;
//    U16 shifterU16;
//    U8 shifterU8;
//    U8 *ptr;
//    unsigned bytes = 0;
//
//    ptr = (U8 *)out;
//    shifterU32 = shifterU16 = shifterU8 = 0;
//    for (unsigned i = 0; i < count; i++)
//    {
//        bytes += read(in + bytes, (U8 *)(ptr + bytes), 1);
//        out++;
//    }
//    return bytes;
//} // CHPXFKP

//unsigned MsWordGenerated::read(const U8 *in, __UNAL PAPXFKP *out, unsigned count)
//{
//    U32 shifterU32;
//    U16 shifterU16;
//    U8 shifterU8;
//    U8 *ptr;
//    unsigned bytes = 0;
//
//    ptr = (U8 *)out;
//    shifterU32 = shifterU16 = shifterU8 = 0;
//    for (unsigned i = 0; i < count; i++)
//    {
//        bytes += read(in + bytes, (FC *)(ptr + bytes), fkp.crun+1);
//        bytes += read(in + bytes, (BX *)(ptr + bytes), fkp.crun);
//        bytes += read(in + bytes, (U8 *)(ptr + bytes), 1);
//        out++;
//    }
//    return bytes;
//} // PAPXFKP

unsigned MsWordGenerated::read(const U8 *in, __UNAL LVLF *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 1);
        bytes += read(in + bytes, &shifterU8);
        out->jc = shifterU8;
        shifterU8 >>= 2;
        out->fLegal = shifterU8;
        shifterU8 >>= 1;
        out->fNoRestart = shifterU8;
        shifterU8 >>= 1;
        out->fPrev = shifterU8;
        shifterU8 >>= 1;
        out->fPrevSpace = shifterU8;
        shifterU8 >>= 1;
        out->fWord6 = shifterU8;
        shifterU8 >>= 1;
        out->unused5_7 = shifterU8;
        shifterU8 >>= 1;
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 10);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        out++;
    }
    return bytes;
} // LVLF

unsigned MsWordGenerated::read(const U8 *in, __UNAL LSPD *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
        out++;
    }
    return bytes;
} // LSPD

unsigned MsWordGenerated::read(const U8 *in, __UNAL LSTF *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 9);
        bytes += read(in + bytes, &shifterU8);
        out->fSimpleList = shifterU8;
        shifterU8 >>= 1;
        out->fRestartHdn = shifterU8;
        shifterU8 >>= 1;
        out->unsigned26_2 = shifterU8;
        shifterU8 >>= 6;
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 1);
        out++;
    }
    return bytes;
} // LSTF

unsigned MsWordGenerated::read(const U8 *in, __UNAL LFO *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 3);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 4);
        out++;
    }
    return bytes;
} // LFO

unsigned MsWordGenerated::read(const U8 *in, __UNAL LFOLVL *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, &shifterU8);
        out->ilvl = shifterU8;
        shifterU8 >>= 4;
        out->fStartAt = shifterU8;
        shifterU8 >>= 1;
        out->fFormatting = shifterU8;
        shifterU8 >>= 1;
        out->unsigned4_6 = shifterU8;
        shifterU8 >>= 2;
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 3);
        out++;
    }
    return bytes;
} // LFOLVL

unsigned MsWordGenerated::read(const U8 *in, __UNAL OLST *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (ANLV *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 4);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 32);
        out++;
    }
    return bytes;
} // OLST

unsigned MsWordGenerated::read(const U8 *in, __UNAL NUMRM *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (DTTM *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 18);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 9);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 32);
        out++;
    }
    return bytes;
} // NUMRM

unsigned MsWordGenerated::read(const U8 *in, __UNAL PGD *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, &shifterU16);
        out->fContinue = shifterU16;
        shifterU16 >>= 1;
        out->fUnk = shifterU16;
        shifterU16 >>= 1;
        out->fRight = shifterU16;
        shifterU16 >>= 1;
        out->fPgnRestart = shifterU16;
        shifterU16 >>= 1;
        out->fEmptyPage = shifterU16;
        shifterU16 >>= 1;
        out->fAllFtn = shifterU16;
        shifterU16 >>= 1;
        out->unused0_6 = shifterU16;
        shifterU16 >>= 1;
        out->fTableBreaks = shifterU16;
        shifterU16 >>= 1;
        out->fMarked = shifterU16;
        shifterU16 >>= 1;
        out->fColumnBreaks = shifterU16;
        shifterU16 >>= 1;
        out->fTableHeader = shifterU16;
        shifterU16 >>= 1;
        out->fNewPage = shifterU16;
        shifterU16 >>= 1;
        out->bkc = shifterU16;
        shifterU16 >>= 4;
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        out++;
    }
    return bytes;
} // PGD

unsigned MsWordGenerated::read(const U8 *in, __UNAL PHE *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, &shifterU16);
        out->fSpare = shifterU16;
        shifterU16 >>= 1;
        out->fUnk = shifterU16;
        shifterU16 >>= 1;
        out->fDiffLines = shifterU16;
        shifterU16 >>= 1;
        out->unused0_3 = shifterU16;
        shifterU16 >>= 5;
        out->clMac = shifterU16;
        shifterU16 >>= 8;
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 2);
        out++;
    }
    return bytes;
} // PHE

unsigned MsWordGenerated::read(const U8 *in, __UNAL PAP *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 4);
        bytes += read(in + bytes, &shifterU8);
        out->fBrLnAbove = shifterU8;
        shifterU8 >>= 1;
        out->fBrLnBelow = shifterU8;
        shifterU8 >>= 1;
        out->fUnused = shifterU8;
        shifterU8 >>= 2;
        out->pcVert = shifterU8;
        shifterU8 >>= 2;
        out->pcHorz = shifterU8;
        shifterU8 >>= 2;
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 5);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 6);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 3);
        bytes += read(in + bytes, (LSPD *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (PHE *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 10);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, &shifterU16);
        out->fVertical = shifterU16;
        shifterU16 >>= 1;
        out->fBackward = shifterU16;
        shifterU16 >>= 1;
        out->fRotateFont = shifterU16;
        shifterU16 >>= 1;
        out->unused68_3 = shifterU16;
        shifterU16 >>= 13;
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 4);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 4);
        bytes += read(in + bytes, (BRC *)(ptr + bytes), 6);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 2);
        bytes += read(in + bytes, &shifterU16);
        out->dyaHeight = shifterU16;
        shifterU16 >>= 15;
        out->fMinHeight = shifterU16;
        shifterU16 >>= 1;
        bytes += read(in + bytes, (SHD *)(ptr + bytes), 1);
        bytes += read(in + bytes, (DCS *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (ANLD *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (DTTM *)(ptr + bytes), 1);
        bytes += read(in + bytes, (NUMRM *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 129);
        out++;
    }
    return bytes;
} // PAP

//unsigned MsWordGenerated::read(const U8 *in, __UNAL PAPX *out, unsigned count)
//{
//    U32 shifterU32;
//    U16 shifterU16;
//    U8 shifterU8;
//    U8 *ptr;
//    unsigned bytes = 0;
//
//    ptr = (U8 *)out;
//    shifterU32 = shifterU16 = shifterU8 = 0;
//    for (unsigned i = 0; i < count; i++)
//    {
//        bytes += read(in + bytes, (U8 *)(ptr + bytes), 3);
//        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
//        bytes += read(in + bytes, (character array *)(ptr + bytes), 1);
//        out++;
//    }
//    return bytes;
//} // PAPX

unsigned MsWordGenerated::read(const U8 *in, __UNAL PICF *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 5);
        out++;
    }
    return bytes;
} // PICF

unsigned MsWordGenerated::read(const U8 *in, __UNAL PCD *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, &shifterU16);
        out->fNoParaLast = shifterU16;
        shifterU16 >>= 1;
        out->fPaphNil = shifterU16;
        shifterU16 >>= 1;
        out->fCopied = shifterU16;
        shifterU16 >>= 1;
        out->unused0_3 = shifterU16;
        shifterU16 >>= 5;
        out->fn = shifterU16;
        shifterU16 >>= 8;
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (PRM *)(ptr + bytes), 1);
        out++;
    }
    return bytes;
} // PCD

//unsigned MsWordGenerated::read(const U8 *in, __UNAL PLCF *out, unsigned count)
//{
//    U32 shifterU32;
//    U16 shifterU16;
//    U8 shifterU8;
//    U8 *ptr;
//    unsigned bytes = 0;
//
//    ptr = (U8 *)out;
//    shifterU32 = shifterU16 = shifterU8 = 0;
//    for (unsigned i = 0; i < count; i++)
//    {
//        out++;
//    }
//    return bytes;
//} // PLCF

unsigned MsWordGenerated::read(const U8 *in, __UNAL PRM2 *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, &shifterU16);
        out->fComplex = shifterU16;
        shifterU16 >>= 1;
        out->igrpprl = shifterU16;
        shifterU16 >>= 15;
        out++;
    }
    return bytes;
} // PRM2

unsigned MsWordGenerated::read(const U8 *in, __UNAL RS *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 8);
        out++;
    }
    return bytes;
} // RS

unsigned MsWordGenerated::read(const U8 *in, __UNAL RR *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
        out++;
    }
    return bytes;
} // RR

unsigned MsWordGenerated::read(const U8 *in, __UNAL SED *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        out++;
    }
    return bytes;
} // SED

unsigned MsWordGenerated::read(const U8 *in, __UNAL SEP *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 10);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 3);
        bytes += read(in + bytes, (BRC *)(ptr + bytes), 4);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (DTTM *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 5);
        bytes += read(in + bytes, &shifterU16);
        out->pgbApplyTo = shifterU16;
        shifterU16 >>= 3;
        out->pgbPageDepth = shifterU16;
        shifterU16 >>= 2;
        out->pgbOffsetFrom = shifterU16;
        shifterU16 >>= 3;
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 11);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 91);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (OLST *)(ptr + bytes), 1);
        out++;
    }
    return bytes;
} // SEP

//unsigned MsWordGenerated::read(const U8 *in, __UNAL SEPX *out, unsigned count)
//{
//    U32 shifterU32;
//    U16 shifterU16;
//    U8 shifterU8;
//    U8 *ptr;
//    unsigned bytes = 0;
//
//    ptr = (U8 *)out;
//    shifterU32 = shifterU16 = shifterU8 = 0;
//    for (unsigned i = 0; i < count; i++)
//    {
//        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
//        out++;
//    }
//    return bytes;
//} // SEPX

unsigned MsWordGenerated::read(const U8 *in, __UNAL STSHI *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
        bytes += read(in + bytes, &shifterU16);
        out->fStdStylenamesWritten = shifterU16;
        shifterU16 >>= 1;
        out->unused4_2 = shifterU16;
        shifterU16 >>= 15;
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 6);
        out++;
    }
    return bytes;
} // STSHI

//unsigned MsWordGenerated::read(const U8 *in, __UNAL STD *out, unsigned count)
//{
//    U32 shifterU32;
//    U16 shifterU16;
//    U8 shifterU8;
//    U8 *ptr;
//    unsigned bytes = 0;
//
//    ptr = (U8 *)out;
//    shifterU32 = shifterU16 = shifterU8 = 0;
//    for (unsigned i = 0; i < count; i++)
//    {
//        bytes += read(in + bytes, &shifterU16);
//        out->sti = shifterU16;
//        shifterU16 >>= 12;
//        out->fScratch = shifterU16;
//        shifterU16 >>= 1;
//        out->fInvalHeight = shifterU16;
//        shifterU16 >>= 1;
//        out->fHasUpe = shifterU16;
//        shifterU16 >>= 1;
//        out->fMassCopy = shifterU16;
//        shifterU16 >>= 1;
//        bytes += read(in + bytes, &shifterU16);
//        out->sgc = shifterU16;
//        shifterU16 >>= 4;
//        out->istdBase = shifterU16;
//        shifterU16 >>= 12;
//        bytes += read(in + bytes, &shifterU16);
//        out->cupx = shifterU16;
//        shifterU16 >>= 4;
//        out->istdNext = shifterU16;
//        shifterU16 >>= 12;
//        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
//        bytes += read(in + bytes, &shifterU16);
//        out->fAutoRedef = shifterU16;
//        shifterU16 >>= 1;
//        out->fHidden = shifterU16;
//        shifterU16 >>= 1;
//        out->unused8_3 = shifterU16;
//        shifterU16 >>= 14;
//        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
//        out++;
//    }
//    return bytes;
//} // STD

unsigned MsWordGenerated::read(const U8 *in, __UNAL TBD *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, &shifterU8);
        out->jc = shifterU8;
        shifterU8 >>= 3;
        out->tlc = shifterU8;
        shifterU8 >>= 3;
        out->unused0_6 = shifterU8;
        shifterU8 >>= 2;
        out++;
    }
    return bytes;
} // TBD

unsigned MsWordGenerated::read(const U8 *in, __UNAL TC *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, &shifterU16);
        out->fFirstMerged = shifterU16;
        shifterU16 >>= 1;
        out->fMerged = shifterU16;
        shifterU16 >>= 1;
        out->fVertical = shifterU16;
        shifterU16 >>= 1;
        out->fBackward = shifterU16;
        shifterU16 >>= 1;
        out->fRotateFont = shifterU16;
        shifterU16 >>= 1;
        out->fVertMerge = shifterU16;
        shifterU16 >>= 1;
        out->fVertRestart = shifterU16;
        shifterU16 >>= 1;
        out->vertAlign = shifterU16;
        shifterU16 >>= 2;
        out->fUnused = shifterU16;
        shifterU16 >>= 7;
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (BRC *)(ptr + bytes), 4);
        out++;
    }
    return bytes;
} // TC

unsigned MsWordGenerated::read(const U8 *in, __UNAL TLP *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, &shifterU16);
        out->fBorders = shifterU16;
        shifterU16 >>= 1;
        out->fShading = shifterU16;
        shifterU16 >>= 1;
        out->fFont = shifterU16;
        shifterU16 >>= 1;
        out->fColor = shifterU16;
        shifterU16 >>= 1;
        out->fBestFit = shifterU16;
        shifterU16 >>= 1;
        out->fHdrRows = shifterU16;
        shifterU16 >>= 1;
        out->fLastRow = shifterU16;
        shifterU16 >>= 1;
        out->fHdrCols = shifterU16;
        shifterU16 >>= 1;
        out->fLastCol = shifterU16;
        shifterU16 >>= 1;
        out++;
    }
    return bytes;
} // TLP

unsigned MsWordGenerated::read(const U8 *in, __UNAL TAP *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (TLP *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, &shifterU16);
        out->fCaFull = shifterU16;
        shifterU16 >>= 1;
        out->fFirstRow = shifterU16;
        shifterU16 >>= 1;
        out->fLastRow = shifterU16;
        shifterU16 >>= 1;
        out->fOutline = shifterU16;
        shifterU16 >>= 1;
        out->unused20_12 = shifterU16;
        shifterU16 >>= 12;
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 3);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 130);
        bytes += read(in + bytes, (TC *)(ptr + bytes), 64);
        bytes += read(in + bytes, (SHD *)(ptr + bytes), 64);
        bytes += read(in + bytes, (BRC *)(ptr + bytes), 6);
        out++;
    }
    return bytes;
} // TAP

unsigned MsWordGenerated::read(const U8 *in, __UNAL FTXBXS *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 3);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 2);
        out++;
    }
    return bytes;
} // FTXBXS

unsigned MsWordGenerated::read(const U8 *in, __UNAL WKB *out, unsigned count)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    U8 *ptr;
    unsigned bytes = 0;

    ptr = (U8 *)out;
    shifterU32 = shifterU16 = shifterU8 = 0;
    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 3);
        bytes += read(in + bytes, &shifterU16);
        out->fnpt = shifterU16;
        shifterU16 >>= 4;
        out->fnpd = shifterU16;
        shifterU16 >>= 12;
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        out++;
    }
    return bytes;
} // WKB

