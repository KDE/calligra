/*
    Copyright (C) 2000, 2002, S.R.Haque <srhaque@iee.org>.
    This file is part of the KDE project.

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

    This file is a description of most structures used in the on-disk format
    of Microsoft Word documents. The only structures missing are those
    which are tricky to autogenerate.
*/

#include <mswordgenerated.h>
#include <string.h>

unsigned MsWordGenerated::read(const U8 *in, U8 *out)
{
    *out = *in;
    return 1;
}

unsigned MsWordGenerated::read(const U8 *in, S8 *out)
{
    return read(in, (U8 *)out);
}

unsigned MsWordGenerated::read(const U8 *in, U16 *out)
{
    U16 tmp;

    tmp = (U16)((*(in + 1) << 8) + (*in));
    *out = tmp;
    return 2;
}

unsigned MsWordGenerated::read(const U8 *in, S16 *out)
{
    return read(in, (U16 *)out);
}

unsigned MsWordGenerated::read(const U8 *in, U32 *out)
{
    U32 tmp;

    tmp = (U32)((*(in + 3) << 24) + (*(in + 2) << 16) + (*(in + 1) << 8) + (*in));
    *out = tmp;
    return 4;
}

unsigned MsWordGenerated::read(const U8 *in, S32 *out)
{
    return read(in, (U32 *)out);
}
unsigned MsWordGenerated::read(const U8 *in, BRC *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
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
    return bytes;
} // BRC

unsigned MsWordGenerated::read(const U8 *in, DOPTYPOGRAPHY *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
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
    bytes += read(in + bytes, &out->cchFollowingPunct);
    bytes += read(in + bytes, &out->cchLeadingPunct);
    for (i = 0; i < 101; i++)
        bytes += read(in + bytes, &out->rgxchFPunct[i]);
    for (i = 0; i < 51; i++)
        bytes += read(in + bytes, &out->rgxchLPunct[i]);
    return bytes;
} // DOPTYPOGRAPHY

unsigned MsWordGenerated::read(const U8 *in, DTTM *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
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
    return bytes;
} // DTTM

unsigned MsWordGenerated::read(const U8 *in, PHE *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
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
    bytes += read(in + bytes, &out->unused2);
    bytes += read(in + bytes, &out->dxaCol);
    bytes += read(in + bytes, &out->dym);
    return bytes;
} // PHE

unsigned MsWordGenerated::read(const U8 *in, PRM *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &shifterU16);
    out->fComplex = shifterU16;
    shifterU16 >>= 1;
    out->isprm = shifterU16;
    shifterU16 >>= 7;
    out->val = shifterU16;
    shifterU16 >>= 8;
    return bytes;
} // PRM

unsigned MsWordGenerated::read(const U8 *in, SHD *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &shifterU16);
    out->icoFore = shifterU16;
    shifterU16 >>= 5;
    out->icoBack = shifterU16;
    shifterU16 >>= 5;
    out->ipat = shifterU16;
    shifterU16 >>= 6;
    return bytes;
} // SHD

unsigned MsWordGenerated::read(const U8 *in, TC *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
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
    bytes += read(in + bytes, &out->wUnused);
    bytes += read(in + bytes, &out->brcTop);
    bytes += read(in + bytes, &out->brcLeft);
    bytes += read(in + bytes, &out->brcBottom);
    bytes += read(in + bytes, &out->brcRight);
    return bytes;
} // TC

unsigned MsWordGenerated::read(const U8 *in, TLP *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &out->itl);
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
    out->unused2_9 = shifterU16;
    shifterU16 >>= 7;
    return bytes;
} // TLP

unsigned MsWordGenerated::read(const U8 *in, ANLD *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &out->nfc);
    bytes += read(in + bytes, &out->cxchTextBefore);
    bytes += read(in + bytes, &out->cxchTextAfter);
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
    bytes += read(in + bytes, &out->ftc);
    bytes += read(in + bytes, &out->hps);
    bytes += read(in + bytes, &out->iStartAt);
    bytes += read(in + bytes, &out->dxaIndent);
    bytes += read(in + bytes, &out->dxaSpace);
    bytes += read(in + bytes, &out->fNumber1);
    bytes += read(in + bytes, &out->fNumberAcross);
    bytes += read(in + bytes, &out->fRestartHdn);
    bytes += read(in + bytes, &out->fSpareX);
    for (i = 0; i < 32; i++)
        bytes += read(in + bytes, &out->rgxch[i]);
    return bytes;
} // ANLD

unsigned MsWordGenerated::read(const U8 *in, ANLV *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &out->nfc);
    bytes += read(in + bytes, &out->cxchTextBefore);
    bytes += read(in + bytes, &out->cxchTextAfter);
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
    bytes += read(in + bytes, &out->ftc);
    bytes += read(in + bytes, &out->hps);
    bytes += read(in + bytes, &out->iStartAt);
    bytes += read(in + bytes, &out->dxaIndent);
    bytes += read(in + bytes, &out->dxaSpace);
    return bytes;
} // ANLV

unsigned MsWordGenerated::read(const U8 *in, ASUMY *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &out->lLevel);
    return bytes;
} // ASUMY

unsigned MsWordGenerated::read(const U8 *in, ASUMYI *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
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
    bytes += read(in + bytes, &out->wDlgLevel);
    bytes += read(in + bytes, &out->lHighestLevel);
    bytes += read(in + bytes, &out->lCurrentLevel);
    return bytes;
} // ASUMYI

unsigned MsWordGenerated::read(const U8 *in, ATRD *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    for (i = 0; i < 10; i++)
        bytes += read(in + bytes, &out->xstUsrInitl[i]);
    bytes += read(in + bytes, &out->ibst);
    bytes += read(in + bytes, &shifterU16);
    out->ak = shifterU16;
    shifterU16 >>= 2;
    out->unused22_2 = shifterU16;
    shifterU16 >>= 14;
    bytes += read(in + bytes, &out->grfbmc);
    bytes += read(in + bytes, &out->lTagBkmk);
    return bytes;
} // ATRD

unsigned MsWordGenerated::read(const U8 *in, BKD *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &out->ipgd);
    bytes += read(in + bytes, &out->dcpDepend);
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
    out->unused4_13 = shifterU16;
    shifterU16 >>= 3;
    return bytes;
} // BKD

unsigned MsWordGenerated::read(const U8 *in, BKF *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &out->ibkl);
    bytes += read(in + bytes, &shifterU16);
    out->itcFirst = shifterU16;
    shifterU16 >>= 7;
    out->fPub = shifterU16;
    shifterU16 >>= 1;
    out->itcLim = shifterU16;
    shifterU16 >>= 7;
    out->fCol = shifterU16;
    shifterU16 >>= 1;
    return bytes;
} // BKF

unsigned MsWordGenerated::read(const U8 *in, BKL *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &out->ibkf);
    return bytes;
} // BKL

unsigned MsWordGenerated::read(const U8 *in, BRC10 *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
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
    return bytes;
} // BRC10

unsigned MsWordGenerated::read(const U8 *in, BTE *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &out->pn);
    return bytes;
} // BTE

