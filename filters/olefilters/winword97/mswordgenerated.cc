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

unsigned MsWordGenerated::read(const U8 *in, U8 *out, unsigned count)
{
    memcpy(out, in, count);
    return count;
}

unsigned MsWordGenerated::read(const U8 *in, S8 *out, unsigned count)
{
    return read(in, (U8 *)out, count);
}

unsigned MsWordGenerated::read(const U8 *in, U16 *out, unsigned count)
{
    U16 tmp;

    for (unsigned i = 0; i < count; i++)
    {
        tmp = (U16)((*(in + 1) << 8) + (*in));
        memcpy(out, &tmp, sizeof(tmp));
        out++;
        in += sizeof(tmp);
    }
    return count + count;
}

unsigned MsWordGenerated::read(const U8 *in, S16 *out, unsigned count)
{
    return read(in, (U16 *)out, count);
}

unsigned MsWordGenerated::read(const U8 *in, U32 *out, unsigned count)
{
    U32 tmp;

    for (unsigned i = 0; i < count; i++)
    {
        tmp = (U32)((*(in + 3) << 24) + (*(in + 2) << 16) + (*(in + 1) << 8) + (*in));
        memcpy(out, &tmp, sizeof(tmp));
        out++;
        in += sizeof(tmp);
    }
    return count + count + count + count;
}

unsigned MsWordGenerated::read(const U8 *in, S32 *out, unsigned count)
{
    return read(in, (U32 *)out, count);
}

unsigned MsWordGenerated::read(const U8 *in, DTTM *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->mint = shiftRegister;
        shiftRegister >>= 6;
        out->hr = shiftRegister;
        shiftRegister >>= 5;
        out->dom = shiftRegister;
        shiftRegister >>= 5;
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->mon = shiftRegister;
        shiftRegister >>= 4;
        out->yr = shiftRegister;
        shiftRegister >>= 9;
        out->wdy = shiftRegister;
        shiftRegister >>= 3;
        out++;
    }
    return bytes;
} // DTTM

unsigned MsWordGenerated::read(const U8 *in, SHD *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->icoFore = shiftRegister;
        shiftRegister >>= 5;
        out->icoBack = shiftRegister;
        shiftRegister >>= 5;
        out->ipat = shiftRegister;
        shiftRegister >>= 6;
        out++;
    }
    return bytes;
} // SHD

unsigned MsWordGenerated::read(const U8 *in, DOPTYPOGRAPHY *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->fKerningPunct = shiftRegister;
        shiftRegister >>= 1;
        out->iJustification = shiftRegister;
        shiftRegister >>= 2;
        out->iLevelOfKinsoku = shiftRegister;
        shiftRegister >>= 2;
        out->f2on1 = shiftRegister;
        shiftRegister >>= 1;
        out->unused0_6 = shiftRegister;
        shiftRegister >>= 10;
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 154);
        out++;
    }
    return bytes;
} // DOPTYPOGRAPHY

unsigned MsWordGenerated::read(const U8 *in, PRM *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->fComplex = shiftRegister;
        shiftRegister >>= 1;
        out->isprm = shiftRegister;
        shiftRegister >>= 7;
        out->val = shiftRegister;
        shiftRegister >>= 8;
        out++;
    }
    return bytes;
} // PRM

unsigned MsWordGenerated::read(const U8 *in, ATRD *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 11);
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->ak = shiftRegister;
        shiftRegister >>= 2;
        out->unused22_2 = shiftRegister;
        shiftRegister >>= 14;
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        out++;
    }
    return bytes;
} // ATRD

unsigned MsWordGenerated::read(const U8 *in, ANLD *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 3);
        bytes += read(in + bytes, (U8 *)&shiftRegister);
        out->jc = shiftRegister;
        shiftRegister >>= 2;
        out->fPrev = shiftRegister;
        shiftRegister >>= 1;
        out->fHang = shiftRegister;
        shiftRegister >>= 1;
        out->fSetBold = shiftRegister;
        shiftRegister >>= 1;
        out->fSetItalic = shiftRegister;
        shiftRegister >>= 1;
        out->fSetSmallCaps = shiftRegister;
        shiftRegister >>= 1;
        out->fSetCaps = shiftRegister;
        shiftRegister >>= 1;
        bytes += read(in + bytes, (U8 *)&shiftRegister);
        out->fSetStrike = shiftRegister;
        shiftRegister >>= 1;
        out->fSetKul = shiftRegister;
        shiftRegister >>= 1;
        out->fPrevSpace = shiftRegister;
        shiftRegister >>= 1;
        out->fBold = shiftRegister;
        shiftRegister >>= 1;
        out->fItalic = shiftRegister;
        shiftRegister >>= 1;
        out->fSmallCaps = shiftRegister;
        shiftRegister >>= 1;
        out->fCaps = shiftRegister;
        shiftRegister >>= 1;
        out->fStrike = shiftRegister;
        shiftRegister >>= 1;
        bytes += read(in + bytes, (U8 *)&shiftRegister);
        out->kul = shiftRegister;
        shiftRegister >>= 3;
        out->ico = shiftRegister;
        shiftRegister >>= 5;
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 5);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 4);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 32);
        out++;
    }
    return bytes;
} // ANLD