unsigned MsWordGenerated::read(const U8 *in, CHP *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
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
    bytes += read(in + bytes, &out->unused4);
    bytes += read(in + bytes, &out->ftc);
    bytes += read(in + bytes, &out->ftcAscii);
    bytes += read(in + bytes, &out->ftcFE);
    bytes += read(in + bytes, &out->ftcOther);
    bytes += read(in + bytes, &out->hps);
    bytes += read(in + bytes, &out->dxaSpace);
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
    out->hpScript = shifterU8;
    shifterU8 >>= 1;
    bytes += read(in + bytes, &out->hpsPos);
    bytes += read(in + bytes, &out->lid);
    bytes += read(in + bytes, &out->lidDefault);
    bytes += read(in + bytes, &out->lidFE);
    bytes += read(in + bytes, &out->idct);
    bytes += read(in + bytes, &out->idctHint);
    bytes += read(in + bytes, &out->wCharScale);
    bytes += read(in + bytes, &out->fcPic_fcObj_lTagObj);
    bytes += read(in + bytes, &out->ibstRMark);
    bytes += read(in + bytes, &out->ibstRMarkDel);
    bytes += read(in + bytes, &out->dttmRMark);
    bytes += read(in + bytes, &out->dttmRMarkDel);
    bytes += read(in + bytes, &out->unused52);
    bytes += read(in + bytes, &out->istd);
    bytes += read(in + bytes, &out->ftcSym);
    bytes += read(in + bytes, &out->xchSym);
    bytes += read(in + bytes, &out->idslRMReason);
    bytes += read(in + bytes, &out->idslReasonDel);
    bytes += read(in + bytes, &out->ysr);
    bytes += read(in + bytes, &out->chYsr);
    bytes += read(in + bytes, &out->cpg);
    bytes += read(in + bytes, &out->hpsKern);
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
    out->reserved_3 = shifterU16;
    shifterU16 >>= 3;
    bytes += read(in + bytes, &out->fPropMark);
    bytes += read(in + bytes, &out->ibstPropRMark);
    bytes += read(in + bytes, &out->dttmPropRMark);
    bytes += read(in + bytes, &out->sfxtText);
    bytes += read(in + bytes, &out->unused81);
    bytes += read(in + bytes, &out->unused82);
    bytes += read(in + bytes, &out->unused83);
    bytes += read(in + bytes, &out->unused85);
    bytes += read(in + bytes, &out->unused87);
    bytes += read(in + bytes, &out->fDispFldRMark);
    bytes += read(in + bytes, &out->ibstDispFldRMark);
    bytes += read(in + bytes, &out->dttmDispFldRMark);
    for (i = 0; i < 16; i++)
        bytes += read(in + bytes, &out->xstDispFldRMark[i]);
    bytes += read(in + bytes, &out->shd);
    bytes += read(in + bytes, &out->brc);
    return bytes;
} // CHP

//unsigned MsWordGenerated::read(const U8 *in, CHPX *out)
//{
//    U32 shifterU32;
//    U16 shifterU16;
//    U8 shifterU8;
//    unsigned bytes;
//    int i;
//
//    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
//    bytes += read(in + bytes, &out->cb);
//    bytes += read(in + bytes, &out->grpprl);
//    return bytes;
//} // CHPX

//unsigned MsWordGenerated::read(const U8 *in, CHPXFKP *out)
//{
//    U32 shifterU32;
//    U16 shifterU16;
//    U8 shifterU8;
//    unsigned bytes;
//    int i;
//
//    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
//    for (i = 0; i < 0; i++)
//        bytes += read(in + bytes, &out->rgfc[i]);
//    for (i = 0; i < 0; i++)
//        bytes += read(in + bytes, &out->rgb[i]);
//    for (i = 0; i < 0; i++)
//        bytes += read(in + bytes, &out->unusedSpace[i]);
//    for (i = 0; i < 0; i++)
//        bytes += read(in + bytes, &out->grpchpx[i]);
//    bytes += read(in + bytes, &out->crun);
//    return bytes;
//} // CHPXFKP

unsigned MsWordGenerated::read(const U8 *in, DCS *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &shifterU8);
    out->fdct = shifterU8;
    shifterU8 >>= 3;
    out->lines = shifterU8;
    shifterU8 >>= 5;
    bytes += read(in + bytes, &out->unused1);
    return bytes;
} // DCS

unsigned MsWordGenerated::read(const U8 *in, DOGRID *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &out->xaGrid);
    bytes += read(in + bytes, &out->yaGrid);
    bytes += read(in + bytes, &out->dxaGrid);
    bytes += read(in + bytes, &out->dyaGrid);
    bytes += read(in + bytes, &shifterU16);
    out->dyGridDisplay = shifterU16;
    shifterU16 >>= 7;
    out->fTurnItOff = shifterU16;
    shifterU16 >>= 1;
    out->dxGridDisplay = shifterU16;
    shifterU16 >>= 7;
    out->fFollowMargins = shifterU16;
    shifterU16 >>= 1;
    return bytes;
} // DOGRID

unsigned MsWordGenerated::read(const U8 *in, DOP *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
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
    out->unused6_6 = shifterU8;
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
    out->unused7_5 = shifterU8;
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
    bytes += read(in + bytes, &out->dxaTab);
    bytes += read(in + bytes, &out->wSpare);
    bytes += read(in + bytes, &out->dxaHotZ);
    bytes += read(in + bytes, &out->cConsecHypLim);
    bytes += read(in + bytes, &out->wSpare2);
    bytes += read(in + bytes, &out->dttmCreated);
    bytes += read(in + bytes, &out->dttmRevised);
    bytes += read(in + bytes, &out->dttmLastPrint);
    bytes += read(in + bytes, &out->nRevision);
    bytes += read(in + bytes, &out->tmEdited);
    bytes += read(in + bytes, &out->cWords);
    bytes += read(in + bytes, &out->cCh);
    bytes += read(in + bytes, &out->cPg);
    bytes += read(in + bytes, &out->cParas);
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
    bytes += read(in + bytes, &out->cLines);
    bytes += read(in + bytes, &out->cWordsFtnEnd);
    bytes += read(in + bytes, &out->cChFtnEdn);
    bytes += read(in + bytes, &out->cPgFtnEdn);
    bytes += read(in + bytes, &out->cParasFtnEdn);
    bytes += read(in + bytes, &out->cLinesFtnEdn);
    bytes += read(in + bytes, &out->lKeyProtDoc);
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
    bytes += read(in + bytes, &out->adt);
    bytes += read(in + bytes, &out->doptypography);
    bytes += read(in + bytes, &out->dogrid);
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
    bytes += read(in + bytes, &out->asumyi);
    bytes += read(in + bytes, &out->cChWS);
    bytes += read(in + bytes, &out->cChWSFtnEdn);
    bytes += read(in + bytes, &out->grfDocEvents);
    bytes += read(in + bytes, &shifterU32);
    out->fVirusPrompted = shifterU32;
    shifterU32 >>= 1;
    out->fVirusLoadSafe = shifterU32;
    shifterU32 >>= 1;
    out->KeyVirusSession30 = shifterU32;
    shifterU32 >>= 30;
    for (i = 0; i < 30; i++)
        bytes += read(in + bytes, &out->Spare[i]);
    bytes += read(in + bytes, &out->unused472);
    bytes += read(in + bytes, &out->unused476);
    bytes += read(in + bytes, &out->cDBC);
    bytes += read(in + bytes, &out->cDBCFtnEdn);
    bytes += read(in + bytes, &out->unused488);
    bytes += read(in + bytes, &out->nfcFtnRef2);
    bytes += read(in + bytes, &out->nfcEdnRef2);
    bytes += read(in + bytes, &out->hpsZoonFontPag);
    bytes += read(in + bytes, &out->dywDispPag);
    return bytes;
} // DOP

//unsigned MsWordGenerated::read(const U8 *in, FFN *out)
//{
//    U32 shifterU32;
//    U16 shifterU16;
//    U8 shifterU8;
//    unsigned bytes;
//    int i;
//
//    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
//    bytes += read(in + bytes, &out->cbFfnM1);
//    bytes += read(in + bytes, &shifterU8);
//    out->prq = shifterU8;
//    shifterU8 >>= 2;
//    out->fTrueType = shifterU8;
//    shifterU8 >>= 1;
//    out->unused1_3 = shifterU8;
//    shifterU8 >>= 1;
//    out->ff = shifterU8;
//    shifterU8 >>= 3;
//    out->unused1_7 = shifterU8;
//    shifterU8 >>= 1;
//    bytes += read(in + bytes, &out->wWeight);
//    bytes += read(in + bytes, &out->chs);
//    bytes += read(in + bytes, &out->ixchSzAlt);
//    for (i = 0; i < 10; i++)
//        bytes += read(in + bytes, &out->panose[i]);
//    for (i = 0; i < 24; i++)
//        bytes += read(in + bytes, &out->fs[i]);
//    for (i = 0; i < 0; i++)
//        bytes += read(in + bytes, &out->xszFfn[i]);
//    return bytes;
//} // FFN

unsigned MsWordGenerated::read(const U8 *in, FIB *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &out->wIdent);
    bytes += read(in + bytes, &out->nFib);
    bytes += read(in + bytes, &out->nProduct);
    bytes += read(in + bytes, &out->lid);
    bytes += read(in + bytes, &out->pnNext);
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
    bytes += read(in + bytes, &out->nFibBack);
    bytes += read(in + bytes, &out->lKey);
    bytes += read(in + bytes, &out->envr);
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
    bytes += read(in + bytes, &out->chs);
    bytes += read(in + bytes, &out->chsTables);
    bytes += read(in + bytes, &out->fcMin);
    bytes += read(in + bytes, &out->fcMac);
    bytes += read(in + bytes, &out->csw);
    bytes += read(in + bytes, &out->wMagicCreated);
    bytes += read(in + bytes, &out->wMagicRevised);
    bytes += read(in + bytes, &out->wMagicCreatedPrivate);
    bytes += read(in + bytes, &out->wMagicRevisedPrivate);
    bytes += read(in + bytes, &out->pnFbpChpFirst_W6);
    bytes += read(in + bytes, &out->pnChpFirst_W6);
    bytes += read(in + bytes, &out->cpnBteChp_W6);
    bytes += read(in + bytes, &out->pnFbpPapFirst_W6);
    bytes += read(in + bytes, &out->pnPapFirst_W6);
    bytes += read(in + bytes, &out->cpnBtePap_W6);
    bytes += read(in + bytes, &out->pnFbpLvcFirst_W6);
    bytes += read(in + bytes, &out->pnLvcFirst_W6);
    bytes += read(in + bytes, &out->cpnBteLvc_W6);
    bytes += read(in + bytes, &out->lidFE);
    bytes += read(in + bytes, &out->clw);
    bytes += read(in + bytes, &out->cbMac);
    bytes += read(in + bytes, &out->lProductCreated);
    bytes += read(in + bytes, &out->lProductRevised);
    bytes += read(in + bytes, &out->ccpText);
    bytes += read(in + bytes, &out->ccpFtn);
    bytes += read(in + bytes, &out->ccpHdd);
    bytes += read(in + bytes, &out->ccpMcr);
    bytes += read(in + bytes, &out->ccpAtn);
    bytes += read(in + bytes, &out->ccpEdn);
    bytes += read(in + bytes, &out->ccpTxbx);
    bytes += read(in + bytes, &out->ccpHdrTxbx);
    bytes += read(in + bytes, &out->pnFbpChpFirst);
    bytes += read(in + bytes, &out->pnChpFirst);
    bytes += read(in + bytes, &out->cpnBteChp);
    bytes += read(in + bytes, &out->pnFbpPapFirst);
    bytes += read(in + bytes, &out->pnPapFirst);
    bytes += read(in + bytes, &out->cpnBtePap);
    bytes += read(in + bytes, &out->pnFbpLvcFirst);
    bytes += read(in + bytes, &out->pnLvcFirst);
    bytes += read(in + bytes, &out->cpnBteLvc);
    bytes += read(in + bytes, &out->fcIslandFirst);
    bytes += read(in + bytes, &out->fcIslandLim);
    bytes += read(in + bytes, &out->cfclcb);
    bytes += read(in + bytes, &out->fcStshfOrig);
    bytes += read(in + bytes, &out->lcbStshfOrig);
    bytes += read(in + bytes, &out->fcStshf);
    bytes += read(in + bytes, &out->lcbStshf);
    bytes += read(in + bytes, &out->fcPlcffndRef);
    bytes += read(in + bytes, &out->lcbPlcffndRef);
    bytes += read(in + bytes, &out->fcPlcffndTxt);
    bytes += read(in + bytes, &out->lcbPlcffndTxt);
    bytes += read(in + bytes, &out->fcPlcfandRef);
    bytes += read(in + bytes, &out->lcbPlcfandRef);
    bytes += read(in + bytes, &out->fcPlcfandTxt);
    bytes += read(in + bytes, &out->lcbPlcfandTxt);
    bytes += read(in + bytes, &out->fcPlcfsed);
    bytes += read(in + bytes, &out->lcbPlcfsed);
    bytes += read(in + bytes, &out->fcPlcfpad);
    bytes += read(in + bytes, &out->lcbPlcfpad);
    bytes += read(in + bytes, &out->fcPlcfphe);
    bytes += read(in + bytes, &out->lcbPlcfphe);
    bytes += read(in + bytes, &out->fcSttbfglsy);
    bytes += read(in + bytes, &out->lcbSttbfglsy);
    bytes += read(in + bytes, &out->fcPlcfglsy);
    bytes += read(in + bytes, &out->lcbPlcfglsy);
    bytes += read(in + bytes, &out->fcPlcfhdd);
    bytes += read(in + bytes, &out->lcbPlcfhdd);
    bytes += read(in + bytes, &out->fcPlcfbteChpx);
    bytes += read(in + bytes, &out->lcbPlcfbteChpx);
    bytes += read(in + bytes, &out->fcPlcfbtePapx);
    bytes += read(in + bytes, &out->lcbPlcfbtePapx);
    bytes += read(in + bytes, &out->fcPlcfsea);
    bytes += read(in + bytes, &out->lcbPlcfsea);
    bytes += read(in + bytes, &out->fcSttbfffn);
    bytes += read(in + bytes, &out->lcbSttbfffn);
    bytes += read(in + bytes, &out->fcPlcffldMom);
    bytes += read(in + bytes, &out->lcbPlcffldMom);
    bytes += read(in + bytes, &out->fcPlcffldHdr);
    bytes += read(in + bytes, &out->lcbPlcffldHdr);
    bytes += read(in + bytes, &out->fcPlcffldFtn);
    bytes += read(in + bytes, &out->lcbPlcffldFtn);
    bytes += read(in + bytes, &out->fcPlcffldAtn);
    bytes += read(in + bytes, &out->lcbPlcffldAtn);
    bytes += read(in + bytes, &out->fcPlcffldMcr);
    bytes += read(in + bytes, &out->lcbPlcffldMcr);
    bytes += read(in + bytes, &out->fcSttbfbkmk);
    bytes += read(in + bytes, &out->lcbSttbfbkmk);
    bytes += read(in + bytes, &out->fcPlcfbkf);
    bytes += read(in + bytes, &out->lcbPlcfbkf);
    bytes += read(in + bytes, &out->fcPlcfbkl);
    bytes += read(in + bytes, &out->lcbPlcfbkl);
    bytes += read(in + bytes, &out->fcCmds);
    bytes += read(in + bytes, &out->lcbCmds);
    bytes += read(in + bytes, &out->fcPlcmcr);
    bytes += read(in + bytes, &out->lcbPlcmcr);
    bytes += read(in + bytes, &out->fcSttbfmcr);
    bytes += read(in + bytes, &out->lcbSttbfmcr);
    bytes += read(in + bytes, &out->fcPrDrvr);
    bytes += read(in + bytes, &out->lcbPrDrvr);
    bytes += read(in + bytes, &out->fcPrEnvPort);
    bytes += read(in + bytes, &out->lcbPrEnvPort);
    bytes += read(in + bytes, &out->fcPrEnvLand);
    bytes += read(in + bytes, &out->lcbPrEnvLand);
    bytes += read(in + bytes, &out->fcWss);
    bytes += read(in + bytes, &out->lcbWss);
    bytes += read(in + bytes, &out->fcDop);
    bytes += read(in + bytes, &out->lcbDop);
    bytes += read(in + bytes, &out->fcSttbfAssoc);
    bytes += read(in + bytes, &out->lcbSttbfAssoc);
    bytes += read(in + bytes, &out->fcClx);
    bytes += read(in + bytes, &out->lcbClx);
    bytes += read(in + bytes, &out->fcPlcfpgdFtn);
    bytes += read(in + bytes, &out->lcbPlcfpgdFtn);
    bytes += read(in + bytes, &out->fcAutosaveSource);
    bytes += read(in + bytes, &out->lcbAutosaveSource);
    bytes += read(in + bytes, &out->fcGrpXstAtnOwners);
    bytes += read(in + bytes, &out->lcbGrpXstAtnOwners);
    bytes += read(in + bytes, &out->fcSttbfAtnbkmk);
    bytes += read(in + bytes, &out->lcbSttbfAtnbkmk);
    bytes += read(in + bytes, &out->fcPlcdoaMom);
    bytes += read(in + bytes, &out->lcbPlcdoaMom);
    bytes += read(in + bytes, &out->fcPlcdoaHdr);
    bytes += read(in + bytes, &out->lcbPlcdoaHdr);
    bytes += read(in + bytes, &out->fcPlcspaMom);
    bytes += read(in + bytes, &out->lcbPlcspaMom);
    bytes += read(in + bytes, &out->fcPlcspaHdr);
    bytes += read(in + bytes, &out->lcbPlcspaHdr);
    bytes += read(in + bytes, &out->fcPlcfAtnbkf);
    bytes += read(in + bytes, &out->lcbPlcfAtnbkf);
    bytes += read(in + bytes, &out->fcPlcfAtnbkl);
    bytes += read(in + bytes, &out->lcbPlcfAtnbkl);
    bytes += read(in + bytes, &out->fcPms);
    bytes += read(in + bytes, &out->lcbPms);
    bytes += read(in + bytes, &out->fcFormFldSttbf);
    bytes += read(in + bytes, &out->lcbFormFldSttbf);
    bytes += read(in + bytes, &out->fcPlcfendRef);
    bytes += read(in + bytes, &out->lcbPlcfendRef);
    bytes += read(in + bytes, &out->fcPlcfendTxt);
    bytes += read(in + bytes, &out->lcbPlcfendTxt);
    bytes += read(in + bytes, &out->fcPlcffldEdn);
    bytes += read(in + bytes, &out->lcbPlcffldEdn);
    bytes += read(in + bytes, &out->fcPlcfpgdEdn);
    bytes += read(in + bytes, &out->lcbPlcfpgdEdn);
    bytes += read(in + bytes, &out->fcDggInfo);
    bytes += read(in + bytes, &out->lcbDggInfo);
    bytes += read(in + bytes, &out->fcSttbfRMark);
    bytes += read(in + bytes, &out->lcbSttbfRMark);
    bytes += read(in + bytes, &out->fcSttbfCaption);
    bytes += read(in + bytes, &out->lcbSttbfCaption);
    bytes += read(in + bytes, &out->fcSttbfAutoCaption);
    bytes += read(in + bytes, &out->lcbSttbfAutoCaption);
    bytes += read(in + bytes, &out->fcPlcfwkb);
    bytes += read(in + bytes, &out->lcbPlcfwkb);
    bytes += read(in + bytes, &out->fcPlcfspl);
    bytes += read(in + bytes, &out->lcbPlcfspl);
    bytes += read(in + bytes, &out->fcPlcftxbxTxt);
    bytes += read(in + bytes, &out->lcbPlcftxbxTxt);
    bytes += read(in + bytes, &out->fcPlcffldTxbx);
    bytes += read(in + bytes, &out->lcbPlcffldTxbx);
    bytes += read(in + bytes, &out->fcPlcfHdrtxbxTxt);
    bytes += read(in + bytes, &out->lcbPlcfHdrtxbxTxt);
    bytes += read(in + bytes, &out->fcPlcffldHdrTxbx);
    bytes += read(in + bytes, &out->lcbPlcffldHdrTxbx);
    bytes += read(in + bytes, &out->fcStwUser);
    bytes += read(in + bytes, &out->lcbStwUser);
    bytes += read(in + bytes, &out->fcSttbttmbd);
    bytes += read(in + bytes, &out->lcbSttbttmbd);
    bytes += read(in + bytes, &out->fcUnused);
    bytes += read(in + bytes, &out->lcbUnused);
    bytes += read(in + bytes, &out->fcPgdMother);
    bytes += read(in + bytes, &out->lcbPgdMother);
    bytes += read(in + bytes, &out->fcBkdMother);
    bytes += read(in + bytes, &out->lcbBkdMother);
    bytes += read(in + bytes, &out->fcPgdFtn);
    bytes += read(in + bytes, &out->lcbPgdFtn);
    bytes += read(in + bytes, &out->fcBkdFtn);
    bytes += read(in + bytes, &out->lcbBkdFtn);
    bytes += read(in + bytes, &out->fcPgdEdn);
    bytes += read(in + bytes, &out->lcbPgdEdn);
    bytes += read(in + bytes, &out->fcBkdEdn);
    bytes += read(in + bytes, &out->lcbBkdEdn);
    bytes += read(in + bytes, &out->fcSttbfIntlFld);
    bytes += read(in + bytes, &out->lcbSttbfIntlFld);
    bytes += read(in + bytes, &out->fcRouteSlip);
    bytes += read(in + bytes, &out->lcbRouteSlip);
    bytes += read(in + bytes, &out->fcSttbSavedBy);
    bytes += read(in + bytes, &out->lcbSttbSavedBy);
    bytes += read(in + bytes, &out->fcSttbFnm);
    bytes += read(in + bytes, &out->lcbSttbFnm);
    bytes += read(in + bytes, &out->fcPlcfLst);
    bytes += read(in + bytes, &out->lcbPlcfLst);
    bytes += read(in + bytes, &out->fcPlfLfo);
    bytes += read(in + bytes, &out->lcbPlfLfo);
    bytes += read(in + bytes, &out->fcPlcftxbxBkd);
    bytes += read(in + bytes, &out->lcbPlcftxbxBkd);
    bytes += read(in + bytes, &out->fcPlcftxbxHdrBkd);
    bytes += read(in + bytes, &out->lcbPlcftxbxHdrBkd);
    bytes += read(in + bytes, &out->fcDocUndo);
    bytes += read(in + bytes, &out->lcbDocUndo);
    bytes += read(in + bytes, &out->fcRgbuse);
    bytes += read(in + bytes, &out->lcbRgbuse);
    bytes += read(in + bytes, &out->fcUsp);
    bytes += read(in + bytes, &out->lcbUsp);
    bytes += read(in + bytes, &out->fcUskf);
    bytes += read(in + bytes, &out->lcbUskf);
    bytes += read(in + bytes, &out->fcPlcupcRgbuse);
    bytes += read(in + bytes, &out->lcbPlcupcRgbuse);
    bytes += read(in + bytes, &out->fcPlcupcUsp);
    bytes += read(in + bytes, &out->lcbPlcupcUsp);
    bytes += read(in + bytes, &out->fcSttbGlsyStyle);
    bytes += read(in + bytes, &out->lcbSttbGlsyStyle);
    bytes += read(in + bytes, &out->fcPlgosl);
    bytes += read(in + bytes, &out->lcbPlgosl);
    bytes += read(in + bytes, &out->fcPlcocx);
    bytes += read(in + bytes, &out->lcbPlcocx);
    bytes += read(in + bytes, &out->fcPlcfbteLvc);
    bytes += read(in + bytes, &out->lcbPlcfbteLvc);
    bytes += read(in + bytes, &out->dwLowDateTime);
    bytes += read(in + bytes, &out->dwHighDateTime);
    bytes += read(in + bytes, &out->fcPlcflvc);
    bytes += read(in + bytes, &out->lcbPlcflvc);
    bytes += read(in + bytes, &out->fcPlcasumy);
    bytes += read(in + bytes, &out->lcbPlcasumy);
    bytes += read(in + bytes, &out->fcPlcfgram);
    bytes += read(in + bytes, &out->lcbPlcfgram);
    bytes += read(in + bytes, &out->fcSttbListNames);
    bytes += read(in + bytes, &out->lcbSttbListNames);
    bytes += read(in + bytes, &out->fcSttbfUssr);
    bytes += read(in + bytes, &out->lcbSttbfUssr);
    return bytes;
} // FIB