unsigned MsWordGenerated::read(const U8 *in, ANLV *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 3);
        bytes += read(in + bytes, (U8 *)&shiftRegister);
        out->jc = shiftRegister;
        shiftRegister >>= 2;
        out->fPrev = shiftRegister;
        shiftRegister >>= 1;
        out->fHang = shiftRegister;
        shiftRegister >>= 1;
        out->fSetBold = shiftRegister;
        shiftRegister >>= 1;
        out->fSetItalic = shiftRegister;
        shiftRegister >>= 1;
        out->fSetSmallCaps = shiftRegister;
        shiftRegister >>= 1;
        out->fSetCaps = shiftRegister;
        shiftRegister >>= 1;
        bytes += read(in + bytes, (U8 *)&shiftRegister);
        out->fSetStrike = shiftRegister;
        shiftRegister >>= 1;
        out->fSetKul = shiftRegister;
        shiftRegister >>= 1;
        out->fPrevSpace = shiftRegister;
        shiftRegister >>= 1;
        out->fBold = shiftRegister;
        shiftRegister >>= 1;
        out->FItalic = shiftRegister;
        shiftRegister >>= 1;
        out->fSmallCaps = shiftRegister;
        shiftRegister >>= 1;
        out->fCaps = shiftRegister;
        shiftRegister >>= 1;
        out->fStrike = shiftRegister;
        shiftRegister >>= 1;
        bytes += read(in + bytes, (U8 *)&shiftRegister);
        out->kul = shiftRegister;
        shiftRegister >>= 3;
        out->ico = shiftRegister;
        shiftRegister >>= 5;
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 5);
        out++;
    }
    return bytes;
} // ANLV

unsigned MsWordGenerated::read(const U8 *in, ASUMY *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        out++;
    }
    return bytes;
} // ASUMY

unsigned MsWordGenerated::read(const U8 *in, ASUMYI *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->fValid = shiftRegister;
        shiftRegister >>= 1;
        out->fView = shiftRegister;
        shiftRegister >>= 1;
        out->iViewBy = shiftRegister;
        shiftRegister >>= 2;
        out->fUpdateProps = shiftRegister;
        shiftRegister >>= 1;
        out->unused0_5 = shiftRegister;
        shiftRegister >>= 11;
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 2);
        out++;
    }
    return bytes;
} // ASUMYI

unsigned MsWordGenerated::read(const U8 *in, BTE *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        out++;
    }
    return bytes;
} // BTE

unsigned MsWordGenerated::read(const U8 *in, BKD *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 3);
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->icol = shiftRegister;
        shiftRegister >>= 8;
        out->fTableBreak = shiftRegister;
        shiftRegister >>= 1;
        out->fColumnBreak = shiftRegister;
        shiftRegister >>= 1;
        out->fMarked = shiftRegister;
        shiftRegister >>= 1;
        out->fUnk = shiftRegister;
        shiftRegister >>= 1;
        out->fTextOverflow = shiftRegister;
        shiftRegister >>= 1;
        out++;
    }
    return bytes;
} // BKD

unsigned MsWordGenerated::read(const U8 *in, BKF *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->itcFirst = shiftRegister;
        shiftRegister >>= 7;
        out->fPub = shiftRegister;
        shiftRegister >>= 1;
        out->itcLim = shiftRegister;
        shiftRegister >>= 7;
        out->fCol = shiftRegister;
        shiftRegister >>= 1;
        out++;
    }
    return bytes;
} // BKF

unsigned MsWordGenerated::read(const U8 *in, BKL *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        out++;
    }
    return bytes;
} // BKL

unsigned MsWordGenerated::read(const U8 *in, BRC *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->dptLineWidth = shiftRegister;
        shiftRegister >>= 8;
        out->brcType = shiftRegister;
        shiftRegister >>= 8;
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->ico = shiftRegister;
        shiftRegister >>= 8;
        out->dptSpace = shiftRegister;
        shiftRegister >>= 5;
        out->fShadow = shiftRegister;
        shiftRegister >>= 1;
        out->fFrame = shiftRegister;
        shiftRegister >>= 1;
        out->unused2_15 = shiftRegister;
        shiftRegister >>= 1;
        out++;
    }
    return bytes;
} // BRC

unsigned MsWordGenerated::read(const U8 *in, BRC10 *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->dxpLine2Width = shiftRegister;
        shiftRegister >>= 3;
        out->dxpSpaceBetween = shiftRegister;
        shiftRegister >>= 3;
        out->dxpLine1Width = shiftRegister;
        shiftRegister >>= 3;
        out->dxpSpace = shiftRegister;
        shiftRegister >>= 5;
        out->fShadow = shiftRegister;
        shiftRegister >>= 1;
        out->fSpare = shiftRegister;
        shiftRegister >>= 1;
        out++;
    }
    return bytes;
} // BRC10