unsigned MsWordGenerated::read(const U8 *in, FIBFCLCB *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &out->fc);
    bytes += read(in + bytes, &out->lcb);
    return bytes;
} // FIBFCLCB

unsigned MsWordGenerated::read(const U8 *in, FLD *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &shifterU8);
    out->ch = shifterU8;
    shifterU8 >>= 5;
    out->unused0_5 = shifterU8;
    shifterU8 >>= 3;
    bytes += read(in + bytes, &out->flt);
    return bytes;
} // FLD

unsigned MsWordGenerated::read(const U8 *in, FRD *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &out->nAuto);
    return bytes;
} // FRD

unsigned MsWordGenerated::read(const U8 *in, FSPA *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &out->spid);
    bytes += read(in + bytes, &out->xaLeft);
    bytes += read(in + bytes, &out->yaTop);
    bytes += read(in + bytes, &out->xaRight);
    bytes += read(in + bytes, &out->yaBottom);
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
    bytes += read(in + bytes, &out->cTxbx);
    return bytes;
} // FSPA

unsigned MsWordGenerated::read(const U8 *in, FTXBXS *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &out->cTxbx);
    bytes += read(in + bytes, &out->iNextReuse);
    bytes += read(in + bytes, &out->cReusable);
    bytes += read(in + bytes, &out->fReusable);
    bytes += read(in + bytes, &out->reserved);
    bytes += read(in + bytes, &out->lid);
    bytes += read(in + bytes, &out->txidUndo);
    return bytes;
} // FTXBXS

unsigned MsWordGenerated::read(const U8 *in, LFO *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &out->lsid);
    bytes += read(in + bytes, &out->unused4);
    bytes += read(in + bytes, &out->unused8);
    bytes += read(in + bytes, &out->clfolvl);
    for (i = 0; i < 3; i++)
        bytes += read(in + bytes, &out->reserved[i]);
    return bytes;
} // LFO

unsigned MsWordGenerated::read(const U8 *in, LFOLVL *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &out->iStartAt);
    bytes += read(in + bytes, &shifterU8);
    out->ilvl = shifterU8;
    shifterU8 >>= 4;
    out->fStartAt = shifterU8;
    shifterU8 >>= 1;
    out->fFormatting = shifterU8;
    shifterU8 >>= 1;
    out->unsigned4_6 = shifterU8;
    shifterU8 >>= 2;
    for (i = 0; i < 3; i++)
        bytes += read(in + bytes, &out->reserved[i]);
    return bytes;
} // LFOLVL

unsigned MsWordGenerated::read(const U8 *in, LSPD *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &out->dyaLine);
    bytes += read(in + bytes, &out->fMultLinespace);
    return bytes;
} // LSPD

unsigned MsWordGenerated::read(const U8 *in, LSTF *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &out->lsid);
    bytes += read(in + bytes, &out->tplc);
    for (i = 0; i < 9; i++)
        bytes += read(in + bytes, &out->rgistd[i]);
    bytes += read(in + bytes, &shifterU8);
    out->fSimpleList = shifterU8;
    shifterU8 >>= 1;
    out->fRestartHdn = shifterU8;
    shifterU8 >>= 1;
    out->unsigned26_2 = shifterU8;
    shifterU8 >>= 6;
    bytes += read(in + bytes, &out->reserved);
    return bytes;
} // LSTF