unsigned MsWordGenerated::read(const U8 *in, CHP *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U8 *)&shiftRegister);
        out->fBold = shiftRegister;
        shiftRegister >>= 1;
        out->FItalic = shiftRegister;
        shiftRegister >>= 1;
        out->fRMarkDel = shiftRegister;
        shiftRegister >>= 1;
        out->fOutline = shiftRegister;
        shiftRegister >>= 1;
        out->fFldVanish = shiftRegister;
        shiftRegister >>= 1;
        out->fSmallCaps = shiftRegister;
        shiftRegister >>= 1;
        out->fCaps = shiftRegister;
        shiftRegister >>= 1;
        out->fVanish = shiftRegister;
        shiftRegister >>= 1;
        bytes += read(in + bytes, (U8 *)&shiftRegister);
        out->fRMark = shiftRegister;
        shiftRegister >>= 1;
        out->fSpec = shiftRegister;
        shiftRegister >>= 1;
        out->fStrike = shiftRegister;
        shiftRegister >>= 1;
        out->fObj = shiftRegister;
        shiftRegister >>= 1;
        out->fShadow = shiftRegister;
        shiftRegister >>= 1;
        out->fLowerCase = shiftRegister;
        shiftRegister >>= 1;
        out->fData = shiftRegister;
        shiftRegister >>= 1;
        out->fOle2 = shiftRegister;
        shiftRegister >>= 1;
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->fEmboss = shiftRegister;
        shiftRegister >>= 1;
        out->fImprint = shiftRegister;
        shiftRegister >>= 1;
        out->fDStrike = shiftRegister;
        shiftRegister >>= 1;
        out->fUsePgsuSettings = shiftRegister;
        shiftRegister >>= 1;
        out->unused2_4 = shiftRegister;
        shiftRegister >>= 12;
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 5);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)&shiftRegister);
        out->iss = shiftRegister;
        shiftRegister >>= 3;
        out->kul = shiftRegister;
        shiftRegister >>= 4;
        out->fSpecSymbol = shiftRegister;
        shiftRegister >>= 1;
        bytes += read(in + bytes, (U8 *)&shiftRegister);
        out->ico = shiftRegister;
        shiftRegister >>= 5;
        out->unused23_5 = shiftRegister;
        shiftRegister >>= 1;
        out->fSysVanish = shiftRegister;
        shiftRegister >>= 1;
        out->hpsPos = shiftRegister;
        shiftRegister >>= 1;
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 4);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (DTTM *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 6);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->icoHighlight = shiftRegister;
        shiftRegister >>= 5;
        out->fHighlight = shiftRegister;
        shiftRegister >>= 1;
        out->kcd = shiftRegister;
        shiftRegister >>= 3;
        out->fNavHighlight = shiftRegister;
        shiftRegister >>= 1;
        out->fChsDiff = shiftRegister;
        shiftRegister >>= 1;
        out->fMacChs = shiftRegister;
        shiftRegister >>= 1;
        out->fFtcAsciSym = shiftRegister;
        shiftRegister >>= 1;
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

//unsigned MsWordGenerated::read(const U8 *in, CHPX *out, unsigned count)
//{
//    unsigned long shiftRegister;
//    U8 *ptr = (U8 *)out;
//    unsigned bytes = 0;
//
//    for (unsigned i = 0; i < count; i++)
//    {
//        bytes += read(in + bytes, (U8 *)(ptr + bytes), 1);
//        bytes += read(in + bytes, (U8 *)(ptr + bytes), cb);
//        out++;
//    }
//    return bytes;
//} // CHPX

unsigned MsWordGenerated::read(const U8 *in, DCS *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U8 *)&shiftRegister);
        out->fdct = shiftRegister;
        shiftRegister >>= 3;
        out->unused0_3 = shiftRegister;
        shiftRegister >>= 5;
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        out++;
    }
    return bytes;
} // DCS

unsigned MsWordGenerated::read(const U8 *in, DOGRID *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 4);
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->dyGridDisplay = shiftRegister;
        shiftRegister >>= 7;
        out->fTurnItOff = shiftRegister;
        shiftRegister >>= 1;
        out->dxGridDisplay = shiftRegister;
        shiftRegister >>= 7;
        out->fFollowMargins = shiftRegister;
        shiftRegister >>= 1;
        out++;
    }
    return bytes;
} // DOGRID