unsigned MsWordGenerated::read(const U8 *in, LVLF *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &out->iStartAt);
    bytes += read(in + bytes, &out->nfc);
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
    for (i = 0; i < 9; i++)
        bytes += read(in + bytes, &out->rgbxchNums[i]);
    bytes += read(in + bytes, &out->ixchFollow);
    bytes += read(in + bytes, &out->dxaSpace);
    bytes += read(in + bytes, &out->dxaIndent);
    bytes += read(in + bytes, &out->cbGrpprlChpx);
    bytes += read(in + bytes, &out->cbGrpprlPapx);
    bytes += read(in + bytes, &out->reserved);
    return bytes;
} // LVLF

unsigned MsWordGenerated::read(const U8 *in, METAFILEPICT *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &out->mm);
    bytes += read(in + bytes, &out->xExt);
    bytes += read(in + bytes, &out->yExt);
    bytes += read(in + bytes, &out->hMF);
    return bytes;
} // METAFILEPICT

unsigned MsWordGenerated::read(const U8 *in, NUMRM *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &out->fNumRM);
    bytes += read(in + bytes, &out->unused1);
    bytes += read(in + bytes, &out->ibstNumRM);
    bytes += read(in + bytes, &out->dttmNumRM);
    for (i = 0; i < 9; i++)
        bytes += read(in + bytes, &out->rgbxchNums[i]);
    for (i = 0; i < 9; i++)
        bytes += read(in + bytes, &out->rgnfc[i]);
    bytes += read(in + bytes, &out->unused26);
    for (i = 0; i < 9; i++)
        bytes += read(in + bytes, &out->PNBR[i]);
    for (i = 0; i < 32; i++)
        bytes += read(in + bytes, &out->xst[i]);
    return bytes;
} // NUMRM

unsigned MsWordGenerated::read(const U8 *in, OBJHEADER *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &out->lcb);
    bytes += read(in + bytes, &out->cbHeader);
    bytes += read(in + bytes, &out->icf);
    return bytes;
} // OBJHEADER

unsigned MsWordGenerated::read(const U8 *in, OLST *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    for (i = 0; i < 9; i++)
        bytes += read(in + bytes, &out->rganlv[i]);
    bytes += read(in + bytes, &out->fRestartHdr);
    bytes += read(in + bytes, &out->fSpareOlst2);
    bytes += read(in + bytes, &out->fSpareOlst3);
    bytes += read(in + bytes, &out->fSpareOlst4);
    for (i = 0; i < 32; i++)
        bytes += read(in + bytes, &out->rgxch[i]);
    return bytes;
} // OLST

unsigned MsWordGenerated::read(const U8 *in, PAP *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &out->istd);
    bytes += read(in + bytes, &out->jc);
    bytes += read(in + bytes, &out->fKeep);
    bytes += read(in + bytes, &out->fKeepFollow);
    bytes += read(in + bytes, &out->fPageBreakBefore);
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
    bytes += read(in + bytes, &out->brcp);
    bytes += read(in + bytes, &out->brcl);
    bytes += read(in + bytes, &out->unused9);
    bytes += read(in + bytes, &out->ilvl);
    bytes += read(in + bytes, &out->fNoLnn);
    bytes += read(in + bytes, &out->ilfo);
    bytes += read(in + bytes, &out->nLvlAnm);
    bytes += read(in + bytes, &out->unused15);
    bytes += read(in + bytes, &out->fSideBySide);
    bytes += read(in + bytes, &out->unused17);
    bytes += read(in + bytes, &out->fNoAutoHyph);
    bytes += read(in + bytes, &out->fWidowControl);
    bytes += read(in + bytes, &out->dxaRight);
    bytes += read(in + bytes, &out->dxaLeft);
    bytes += read(in + bytes, &out->dxaLeft1);
    bytes += read(in + bytes, &out->lspd);
    bytes += read(in + bytes, &out->dyaBefore);
    bytes += read(in + bytes, &out->dyaAfter);
    bytes += read(in + bytes, &out->phe);
    bytes += read(in + bytes, &out->fCrLf);
    bytes += read(in + bytes, &out->fUsePgsuSettings);
    bytes += read(in + bytes, &out->fAdjustRight);
    bytes += read(in + bytes, &out->unused59);
    bytes += read(in + bytes, &out->fKinsoku);
    bytes += read(in + bytes, &out->fWordWrap);
    bytes += read(in + bytes, &out->fOverflowPunct);
    bytes += read(in + bytes, &out->fTopLinePunct);
    bytes += read(in + bytes, &out->fAutoSpaceDE);
    bytes += read(in + bytes, &out->fAutoSpaceDN);
    bytes += read(in + bytes, &out->wAlignFont);
    bytes += read(in + bytes, &shifterU16);
    out->fVertical = shifterU16;
    shifterU16 >>= 1;
    out->fBackward = shifterU16;
    shifterU16 >>= 1;
    out->fRotateFont = shifterU16;
    shifterU16 >>= 1;
    out->unused68_3 = shifterU16;
    shifterU16 >>= 13;
    bytes += read(in + bytes, &out->unused70);
    bytes += read(in + bytes, &out->fInTable);
    bytes += read(in + bytes, &out->fTtp);
    bytes += read(in + bytes, &out->wr);
    bytes += read(in + bytes, &out->fLocked);
    bytes += read(in + bytes, &out->ptap);
    bytes += read(in + bytes, &out->dxaAbs);
    bytes += read(in + bytes, &out->dyaAbs);
    bytes += read(in + bytes, &out->dxaWidth);
    bytes += read(in + bytes, &out->brcTop);
    bytes += read(in + bytes, &out->brcLeft);
    bytes += read(in + bytes, &out->brcBottom);
    bytes += read(in + bytes, &out->brcRight);
    bytes += read(in + bytes, &out->brcBetween);
    bytes += read(in + bytes, &out->brcBar);
    bytes += read(in + bytes, &out->dxaFromText);
    bytes += read(in + bytes, &out->dyaFromText);
    bytes += read(in + bytes, &shifterU16);
    out->dyaHeight = shifterU16;
    shifterU16 >>= 15;
    out->fMinHeight = shifterU16;
    shifterU16 >>= 1;
    bytes += read(in + bytes, &out->shd);
    bytes += read(in + bytes, &out->dcs);
    bytes += read(in + bytes, &out->lvl);
    bytes += read(in + bytes, &out->fNumRMIns);
    bytes += read(in + bytes, &out->anld);
    bytes += read(in + bytes, &out->fPropRMark);
    bytes += read(in + bytes, &out->ibstPropRMark);
    bytes += read(in + bytes, &out->dttmPropRMark);
    bytes += read(in + bytes, &out->numrm);
    bytes += read(in + bytes, &out->itbdMac);
    for (i = 0; i < 64; i++)
        bytes += read(in + bytes, &out->rgdxaTab[i]);
    for (i = 0; i < 64; i++)
        bytes += read(in + bytes, &out->rgtbd[i]);
    return bytes;
} // PAP