unsigned MsWordGenerated::read(const U8 *in, DOP *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->fFacingPages = shiftRegister;
        shiftRegister >>= 1;
        out->fWidowControl = shiftRegister;
        shiftRegister >>= 1;
        out->fPMHMainDoc = shiftRegister;
        shiftRegister >>= 1;
        out->grfSuppression = shiftRegister;
        shiftRegister >>= 2;
        out->fpc = shiftRegister;
        shiftRegister >>= 2;
        out->unused0_7 = shiftRegister;
        shiftRegister >>= 1;
        out->grpfIhdt = shiftRegister;
        shiftRegister >>= 8;
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->rncFtn = shiftRegister;
        shiftRegister >>= 2;
        out->nFtn = shiftRegister;
        shiftRegister >>= 14;
        bytes += read(in + bytes, (U8 *)&shiftRegister);
        out->fOutlineDirtySave = shiftRegister;
        shiftRegister >>= 1;
        out->unused4_1 = shiftRegister;
        shiftRegister >>= 7;
        bytes += read(in + bytes, (U8 *)&shiftRegister);
        out->fOnlyMacPics = shiftRegister;
        shiftRegister >>= 1;
        out->fOnlyWinPics = shiftRegister;
        shiftRegister >>= 1;
        out->fLabelDoc = shiftRegister;
        shiftRegister >>= 1;
        out->fHyphCapitals = shiftRegister;
        shiftRegister >>= 1;
        out->fAutoHyphen = shiftRegister;
        shiftRegister >>= 1;
        out->fFormNoFields = shiftRegister;
        shiftRegister >>= 1;
        out->fLinkStyles = shiftRegister;
        shiftRegister >>= 1;
        out->fRevMarking = shiftRegister;
        shiftRegister >>= 1;
        bytes += read(in + bytes, (U8 *)&shiftRegister);
        out->fBackup = shiftRegister;
        shiftRegister >>= 1;
        out->fExactCWords = shiftRegister;
        shiftRegister >>= 1;
        out->fPagHidden = shiftRegister;
        shiftRegister >>= 1;
        out->fPagResults = shiftRegister;
        shiftRegister >>= 1;
        out->fLockAtn = shiftRegister;
        shiftRegister >>= 1;
        out->fMirrorMargins = shiftRegister;
        shiftRegister >>= 1;
        out->fDfltTrueType = shiftRegister;
        shiftRegister >>= 1;
        bytes += read(in + bytes, (U8 *)&shiftRegister);
        out->fPagSuppressTopSpacing = shiftRegister;
        shiftRegister >>= 1;
        out->fProtEnabled = shiftRegister;
        shiftRegister >>= 1;
        out->fDispFormFldSel = shiftRegister;
        shiftRegister >>= 1;
        out->fRMView = shiftRegister;
        shiftRegister >>= 1;
        out->fRMPrint = shiftRegister;
        shiftRegister >>= 1;
        out->fLockRev = shiftRegister;
        shiftRegister >>= 1;
        out->fEmbedFonts = shiftRegister;
        shiftRegister >>= 1;
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->copts_fNoTabForInd = shiftRegister;
        shiftRegister >>= 1;
        out->copts_fNoSpaceRaiseLower = shiftRegister;
        shiftRegister >>= 1;
        out->copts_fSuppressSpbfAfterPageBreak = shiftRegister;
        shiftRegister >>= 1;
        out->copts_fWrapTrailSpaces = shiftRegister;
        shiftRegister >>= 1;
        out->copts_fMapPrintTextColor = shiftRegister;
        shiftRegister >>= 1;
        out->copts_fNoColumnBalance = shiftRegister;
        shiftRegister >>= 1;
        out->copts_fConvMailMergeEsc = shiftRegister;
        shiftRegister >>= 1;
        out->copts_fSupressTopSpacing = shiftRegister;
        shiftRegister >>= 1;
        out->copts_fOrigWordTableRules = shiftRegister;
        shiftRegister >>= 1;
        out->copts_fTransparentMetafiles = shiftRegister;
        shiftRegister >>= 1;
        out->copts_fShowBreaksInFrames = shiftRegister;
        shiftRegister >>= 1;
        out->copts_fSwapBordersFacingPgs = shiftRegister;
        shiftRegister >>= 1;
        out->unused8_12 = shiftRegister;
        shiftRegister >>= 4;
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 5);
        bytes += read(in + bytes, (DTTM *)(ptr + bytes), 3);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 3);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->rncEdn = shiftRegister;
        shiftRegister >>= 2;
        out->nEdn = shiftRegister;
        shiftRegister >>= 14;
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->epc = shiftRegister;
        shiftRegister >>= 2;
        out->nfcFtnRef = shiftRegister;
        shiftRegister >>= 4;
        out->nfcEdnRef = shiftRegister;
        shiftRegister >>= 4;
        out->fPrintFormData = shiftRegister;
        shiftRegister >>= 1;
        out->fSaveFormData = shiftRegister;
        shiftRegister >>= 1;
        out->fShadeFormData = shiftRegister;
        shiftRegister >>= 1;
        out->unused54_13 = shiftRegister;
        shiftRegister >>= 2;
        out->fWCFtnEdn = shiftRegister;
        shiftRegister >>= 1;
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 3);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 3);
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->wvkSaved = shiftRegister;
        shiftRegister >>= 3;
        out->wScaleSaved = shiftRegister;
        shiftRegister >>= 9;
        out->zkSaved = shiftRegister;
        shiftRegister >>= 2;
        out->fRotateFontW6 = shiftRegister;
        shiftRegister >>= 1;
        out->iGutterPos = shiftRegister;
        shiftRegister >>= 1;
        out++;
    }
    return bytes;
} // DOP

unsigned MsWordGenerated::read(const U8 *in, DOP102 *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U32 *)&shiftRegister);
        out->fNoTabForInd = shiftRegister;
        shiftRegister >>= 1;
        out->fNoSpaceRaiseLower = shiftRegister;
        shiftRegister >>= 1;
        out->fSupressSpbfAfterPageBreak = shiftRegister;
        shiftRegister >>= 1;
        out->fWrapTrailSpaces = shiftRegister;
        shiftRegister >>= 1;
        out->fMapPrintTextColor = shiftRegister;
        shiftRegister >>= 1;
        out->fNoColumnBalance = shiftRegister;
        shiftRegister >>= 1;
        out->fConvMailMergeEsc = shiftRegister;
        shiftRegister >>= 1;
        out->fSupressTopSpacing = shiftRegister;
        shiftRegister >>= 1;
        out->fOrigWordTableRules = shiftRegister;
        shiftRegister >>= 1;
        out->fTransparentMetafiles = shiftRegister;
        shiftRegister >>= 1;
        out->fShowBreaksInFrames = shiftRegister;
        shiftRegister >>= 1;
        out->fSwapBordersFacingPgs = shiftRegister;
        shiftRegister >>= 1;
        out->unused84_12 = shiftRegister;
        shiftRegister >>= 4;
        out->fSuppressTopSpacingMac5 = shiftRegister;
        shiftRegister >>= 1;
        out->fTruncDxaExpand = shiftRegister;
        shiftRegister >>= 1;
        out->fPrintBodyBeforeHdr = shiftRegister;
        shiftRegister >>= 1;
        out->fNoLeading = shiftRegister;
        shiftRegister >>= 1;
        out->unused84_20 = shiftRegister;
        shiftRegister >>= 1;
        out->fMWSmallCaps = shiftRegister;
        shiftRegister >>= 1;
        out->unused84_22 = shiftRegister;
        shiftRegister >>= 10;
        out++;
    }
    return bytes;
} // DOP102

unsigned MsWordGenerated::read(const U8 *in, DOP105 *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (DOPTYPOGRAPHY *)(ptr + bytes), 1);
        bytes += read(in + bytes, (DOGRID *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->reserved = shiftRegister;
        shiftRegister >>= 1;
        out->lvl = shiftRegister;
        shiftRegister >>= 4;
        out->fGramAllDone = shiftRegister;
        shiftRegister >>= 1;
        out->fGramAllClean = shiftRegister;
        shiftRegister >>= 1;
        out->fSubsetFonts = shiftRegister;
        shiftRegister >>= 1;
        out->fHideLastVersion = shiftRegister;
        shiftRegister >>= 1;
        out->fHtmlDoc = shiftRegister;
        shiftRegister >>= 1;
        out->unused410_11 = shiftRegister;
        shiftRegister >>= 1;
        out->fSnapBorder = shiftRegister;
        shiftRegister >>= 1;
        out->fIncludeHeader = shiftRegister;
        shiftRegister >>= 1;
        out->fIncludeFooter = shiftRegister;
        shiftRegister >>= 1;
        out->fForcePageSizePag = shiftRegister;
        shiftRegister >>= 1;
        out->fMinFontSizePag = shiftRegister;
        shiftRegister >>= 1;
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->fHaveVersions = shiftRegister;
        shiftRegister >>= 1;
        out->fAutoVersion = shiftRegister;
        shiftRegister >>= 1;
        out->unused412_2 = shiftRegister;
        shiftRegister >>= 14;
        bytes += read(in + bytes, (ASUMYI *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 3);
        bytes += read(in + bytes, (U32 *)&shiftRegister);
        out->fVirusPrompted = shiftRegister;
        shiftRegister >>= 1;
        out->fVirusLoadSafe = shiftRegister;
        shiftRegister >>= 1;
        out->KeyVirusSession30 = shiftRegister;
        shiftRegister >>= 30;
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 30);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 5);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 4);
        out++;
    }
    return bytes;
} // DOP105

unsigned MsWordGenerated::read(const U8 *in, FLD *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U8 *)&shiftRegister);
        out->ch = shiftRegister;
        shiftRegister >>= 5;
        out->unused0_5 = shiftRegister;
        shiftRegister >>= 3;
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 1);
        out++;
    }
    return bytes;
} // FLD

unsigned MsWordGenerated::read(const U8 *in, FSPA *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 5);
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->fHdr = shiftRegister;
        shiftRegister >>= 1;
        out->bx = shiftRegister;
        shiftRegister >>= 2;
        out->by = shiftRegister;
        shiftRegister >>= 2;
        out->wr = shiftRegister;
        shiftRegister >>= 4;
        out->wrk = shiftRegister;
        shiftRegister >>= 4;
        out->fRcaSimple = shiftRegister;
        shiftRegister >>= 1;
        out->fBelowText = shiftRegister;
        shiftRegister >>= 1;
        out->fAnchorLock = shiftRegister;
        shiftRegister >>= 1;
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        out++;
    }
    return bytes;
} // FSPA

//unsigned MsWordGenerated::read(const U8 *in, FFN *out, unsigned count)
//{
//    unsigned long shiftRegister;
//    U8 *ptr = (U8 *)out;
//    unsigned bytes = 0;
//
//    for (unsigned i = 0; i < count; i++)
//    {
//        bytes += read(in + bytes, (U8 *)(ptr + bytes), 1);
//        bytes += read(in + bytes, (U8 *)&shiftRegister);
//        out->prq = shiftRegister;
//        shiftRegister >>= 2;
//        out->fTrueType = shiftRegister;
//        shiftRegister >>= 1;
//        out->unused1_3 = shiftRegister;
//        shiftRegister >>= 1;
//        out->ff = shiftRegister;
//        shiftRegister >>= 3;
//        out->unused1_7 = shiftRegister;
//        shiftRegister >>= 1;
//        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
//        bytes += read(in + bytes, (U8 *)(ptr + bytes), 36);
//        out++;
//    }
//    return bytes;
//} // FFN