//unsigned MsWordGenerated::read(const U8 *in, PAPX *out)
//{
//    U32 shifterU32;
//    U16 shifterU16;
//    U8 shifterU8;
//    unsigned bytes;
//    int i;
//
//    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
//    bytes += read(in + bytes, &out->cb);
//    bytes += read(in + bytes, &out->cw);
//    bytes += read(in + bytes, &out->(cw));
//    bytes += read(in + bytes, &out->istd);
//    bytes += read(in + bytes, &out->array);
//    return bytes;
//} // PAPX

//unsigned MsWordGenerated::read(const U8 *in, PAPXFKP *out)
//{
//    U32 shifterU32;
//    U16 shifterU16;
//    U8 shifterU8;
//    unsigned bytes;
//    int i;
//
//    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
//    bytes += read(in + bytes, &out->rgfc);
//    bytes += read(in + bytes, &out->rgbx);
//    for (i = 0; i < 0; i++)
//        bytes += read(in + bytes, &out->grppapx[i]);
//    bytes += read(in + bytes, &out->crun);
//    return bytes;
//} // PAPXFKP

unsigned MsWordGenerated::read(const U8 *in, PCD *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
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
    bytes += read(in + bytes, &out->fc);
    bytes += read(in + bytes, &out->prm);
    return bytes;
} // PCD

unsigned MsWordGenerated::read(const U8 *in, PGD *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
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
    bytes += read(in + bytes, &out->lnn);
    bytes += read(in + bytes, &out->pgn);
    bytes += read(in + bytes, &out->dym);
    return bytes;
} // PGD

unsigned MsWordGenerated::read(const U8 *in, PHE2 *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &shifterU32);
    out->fSpare = shifterU32;
    shifterU32 >>= 1;
    out->fUnk = shifterU32;
    shifterU32 >>= 1;
    bytes += read(in + bytes, &out->dxaCol);
    bytes += read(in + bytes, &out->dymTableHeight);
    return bytes;
} // PHE2

unsigned MsWordGenerated::read(const U8 *in, PICF *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &out->lcb);
    bytes += read(in + bytes, &out->cbHeader);
    bytes += read(in + bytes, &out->mfp);
    for (i = 0; i < 14; i++)
        bytes += read(in + bytes, &out->bm_rcWinMF[i]);
    bytes += read(in + bytes, &out->dxaGoal);
    bytes += read(in + bytes, &out->dyaGoal);
    bytes += read(in + bytes, &out->mx);
    bytes += read(in + bytes, &out->my);
    bytes += read(in + bytes, &out->dxaCropLeft);
    bytes += read(in + bytes, &out->dyaCropTop);
    bytes += read(in + bytes, &out->dxaCropRight);
    bytes += read(in + bytes, &out->dyaCropBottom);
    bytes += read(in + bytes, &shifterU16);
    out->brcl = shifterU16;
    shifterU16 >>= 4;
    out->fFrameEmpty = shifterU16;
    shifterU16 >>= 1;
    out->fBitmap = shifterU16;
    shifterU16 >>= 1;
    out->fDrawHatch = shifterU16;
    shifterU16 >>= 1;
    out->fError = shifterU16;
    shifterU16 >>= 1;
    out->bpp = shifterU16;
    shifterU16 >>= 8;
    bytes += read(in + bytes, &out->brcTop);
    bytes += read(in + bytes, &out->brcLeft);
    bytes += read(in + bytes, &out->brcBottom);
    bytes += read(in + bytes, &out->brcRight);
    bytes += read(in + bytes, &out->dxaOrigin);
    bytes += read(in + bytes, &out->dyaOrigin);
    bytes += read(in + bytes, &out->cProps);
    return bytes;
} // PICF

//unsigned MsWordGenerated::read(const U8 *in, PLCF *out)
//{
//    U32 shifterU32;
//    U16 shifterU16;
//    U8 shifterU8;
//    unsigned bytes;
//    int i;
//
//    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
//    for (i = 0; i < 0; i++)
//        bytes += read(in + bytes, &out->rgfc[i]);
//    for (i = 0; i < 0; i++)
//        bytes += read(in + bytes, &out->rgstruct[i]);
//    return bytes;
//} // PLCF

unsigned MsWordGenerated::read(const U8 *in, PRM2 *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &shifterU16);
    out->fComplex = shifterU16;
    shifterU16 >>= 1;
    out->igrpprl = shifterU16;
    shifterU16 >>= 15;
    return bytes;
} // PRM2

unsigned MsWordGenerated::read(const U8 *in, RR *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &out->cb);
    bytes += read(in + bytes, &out->cbSzRecip);
    return bytes;
} // RR

unsigned MsWordGenerated::read(const U8 *in, RS *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &out->fRouted);
    bytes += read(in + bytes, &out->fReturnOrig);
    bytes += read(in + bytes, &out->fTrackStatus);
    bytes += read(in + bytes, &out->fDirty);
    bytes += read(in + bytes, &out->nProtect);
    bytes += read(in + bytes, &out->iStage);
    bytes += read(in + bytes, &out->delOption);
    bytes += read(in + bytes, &out->cRecip);
    return bytes;
} // RS

unsigned MsWordGenerated::read(const U8 *in, SED *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &out->fn);
    bytes += read(in + bytes, &out->fcSepx);
    bytes += read(in + bytes, &out->fnMpr);
    bytes += read(in + bytes, &out->fcMpr);
    return bytes;
} // SED