unsigned MsWordGenerated::read(const U8 *in, FIB *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 5);
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->fDot = shiftRegister;
        shiftRegister >>= 1;
        out->fGlsy = shiftRegister;
        shiftRegister >>= 1;
        out->fComplex = shiftRegister;
        shiftRegister >>= 1;
        out->fHasPic = shiftRegister;
        shiftRegister >>= 1;
        out->cQuickSaves = shiftRegister;
        shiftRegister >>= 4;
        out->fEncrypted = shiftRegister;
        shiftRegister >>= 1;
        out->fWhichTblStm = shiftRegister;
        shiftRegister >>= 1;
        out->fReadOnlyRecommended = shiftRegister;
        shiftRegister >>= 1;
        out->fWriteReservation = shiftRegister;
        shiftRegister >>= 1;
        out->fExtChar = shiftRegister;
        shiftRegister >>= 1;
        out->fLoadOverride = shiftRegister;
        shiftRegister >>= 1;
        out->fFarEast = shiftRegister;
        shiftRegister >>= 1;
        out->fCrypto = shiftRegister;
        shiftRegister >>= 1;
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)&shiftRegister);
        out->fMac = shiftRegister;
        shiftRegister >>= 1;
        out->fEmptySpecial = shiftRegister;
        shiftRegister >>= 1;
        out->fLoadOverridePage = shiftRegister;
        shiftRegister >>= 1;
        out->fFutureSavedUndo = shiftRegister;
        shiftRegister >>= 1;
        out->fWord97Saved = shiftRegister;
        shiftRegister >>= 1;
        out->fSpare0 = shiftRegister;
        shiftRegister >>= 3;
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

unsigned MsWordGenerated::read(const U8 *in, FIBFCLCB *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 2);
        out++;
    }
    return bytes;
} // FIBFCLCB

unsigned MsWordGenerated::read(const U8 *in, FRD *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        out++;
    }
    return bytes;
} // FRD

//unsigned MsWordGenerated::read(const U8 *in, CHPXFKP *out, unsigned count)
//{
//    unsigned long shiftRegister;
//    U8 *ptr = (U8 *)out;
//    unsigned bytes = 0;
//
//    for (unsigned i = 0; i < count; i++)
//    {
//        bytes += read(in + bytes, (U8 *)(ptr + bytes), 1);
//        out++;
//    }
//    return bytes;
//} // CHPXFKP

//unsigned MsWordGenerated::read(const U8 *in, PAPXFKP *out, unsigned count)
//{
//    unsigned long shiftRegister;
//    U8 *ptr = (U8 *)out;
//    unsigned bytes = 0;
//
//    for (unsigned i = 0; i < count; i++)
//    {
//        bytes += read(in + bytes, (FC *)(ptr + bytes), fkp.crun+1);
//        bytes += read(in + bytes, (BX *)(ptr + bytes), fkp.crun);
//        bytes += read(in + bytes, (U8 *)(ptr + bytes), 1);
//        out++;
//    }
//    return bytes;
//} // PAPXFKP

unsigned MsWordGenerated::read(const U8 *in, LVLF *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)&shiftRegister);
        out->jc = shiftRegister;
        shiftRegister >>= 2;
        out->fLegal = shiftRegister;
        shiftRegister >>= 1;
        out->fNoRestart = shiftRegister;
        shiftRegister >>= 1;
        out->fPrev = shiftRegister;
        shiftRegister >>= 1;
        out->fPrevSpace = shiftRegister;
        shiftRegister >>= 1;
        out->fWord6 = shiftRegister;
        shiftRegister >>= 1;
        out->unused5_7 = shiftRegister;
        shiftRegister >>= 1;
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 10);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        out++;
    }
    return bytes;
} // LVLF

unsigned MsWordGenerated::read(const U8 *in, LSPD *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
        out++;
    }
    return bytes;
} // LSPD

unsigned MsWordGenerated::read(const U8 *in, LSTF *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 9);
        bytes += read(in + bytes, (U8 *)&shiftRegister);
        out->fSimpleList = shiftRegister;
        shiftRegister >>= 1;
        out->fRestartHdn = shiftRegister;
        shiftRegister >>= 1;
        out->unsigned26_2 = shiftRegister;
        shiftRegister >>= 6;
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 1);
        out++;
    }
    return bytes;
} // LSTF

unsigned MsWordGenerated::read(const U8 *in, LFO *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 3);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 4);
        out++;
    }
    return bytes;
} // LFO

unsigned MsWordGenerated::read(const U8 *in, LFOLVL *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)&shiftRegister);
        out->ilvl = shiftRegister;
        shiftRegister >>= 4;
        out->fStartAt = shiftRegister;
        shiftRegister >>= 1;
        out->fFormatting = shiftRegister;
        shiftRegister >>= 1;
        out->unsigned4_6 = shiftRegister;
        shiftRegister >>= 2;
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 3);
        out++;
    }
    return bytes;
} // LFOLVL

unsigned MsWordGenerated::read(const U8 *in, OLST *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (ANLV *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 4);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 32);
        out++;
    }
    return bytes;
} // OLST