unsigned MsWordGenerated::read(const U8 *in, SEP *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &out->bkc);
    bytes += read(in + bytes, &out->fTitlePage);
    bytes += read(in + bytes, &out->fAutoPgn);
    bytes += read(in + bytes, &out->nfcPgn);
    bytes += read(in + bytes, &out->fUnlocked);
    bytes += read(in + bytes, &out->cnsPgn);
    bytes += read(in + bytes, &out->fPgnRestart);
    bytes += read(in + bytes, &out->fEndNote);
    bytes += read(in + bytes, &out->lnc);
    bytes += read(in + bytes, &out->grpfIhdt);
    bytes += read(in + bytes, &out->nLnnMod);
    bytes += read(in + bytes, &out->dxaLnn);
    bytes += read(in + bytes, &out->dxaPgn);
    bytes += read(in + bytes, &out->dyaPgn);
    bytes += read(in + bytes, &out->fLBetween);
    bytes += read(in + bytes, &out->vjc);
    bytes += read(in + bytes, &out->dmBinFirst);
    bytes += read(in + bytes, &out->dmBinOther);
    bytes += read(in + bytes, &out->dmPaperReq);
    bytes += read(in + bytes, &out->brcTop);
    bytes += read(in + bytes, &out->brcLeft);
    bytes += read(in + bytes, &out->brcBottom);
    bytes += read(in + bytes, &out->brcRight);
    bytes += read(in + bytes, &out->fPropRMark);
    bytes += read(in + bytes, &out->ibstPropRMark);
    bytes += read(in + bytes, &out->dttmPropRMark);
    bytes += read(in + bytes, &out->dxtCharSpace);
    bytes += read(in + bytes, &out->dyaLinePitch);
    bytes += read(in + bytes, &out->clm);
    bytes += read(in + bytes, &out->unused62);
    bytes += read(in + bytes, &out->dmOrientPage);
    bytes += read(in + bytes, &out->iHeadingPgn);
    bytes += read(in + bytes, &out->pgnStart);
    bytes += read(in + bytes, &out->lnnMin);
    bytes += read(in + bytes, &out->wTextFlow);
    bytes += read(in + bytes, &out->unused72);
    bytes += read(in + bytes, &shifterU16);
    out->pgbApplyTo = shifterU16;
    shifterU16 >>= 3;
    out->pgbPageDepth = shifterU16;
    shifterU16 >>= 2;
    out->pgbOffsetFrom = shifterU16;
    shifterU16 >>= 3;
    out->unused74_8 = shifterU16;
    shifterU16 >>= 8;
    bytes += read(in + bytes, &out->xaPage);
    bytes += read(in + bytes, &out->yaPage);
    bytes += read(in + bytes, &out->xaPageNUp);
    bytes += read(in + bytes, &out->yaPageNUp);
    bytes += read(in + bytes, &out->dxaLeft);
    bytes += read(in + bytes, &out->dxaRight);
    bytes += read(in + bytes, &out->dyaTop);
    bytes += read(in + bytes, &out->dyaBottom);
    bytes += read(in + bytes, &out->dzaGutter);
    bytes += read(in + bytes, &out->dyaHdrTop);
    bytes += read(in + bytes, &out->dyaHdrBottom);
    bytes += read(in + bytes, &out->ccolM1);
    bytes += read(in + bytes, &out->fEvenlySpaced);
    bytes += read(in + bytes, &out->unused123);
    bytes += read(in + bytes, &out->dxaColumns);
    for (i = 0; i < 89; i++)
        bytes += read(in + bytes, &out->rgdxaColumnWidthSpacing[i]);
    bytes += read(in + bytes, &out->dxaColumnWidth);
    bytes += read(in + bytes, &out->dmOrientFirst);
    bytes += read(in + bytes, &out->fLayout);
    bytes += read(in + bytes, &out->unused490);
    bytes += read(in + bytes, &out->olstAnm);
    return bytes;
} // SEP

//unsigned MsWordGenerated::read(const U8 *in, SEPX *out)
//{
//    U32 shifterU32;
//    U16 shifterU16;
//    U8 shifterU8;
//    unsigned bytes;
//    int i;
//
//    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
//    bytes += read(in + bytes, &out->cb);
//    for (i = 0; i < 0; i++)
//        bytes += read(in + bytes, &out->grpprl[i]);
//    return bytes;
//} // SEPX

//unsigned MsWordGenerated::read(const U8 *in, STD *out)
//{
//    U32 shifterU32;
//    U16 shifterU16;
//    U8 shifterU8;
//    unsigned bytes;
//    int i;
//
//    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
//    bytes += read(in + bytes, &shifterU16);
//    out->sti = shifterU16;
//    shifterU16 >>= 12;
//    out->fScratch = shifterU16;
//    shifterU16 >>= 1;
//    out->fInvalHeight = shifterU16;
//    shifterU16 >>= 1;
//    out->fHasUpe = shifterU16;
//    shifterU16 >>= 1;
//    out->fMassCopy = shifterU16;
//    shifterU16 >>= 1;
//    bytes += read(in + bytes, &shifterU16);
//    out->sgc = shifterU16;
//    shifterU16 >>= 4;
//    out->istdBase = shifterU16;
//    shifterU16 >>= 12;
//    bytes += read(in + bytes, &shifterU16);
//    out->cupx = shifterU16;
//    shifterU16 >>= 4;
//    out->istdNext = shifterU16;
//    shifterU16 >>= 12;
//    bytes += read(in + bytes, &out->bchUpe);
//    bytes += read(in + bytes, &shifterU16);
//    out->fAutoRedef = shifterU16;
//    shifterU16 >>= 1;
//    out->fHidden = shifterU16;
//    shifterU16 >>= 1;
//    out->unused8_3 = shifterU16;
//    shifterU16 >>= 14;
//    for (i = 0; i < 0; i++)
//        bytes += read(in + bytes, &out->xstzName[i]);
//    return bytes;
//} // STD

unsigned MsWordGenerated::read(const U8 *in, STSHI *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &out->cstd);
    bytes += read(in + bytes, &out->cbSTDBaseInFile);
    bytes += read(in + bytes, &shifterU16);
    out->fStdStylenamesWritten = shifterU16;
    shifterU16 >>= 1;
    out->unused4_2 = shifterU16;
    shifterU16 >>= 15;
    bytes += read(in + bytes, &out->stiMaxWhenSaved);
    bytes += read(in + bytes, &out->istdMaxFixedWhenSaved);
    bytes += read(in + bytes, &out->nVerBuiltInNamesWhenSaved);
    for (i = 0; i < 3; i++)
        bytes += read(in + bytes, &out->rgftcStandardChpStsh[i]);
    return bytes;
} // STSHI

unsigned MsWordGenerated::read(const U8 *in, TAP *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &out->jc);
    bytes += read(in + bytes, &out->dxaGapHalf);
    bytes += read(in + bytes, &out->dyaRowHeight);
    bytes += read(in + bytes, &out->fCantSplit);
    bytes += read(in + bytes, &out->fTableHeader);
    bytes += read(in + bytes, &out->tlp);
    bytes += read(in + bytes, &out->lwHTMLProps);
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
    bytes += read(in + bytes, &out->itcMac);
    bytes += read(in + bytes, &out->dxaAdjust);
    bytes += read(in + bytes, &out->dxaScale);
    bytes += read(in + bytes, &out->dxsInch);
    for (i = 0; i < 65; i++)
        bytes += read(in + bytes, &out->rgdxaCenter[i]);
    for (i = 0; i < 65; i++)
        bytes += read(in + bytes, &out->rgdxaCenterPrint[i]);
    for (i = 0; i < 64; i++)
        bytes += read(in + bytes, &out->rgtc[i]);
    for (i = 0; i < 64; i++)
        bytes += read(in + bytes, &out->rgshd[i]);
    for (i = 0; i < 6; i++)
        bytes += read(in + bytes, &out->rgbrcTable[i]);
    return bytes;
} // TAP

unsigned MsWordGenerated::read(const U8 *in, TBD *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &shifterU8);
    out->jc = shifterU8;
    shifterU8 >>= 3;
    out->tlc = shifterU8;
    shifterU8 >>= 3;
    out->unused0_6 = shifterU8;
    shifterU8 >>= 2;
    return bytes;
} // TBD

unsigned MsWordGenerated::read(const U8 *in, WKB *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += read(in + bytes, &out->fn);
    bytes += read(in + bytes, &out->grfwkb);
    bytes += read(in + bytes, &out->lvl);
    bytes += read(in + bytes, &shifterU16);
    out->fnpt = shifterU16;
    shifterU16 >>= 4;
    out->fnpd = shifterU16;
    shifterU16 >>= 12;
    bytes += read(in + bytes, &out->doc);
    return bytes;
} // WKB