unsigned MsWordGenerated::read(const U8 *in, NUMRM *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

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

unsigned MsWordGenerated::read(const U8 *in, PGD *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->fContinue = shiftRegister;
        shiftRegister >>= 1;
        out->fUnk = shiftRegister;
        shiftRegister >>= 1;
        out->fRight = shiftRegister;
        shiftRegister >>= 1;
        out->fPgnRestart = shiftRegister;
        shiftRegister >>= 1;
        out->fEmptyPage = shiftRegister;
        shiftRegister >>= 1;
        out->fAllFtn = shiftRegister;
        shiftRegister >>= 1;
        out->unused0_6 = shiftRegister;
        shiftRegister >>= 1;
        out->fTableBreaks = shiftRegister;
        shiftRegister >>= 1;
        out->fMarked = shiftRegister;
        shiftRegister >>= 1;
        out->fColumnBreaks = shiftRegister;
        shiftRegister >>= 1;
        out->fTableHeader = shiftRegister;
        shiftRegister >>= 1;
        out->fNewPage = shiftRegister;
        shiftRegister >>= 1;
        out->bkc = shiftRegister;
        shiftRegister >>= 4;
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        out++;
    }
    return bytes;
} // PGD

unsigned MsWordGenerated::read(const U8 *in, PHE *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->fSpare = shiftRegister;
        shiftRegister >>= 1;
        out->fUnk = shiftRegister;
        shiftRegister >>= 1;
        out->fDiffLines = shiftRegister;
        shiftRegister >>= 1;
        out->unused0_3 = shiftRegister;
        shiftRegister >>= 5;
        out->clMac = shiftRegister;
        shiftRegister >>= 8;
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 2);
        out++;
    }
    return bytes;
} // PHE

unsigned MsWordGenerated::read(const U8 *in, PAP *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 4);
        bytes += read(in + bytes, (U8 *)&shiftRegister);
        out->fBrLnAbove = shiftRegister;
        shiftRegister >>= 1;
        out->fBrLnBelow = shiftRegister;
        shiftRegister >>= 1;
        out->fUnused = shiftRegister;
        shiftRegister >>= 2;
        out->pcVert = shiftRegister;
        shiftRegister >>= 2;
        out->pcHorz = shiftRegister;
        shiftRegister >>= 2;
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 5);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 6);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 3);
        bytes += read(in + bytes, (LSPD *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (PHE *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 10);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->fVertical = shiftRegister;
        shiftRegister >>= 1;
        out->fBackward = shiftRegister;
        shiftRegister >>= 1;
        out->fRotateFont = shiftRegister;
        shiftRegister >>= 1;
        out->unused68_3 = shiftRegister;
        shiftRegister >>= 13;
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 4);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 4);
        bytes += read(in + bytes, (BRC *)(ptr + bytes), 6);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->dyaHeight = shiftRegister;
        shiftRegister >>= 15;
        out->fMinHeight = shiftRegister;
        shiftRegister >>= 1;
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

//unsigned MsWordGenerated::read(const U8 *in, PAPX *out, unsigned count)
//{
//    unsigned long shiftRegister;
//    U8 *ptr = (U8 *)out;
//    unsigned bytes = 0;
//
//    for (unsigned i = 0; i < count; i++)
//    {
//        bytes += read(in + bytes, (U8 *)(ptr + bytes), 3);
//        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
//        bytes += read(in + bytes, (character array *)(ptr + bytes), 1);
//        out++;
//    }
//    return bytes;
//} // PAPX

unsigned MsWordGenerated::read(const U8 *in, PICF *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 5);
        out++;
    }
    return bytes;
} // PICF

unsigned MsWordGenerated::read(const U8 *in, PCD *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->fNoParaLast = shiftRegister;
        shiftRegister >>= 1;
        out->fPaphNil = shiftRegister;
        shiftRegister >>= 1;
        out->fCopied = shiftRegister;
        shiftRegister >>= 1;
        out->unused0_3 = shiftRegister;
        shiftRegister >>= 5;
        out->fn = shiftRegister;
        shiftRegister >>= 8;
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (PRM *)(ptr + bytes), 1);
        out++;
    }
    return bytes;
} // PCD

//unsigned MsWordGenerated::read(const U8 *in, PLCF *out, unsigned count)
//{
//    unsigned long shiftRegister;
//    U8 *ptr = (U8 *)out;
//    unsigned bytes = 0;
//
//    for (unsigned i = 0; i < count; i++)
//    {
//        out++;
//    }
//    return bytes;
//} // PLCF

unsigned MsWordGenerated::read(const U8 *in, PRM2 *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->fComplex = shiftRegister;
        shiftRegister >>= 1;
        out->igrpprl = shiftRegister;
        shiftRegister >>= 15;
        out++;
    }
    return bytes;
} // PRM2

unsigned MsWordGenerated::read(const U8 *in, RS *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 8);
        out++;
    }
    return bytes;
} // RS

unsigned MsWordGenerated::read(const U8 *in, RR *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
        out++;
    }
    return bytes;
} // RR

unsigned MsWordGenerated::read(const U8 *in, SED *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

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

unsigned MsWordGenerated::read(const U8 *in, SEP *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

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
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->pgbApplyTo = shiftRegister;
        shiftRegister >>= 3;
        out->pgbPageDepth = shiftRegister;
        shiftRegister >>= 2;
        out->pgbOffsetFrom = shiftRegister;
        shiftRegister >>= 3;
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

//unsigned MsWordGenerated::read(const U8 *in, SEPX *out, unsigned count)
//{
//    unsigned long shiftRegister;
//    U8 *ptr = (U8 *)out;
//    unsigned bytes = 0;
//
//    for (unsigned i = 0; i < count; i++)
//    {
//        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
//        out++;
//    }
//    return bytes;
//} // SEPX

unsigned MsWordGenerated::read(const U8 *in, STSHI *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->fStdStylenamesWritten = shiftRegister;
        shiftRegister >>= 1;
        out->unused4_2 = shiftRegister;
        shiftRegister >>= 15;
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 6);
        out++;
    }
    return bytes;
} // STSHI

//unsigned MsWordGenerated::read(const U8 *in, STD *out, unsigned count)
//{
//    unsigned long shiftRegister;
//    U8 *ptr = (U8 *)out;
//    unsigned bytes = 0;
//
//    for (unsigned i = 0; i < count; i++)
//    {
//        bytes += read(in + bytes, (U16 *)&shiftRegister);
//        out->sti = shiftRegister;
//        shiftRegister >>= 12;
//        out->fScratch = shiftRegister;
//        shiftRegister >>= 1;
//        out->fInvalHeight = shiftRegister;
//        shiftRegister >>= 1;
//        out->fHasUpe = shiftRegister;
//        shiftRegister >>= 1;
//        out->fMassCopy = shiftRegister;
//        shiftRegister >>= 1;
//        bytes += read(in + bytes, (U16 *)&shiftRegister);
//        out->sgc = shiftRegister;
//        shiftRegister >>= 4;
//        out->istdBase = shiftRegister;
//        shiftRegister >>= 12;
//        bytes += read(in + bytes, (U16 *)&shiftRegister);
//        out->cupx = shiftRegister;
//        shiftRegister >>= 4;
//        out->istdNext = shiftRegister;
//        shiftRegister >>= 12;
//        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
//        bytes += read(in + bytes, (U16 *)&shiftRegister);
//        out->fAutoRedef = shiftRegister;
//        shiftRegister >>= 1;
//        out->fHidden = shiftRegister;
//        shiftRegister >>= 1;
//        out->unused8_3 = shiftRegister;
//        shiftRegister >>= 14;
//        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
//        out++;
//    }
//    return bytes;
//} // STD

unsigned MsWordGenerated::read(const U8 *in, TBD *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U8 *)&shiftRegister);
        out->jc = shiftRegister;
        shiftRegister >>= 3;
        out->tlc = shiftRegister;
        shiftRegister >>= 3;
        out->unused0_6 = shiftRegister;
        shiftRegister >>= 2;
        out++;
    }
    return bytes;
} // TBD

unsigned MsWordGenerated::read(const U8 *in, TC *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->fFirstMerged = shiftRegister;
        shiftRegister >>= 1;
        out->fMerged = shiftRegister;
        shiftRegister >>= 1;
        out->fVertical = shiftRegister;
        shiftRegister >>= 1;
        out->fBackward = shiftRegister;
        shiftRegister >>= 1;
        out->fRotateFont = shiftRegister;
        shiftRegister >>= 1;
        out->fVertMerge = shiftRegister;
        shiftRegister >>= 1;
        out->fVertRestart = shiftRegister;
        shiftRegister >>= 1;
        out->vertAlign = shiftRegister;
        shiftRegister >>= 2;
        out->fUnused = shiftRegister;
        shiftRegister >>= 7;
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (BRC *)(ptr + bytes), 4);
        out++;
    }
    return bytes;
} // TC

unsigned MsWordGenerated::read(const U8 *in, TLP *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->fBorders = shiftRegister;
        shiftRegister >>= 1;
        out->fShading = shiftRegister;
        shiftRegister >>= 1;
        out->fFont = shiftRegister;
        shiftRegister >>= 1;
        out->fColor = shiftRegister;
        shiftRegister >>= 1;
        out->fBestFit = shiftRegister;
        shiftRegister >>= 1;
        out->fHdrRows = shiftRegister;
        shiftRegister >>= 1;
        out->fLastRow = shiftRegister;
        shiftRegister >>= 1;
        out->fHdrCols = shiftRegister;
        shiftRegister >>= 1;
        out->fLastCol = shiftRegister;
        shiftRegister >>= 1;
        out++;
    }
    return bytes;
} // TLP

unsigned MsWordGenerated::read(const U8 *in, TAP *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (TLP *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->fCaFull = shiftRegister;
        shiftRegister >>= 1;
        out->fFirstRow = shiftRegister;
        shiftRegister >>= 1;
        out->fLastRow = shiftRegister;
        shiftRegister >>= 1;
        out->fOutline = shiftRegister;
        shiftRegister >>= 1;
        out->unused20_12 = shiftRegister;
        shiftRegister >>= 12;
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

unsigned MsWordGenerated::read(const U8 *in, FTXBXS *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 3);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 2);
        out++;
    }
    return bytes;
} // FTXBXS

unsigned MsWordGenerated::read(const U8 *in, WKB *out, unsigned count)
{
    unsigned long shiftRegister;
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 3);
        bytes += read(in + bytes, (U16 *)&shiftRegister);
        out->fnpt = shiftRegister;
        shiftRegister >>= 4;
        out->fnpd = shiftRegister;
        shiftRegister >>= 12;
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        out++;
    }
    return bytes;
} // WKB

