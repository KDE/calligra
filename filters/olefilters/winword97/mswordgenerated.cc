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

unsigned MsWordGenerated::read(const U8 *in, S8 *out, unsigned count)
{
    return read(in, (U8 *)out, count);
}

unsigned MsWordGenerated::read(const U8 *in, U8 *out, unsigned count)
{
    memcpy(out, in, count);
    return count;
}

unsigned MsWordGenerated::read(const U8 *in, S16 *out, unsigned count)
{
    return read(in, (U16 *)out, count);
}

unsigned MsWordGenerated::read(const U8 *in, U16 *out, unsigned count)
{
    for (unsigned i = 0; i < count; i++)
    {
        *out = (U16)((*(in + 1) << 8) + (*in));
        out++;
        in += 2;
    }
    return count + count;
}

unsigned MsWordGenerated::read(const U8 *in, S32 *out, unsigned count)
{
    return read(in, (U32 *)out, count);
}

unsigned MsWordGenerated::read(const U8 *in, U32 *out, unsigned count)
{
    for (unsigned i = 0; i < count; i++)
    {
        *out = (U32)((*(in + 3) << 24) + (*(in + 2) << 16) + (*(in + 1) << 8) + (*in));
        out++;
        in += 4;
    }
    return count + count + count + count;
}

unsigned MsWordGenerated::read(const U8 *in, DTTM *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, (U16 *)&shiftRegister);
        out->mint = shiftRegister;
        shiftRegister >>= 6;
        out->hr = shiftRegister;
        shiftRegister >>= 5;
        out->dom = shiftRegister;
        shiftRegister >>= 5;
        in += read(in, (U16 *)&shiftRegister);
        out->mon = shiftRegister;
        shiftRegister >>= 4;
        out->yr = shiftRegister;
        shiftRegister >>= 9;
        out->wdy = shiftRegister;
        shiftRegister >>= 3;
        out++;
    }
    return count * sizeof(DTTM);
} // DTTM

unsigned MsWordGenerated::read(const U8 *in, SHD *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, (U16 *)&shiftRegister);
        out->icoFore = shiftRegister;
        shiftRegister >>= 5;
        out->icoBack = shiftRegister;
        shiftRegister >>= 5;
        out->ipat = shiftRegister;
        shiftRegister >>= 6;
        out++;
    }
    return count * sizeof(SHD);
} // SHD

unsigned MsWordGenerated::read(const U8 *in, DOPTYPOGRAPHY *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, (U16 *)&shiftRegister);
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
        in += read(in, &out->cchFollowingPunct); // U16
        in += read(in, &out->cchLeadingPunct); // U16
        in += read(in, &out->rgxchFPunct[0], 101); // XCHAR
        in += read(in, &out->rgxchLPunct[0], 51); // XCHAR
        out++;
    }
    return count * sizeof(DOPTYPOGRAPHY);
} // DOPTYPOGRAPHY

unsigned MsWordGenerated::read(const U8 *in, PRM *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, (U16 *)&shiftRegister);
        out->fComplex = shiftRegister;
        shiftRegister >>= 1;
        out->isprm = shiftRegister;
        shiftRegister >>= 7;
        out->val = shiftRegister;
        shiftRegister >>= 8;
        out++;
    }
    return count * sizeof(PRM);
} // PRM

unsigned MsWordGenerated::read(const U8 *in, ATRD *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, &out->xstUsrInitl[0], 10); // XCHAR
        in += read(in, &out->ibst); // U16
        in += read(in, (U16 *)&shiftRegister);
        out->ak = shiftRegister;
        shiftRegister >>= 2;
        out->unused22_2 = shiftRegister;
        shiftRegister >>= 14;
        in += read(in, &out->grfbmc); // U16
        in += read(in, &out->lTagBkmk); // U32
        out++;
    }
    return count * sizeof(ATRD);
} // ATRD

unsigned MsWordGenerated::read(const U8 *in, ANLD *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, &out->nfc); // U8
        in += read(in, &out->cxchTextBefore); // U8
        in += read(in, &out->cxchTextAfter); // U8
        in += read(in, (U8 *)&shiftRegister);
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
        in += read(in, (U8 *)&shiftRegister);
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
        in += read(in, (U8 *)&shiftRegister);
        out->kul = shiftRegister;
        shiftRegister >>= 3;
        out->ico = shiftRegister;
        shiftRegister >>= 5;
        in += read(in, &out->ftc); // U16
        in += read(in, &out->hps); // U16
        in += read(in, &out->iStartAt); // U16
        in += read(in, &out->dxaIndent); // U16
        in += read(in, &out->dxaSpace); // U16
        in += read(in, &out->fNumber1); // U8
        in += read(in, &out->fNumberAcross); // U8
        in += read(in, &out->fRestartHdn); // U8
        in += read(in, &out->fSpareX); // U8
        in += read(in, &out->rgxch[0], 32); // XCHAR
        out++;
    }
    return count * sizeof(ANLD);
} // ANLD

unsigned MsWordGenerated::read(const U8 *in, ANLV *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, &out->nfc); // U8
        in += read(in, &out->cxchTextBefore); // U8
        in += read(in, &out->cxchTextAfter); // U8
        in += read(in, (U8 *)&shiftRegister);
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
        in += read(in, (U8 *)&shiftRegister);
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
        in += read(in, (U8 *)&shiftRegister);
        out->kul = shiftRegister;
        shiftRegister >>= 3;
        out->ico = shiftRegister;
        shiftRegister >>= 5;
        in += read(in, &out->ftc); // U16
        in += read(in, &out->hps); // U16
        in += read(in, &out->iStartAt); // U16
        in += read(in, &out->dxaIndent); // U16
        in += read(in, &out->dxaSpace); // U16
        out++;
    }
    return count * sizeof(ANLV);
} // ANLV

unsigned MsWordGenerated::read(const U8 *in, ASUMY *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, &out->lLevel); // U32
        out++;
    }
    return count * sizeof(ASUMY);
} // ASUMY

unsigned MsWordGenerated::read(const U8 *in, ASUMYI *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, (U16 *)&shiftRegister);
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
        in += read(in, &out->wDlgLevel); // U16
        in += read(in, &out->lHighestLevel); // U32
        in += read(in, &out->lCurrentLevel); // U32
        out++;
    }
    return count * sizeof(ASUMYI);
} // ASUMYI

unsigned MsWordGenerated::read(const U8 *in, BTE *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, &out->pn); // U32
        out++;
    }
    return count * sizeof(BTE);
} // BTE

unsigned MsWordGenerated::read(const U8 *in, BKD *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, &out->ipgd); // U16
        in += read(in, &out->itxbxs); // U16
        in += read(in, &out->dcpDepend); // U16
        in += read(in, (U16 *)&shiftRegister);
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
    return count * sizeof(BKD);
} // BKD

unsigned MsWordGenerated::read(const U8 *in, BKF *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, &out->ibkl); // U16
        in += read(in, (U16 *)&shiftRegister);
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
    return count * sizeof(BKF);
} // BKF

unsigned MsWordGenerated::read(const U8 *in, BKL *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, &out->ibkf); // U16
        out++;
    }
    return count * sizeof(BKL);
} // BKL

unsigned MsWordGenerated::read(const U8 *in, BRC *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, (U16 *)&shiftRegister);
        out->dptLineWidth = shiftRegister;
        shiftRegister >>= 8;
        out->brcType = shiftRegister;
        shiftRegister >>= 8;
        in += read(in, (U16 *)&shiftRegister);
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
    return count * sizeof(BRC);
} // BRC

unsigned MsWordGenerated::read(const U8 *in, BRC10 *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, (U16 *)&shiftRegister);
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
    return count * sizeof(BRC10);
} // BRC10

unsigned MsWordGenerated::read(const U8 *in, CHP *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, (U8 *)&shiftRegister);
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
        in += read(in, (U8 *)&shiftRegister);
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
        in += read(in, (U16 *)&shiftRegister);
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
        in += read(in, &out->unused4); // U32
        in += read(in, &out->ftc); // U16
        in += read(in, &out->ftcAscii); // U16
        in += read(in, &out->ftcFE); // U16
        in += read(in, &out->ftcOther); // U16
        in += read(in, &out->hps); // U16
        in += read(in, &out->dxaSpace); // U32
        in += read(in, (U8 *)&shiftRegister);
        out->iss = shiftRegister;
        shiftRegister >>= 3;
        out->kul = shiftRegister;
        shiftRegister >>= 4;
        out->fSpecSymbol = shiftRegister;
        shiftRegister >>= 1;
        in += read(in, (U8 *)&shiftRegister);
        out->ico = shiftRegister;
        shiftRegister >>= 5;
        out->unused23_5 = shiftRegister;
        shiftRegister >>= 1;
        out->fSysVanish = shiftRegister;
        shiftRegister >>= 1;
        out->hpsPos = shiftRegister;
        shiftRegister >>= 1;
        in += read(in, &out->hpScript); // U16
        in += read(in, &out->lid); // U16
        in += read(in, &out->lidDefault); // U16
        in += read(in, &out->lidFE); // U16
        in += read(in, &out->idct); // U8
        in += read(in, &out->idctHint); // U8
        in += read(in, &out->wCharScale); // U16
        in += read(in, &out->fcPic_fcObj_lTagObj); // U32
        in += read(in, &out->ibstRMark); // U16
        in += read(in, &out->ibstRMarkDel); // U16
        in += read(in, &out->dttmRMark); // DTTM
        in += read(in, &out->dttmRMarkDel); // DTTM
        in += read(in, &out->unused52); // U16
        in += read(in, &out->istd); // U16
        in += read(in, &out->ftcSym); // U16
        in += read(in, &out->xchSym[0], 1); // XCHAR
        in += read(in, &out->idslRMReason); // U16
        in += read(in, &out->idslReasonDel); // U16
        in += read(in, &out->ysr); // U8
        in += read(in, &out->chYsr); // U8
        in += read(in, &out->cpg); // U16
        in += read(in, &out->hpsKern); // U16
        in += read(in, (U16 *)&shiftRegister);
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
        in += read(in, &out->fPropMark); // U16
        in += read(in, &out->ibstPropRMark); // U16
        in += read(in, &out->dttmPropRMark); // DTTM
        in += read(in, &out->sfxtText); // U8
        in += read(in, &out->unused81); // U8
        in += read(in, &out->unused82); // U8
        in += read(in, &out->unused83); // U16
        in += read(in, &out->unused85); // U16
        in += read(in, &out->unused87); // U32
        in += read(in, &out->fDispFldRMark); // U8
        in += read(in, &out->ibstDispFldRMark); // U16
        in += read(in, &out->dttmDispFldRMark); // U32
        in += read(in, &out->xstDispFldRMark[0], 16); // XCHAR
        in += read(in, &out->shd); // SHD
        in += read(in, &out->brc); // BRC
        out++;
    }
    return count * sizeof(CHP);
} // CHP

//unsigned MsWordGenerated::read(const U8 *in, CHPX *out, unsigned count)
//{
//    unsigned long shiftRegister;
//
//    for (unsigned i = 0; i < count; i++)
//    {
//        in += read(in, &out->cb); // U8
//        in += read(in, &out->grpprl[0], cb); // U8
//        out++;
//    }
//    return count * sizeof(CHPX);
//} // CHPX

unsigned MsWordGenerated::read(const U8 *in, DCS *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, (U8 *)&shiftRegister);
        out->fdct = shiftRegister;
        shiftRegister >>= 3;
        out->unused0_3 = shiftRegister;
        shiftRegister >>= 5;
        in += read(in, &out->unused1); // U16
        out++;
    }
    return count * sizeof(DCS);
} // DCS

unsigned MsWordGenerated::read(const U8 *in, DOGRID *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, &out->xaGrid); // U16
        in += read(in, &out->yaGrid); // U16
        in += read(in, &out->dxaGrid); // U16
        in += read(in, &out->dyaGrid); // U16
        in += read(in, (U16 *)&shiftRegister);
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
    return count * sizeof(DOGRID);
} // DOGRID

unsigned MsWordGenerated::read(const U8 *in, DOP *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, (U16 *)&shiftRegister);
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
        in += read(in, (U16 *)&shiftRegister);
        out->rncFtn = shiftRegister;
        shiftRegister >>= 2;
        out->nFtn = shiftRegister;
        shiftRegister >>= 14;
        in += read(in, (U8 *)&shiftRegister);
        out->fOutlineDirtySave = shiftRegister;
        shiftRegister >>= 1;
        out->unused4_1 = shiftRegister;
        shiftRegister >>= 7;
        in += read(in, (U8 *)&shiftRegister);
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
        in += read(in, (U8 *)&shiftRegister);
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
        in += read(in, (U8 *)&shiftRegister);
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
        in += read(in, (U16 *)&shiftRegister);
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
        in += read(in, &out->dxaTab); // U16
        in += read(in, &out->wSpare); // U16
        in += read(in, &out->dxaHotZ); // U16
        in += read(in, &out->cConsecHypLim); // U16
        in += read(in, &out->wSpare2); // U16
        in += read(in, &out->dttmCreated); // DTTM
        in += read(in, &out->dttmRevised); // DTTM
        in += read(in, &out->dttmLastPrint); // DTTM
        in += read(in, &out->nRevision); // U16
        in += read(in, &out->tmEdited); // U32
        in += read(in, &out->cWords); // U32
        in += read(in, &out->cCh); // U32
        in += read(in, &out->cPg); // U16
        in += read(in, &out->cParas); // U32
        in += read(in, (U16 *)&shiftRegister);
        out->rncEdn = shiftRegister;
        shiftRegister >>= 2;
        out->nEdn = shiftRegister;
        shiftRegister >>= 14;
        in += read(in, (U16 *)&shiftRegister);
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
        in += read(in, &out->cLines); // U32
        in += read(in, &out->cWordsFtnEnd); // U32
        in += read(in, &out->cChFtnEdn); // U32
        in += read(in, &out->cPgFtnEdn); // U16
        in += read(in, &out->cParasFtnEdn); // U32
        in += read(in, &out->cLinesFtnEdn); // U32
        in += read(in, &out->lKeyProtDoc); // U32
        in += read(in, (U16 *)&shiftRegister);
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
    return count * sizeof(DOP);
} // DOP

unsigned MsWordGenerated::read(const U8 *in, DOP102 *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, (U32 *)&shiftRegister);
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
    return count * sizeof(DOP102);
} // DOP102

unsigned MsWordGenerated::read(const U8 *in, DOP105 *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, &out->adt); // U16
        in += read(in, &out->doptypography); // DOPTYPOGRAPHY
        in += read(in, &out->dogrid); // DOGRID
        in += read(in, (U16 *)&shiftRegister);
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
        in += read(in, (U16 *)&shiftRegister);
        out->fHaveVersions = shiftRegister;
        shiftRegister >>= 1;
        out->fAutoVersion = shiftRegister;
        shiftRegister >>= 1;
        out->unused412_2 = shiftRegister;
        shiftRegister >>= 14;
        in += read(in, &out->asumyi); // ASUMYI
        in += read(in, &out->cChWS); // U32
        in += read(in, &out->cChWSFtnEdn); // U32
        in += read(in, &out->grfDocEvents); // U32
        in += read(in, (U32 *)&shiftRegister);
        out->fVirusPrompted = shiftRegister;
        shiftRegister >>= 1;
        out->fVirusLoadSafe = shiftRegister;
        shiftRegister >>= 1;
        out->KeyVirusSession30 = shiftRegister;
        shiftRegister >>= 30;
        in += read(in, &out->Spare[0], 30); // U8
        in += read(in, &out->unused472); // U32
        in += read(in, &out->unused476); // U32
        in += read(in, &out->cDBC); // U32
        in += read(in, &out->cDBCFtnEdn); // U32
        in += read(in, &out->unused488); // U32
        in += read(in, &out->nfcFtnRef); // U16
        in += read(in, &out->nfcEdnRef); // U16
        in += read(in, &out->hpsZoonFontPag); // U16
        in += read(in, &out->dywDispPag); // U16
        out++;
    }
    return count * sizeof(DOP105);
} // DOP105

unsigned MsWordGenerated::read(const U8 *in, FLD *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, (U8 *)&shiftRegister);
        out->ch = shiftRegister;
        shiftRegister >>= 5;
        out->unused0_5 = shiftRegister;
        shiftRegister >>= 3;
        in += read(in, &out->flt); // U8
        out++;
    }
    return count * sizeof(FLD);
} // FLD

unsigned MsWordGenerated::read(const U8 *in, FSPA *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, &out->spid); // U32
        in += read(in, &out->xaLeft); // U32
        in += read(in, &out->yaTop); // U32
        in += read(in, &out->xaRight); // U32
        in += read(in, &out->yaBottom); // U32
        in += read(in, (U16 *)&shiftRegister);
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
        in += read(in, &out->cTxbx); // U32
        out++;
    }
    return count * sizeof(FSPA);
} // FSPA

//unsigned MsWordGenerated::read(const U8 *in, FFN *out, unsigned count)
//{
//    unsigned long shiftRegister;
//
//    for (unsigned i = 0; i < count; i++)
//    {
//        in += read(in, &out->cbFfnM1); // U8
//        in += read(in, (U8 *)&shiftRegister);
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
//        in += read(in, &out->wWeight); // U16
//        in += read(in, &out->chs); // U8
//        in += read(in, &out->ixchSzAlt); // U8
//        in += read(in, &out->panose[0], 10); // U8
//        in += read(in, &out->fs[0], 24); // U8
//        in += read(in, &out->xszFfn[0], 0); // U8
//        out++;
//    }
//    return count * sizeof(FFN);
//} // FFN

unsigned MsWordGenerated::read(const U8 *in, FIB *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, &out->wIdent); // U16
        in += read(in, &out->nFib); // U16
        in += read(in, &out->nProduct); // U16
        in += read(in, &out->lid); // U16
        in += read(in, &out->pnNext); // U16
        in += read(in, (U16 *)&shiftRegister);
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
        in += read(in, &out->nFibBack); // U16
        in += read(in, &out->lKey); // U32
        in += read(in, &out->envr); // U8
        in += read(in, (U8 *)&shiftRegister);
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
        in += read(in, &out->chs); // U16
        in += read(in, &out->chsTables); // U16
        in += read(in, &out->fcMin); // U32
        in += read(in, &out->fcMac); // U32
        in += read(in, &out->csw); // U16
        in += read(in, &out->wMagicCreated); // U16
        in += read(in, &out->wMagicRevised); // U16
        in += read(in, &out->wMagicCreatedPrivate); // U16
        in += read(in, &out->wMagicRevisedPrivate); // U16
        in += read(in, &out->pnFbpChpFirst_W6); // U16
        in += read(in, &out->pnChpFirst_W6); // U16
        in += read(in, &out->cpnBteChp_W6); // U16
        in += read(in, &out->pnFbpPapFirst_W6); // U16
        in += read(in, &out->pnPapFirst_W6); // U16
        in += read(in, &out->cpnBtePap_W6); // U16
        in += read(in, &out->pnFbpLvcFirst_W6); // U16
        in += read(in, &out->pnLvcFirst_W6); // U16
        in += read(in, &out->cpnBteLvc_W6); // U16
        in += read(in, &out->lidFE); // U16
        in += read(in, &out->clw); // U16
        in += read(in, &out->cbMac); // U32
        in += read(in, &out->lProductCreated); // U32
        in += read(in, &out->lProductRevised); // U32
        in += read(in, &out->ccpText); // U32
        in += read(in, &out->ccpFtn); // U32
        in += read(in, &out->ccpHdd); // U32
        in += read(in, &out->ccpMcr); // U32
        in += read(in, &out->ccpAtn); // U32
        in += read(in, &out->ccpEdn); // U32
        in += read(in, &out->ccpTxbx); // U32
        in += read(in, &out->ccpHdrTxbx); // U32
        in += read(in, &out->pnFbpChpFirst); // U32
        in += read(in, &out->pnChpFirst); // U32
        in += read(in, &out->cpnBteChp); // U32
        in += read(in, &out->pnFbpPapFirst); // U32
        in += read(in, &out->pnPapFirst); // U32
        in += read(in, &out->cpnBtePap); // U32
        in += read(in, &out->pnFbpLvcFirst); // U32
        in += read(in, &out->pnLvcFirst); // U32
        in += read(in, &out->cpnBteLvc); // U32
        in += read(in, &out->fcIslandFirst); // U32
        in += read(in, &out->fcIslandLim); // U32
        in += read(in, &out->cfclcb); // U16
        in += read(in, &out->fcStshfOrig); // U32
        in += read(in, &out->lcbStshfOrig); // U32
        in += read(in, &out->fcStshf); // U32
        in += read(in, &out->lcbStshf); // U32
        in += read(in, &out->fcPlcffndRef); // U32
        in += read(in, &out->lcbPlcffndRef); // U32
        in += read(in, &out->fcPlcffndTxt); // U32
        in += read(in, &out->lcbPlcffndTxt); // U32
        in += read(in, &out->fcPlcfandRef); // U32
        in += read(in, &out->lcbPlcfandRef); // U32
        in += read(in, &out->fcPlcfandTxt); // U32
        in += read(in, &out->lcbPlcfandTxt); // U32
        in += read(in, &out->fcPlcfsed); // U32
        in += read(in, &out->lcbPlcfsed); // U32
        in += read(in, &out->fcPlcpad); // U32
        in += read(in, &out->lcbPlcpad); // U32
        in += read(in, &out->fcPlcfphe); // U32
        in += read(in, &out->lcbPlcfphe); // U32
        in += read(in, &out->fcSttbfglsy); // U32
        in += read(in, &out->lcbSttbfglsy); // U32
        in += read(in, &out->fcPlcfglsy); // U32
        in += read(in, &out->lcbPlcfglsy); // U32
        in += read(in, &out->fcPlcfhdd); // U32
        in += read(in, &out->lcbPlcfhdd); // U32
        in += read(in, &out->fcPlcfbteChpx); // U32
        in += read(in, &out->lcbPlcfbteChpx); // U32
        in += read(in, &out->fcPlcfbtePapx); // U32
        in += read(in, &out->lcbPlcfbtePapx); // U32
        in += read(in, &out->fcPlcfsea); // U32
        in += read(in, &out->lcbPlcfsea); // U32
        in += read(in, &out->fcSttbfffn); // U32
        in += read(in, &out->lcbSttbfffn); // U32
        in += read(in, &out->fcPlcffldMom); // U32
        in += read(in, &out->lcbPlcffldMom); // U32
        in += read(in, &out->fcPlcffldHdr); // U32
        in += read(in, &out->lcbPlcffldHdr); // U32
        in += read(in, &out->fcPlcffldFtn); // U32
        in += read(in, &out->lcbPlcffldFtn); // U32
        in += read(in, &out->fcPlcffldAtn); // U32
        in += read(in, &out->lcbPlcffldAtn); // U32
        in += read(in, &out->fcPlcffldMcr); // U32
        in += read(in, &out->lcbPlcffldMcr); // U32
        in += read(in, &out->fcSttbfbkmk); // U32
        in += read(in, &out->lcbSttbfbkmk); // U32
        in += read(in, &out->fcPlcfbkf); // U32
        in += read(in, &out->lcbPlcfbkf); // U32
        in += read(in, &out->fcPlcfbkl); // U32
        in += read(in, &out->lcbPlcfbkl); // U32
        in += read(in, &out->fcCmds); // U32
        in += read(in, &out->lcbCmds); // U32
        in += read(in, &out->fcPlcmcr); // U32
        in += read(in, &out->lcbPlcmcr); // U32
        in += read(in, &out->fcSttbfmcr); // U32
        in += read(in, &out->lcbSttbfmcr); // U32
        in += read(in, &out->fcPrDrvr); // U32
        in += read(in, &out->lcbPrDrvr); // U32
        in += read(in, &out->fcPrEnvPort); // U32
        in += read(in, &out->lcbPrEnvPort); // U32
        in += read(in, &out->fcPrEnvLand); // U32
        in += read(in, &out->lcbPrEnvLand); // U32
        in += read(in, &out->fcWss); // U32
        in += read(in, &out->lcbWss); // U32
        in += read(in, &out->fcDop); // U32
        in += read(in, &out->lcbDop); // U32
        in += read(in, &out->fcSttbfAssoc); // U32
        in += read(in, &out->lcbSttbfAssoc); // U32
        in += read(in, &out->fcClx); // U32
        in += read(in, &out->lcbClx); // U32
        in += read(in, &out->fcPlcfpgdFtn); // U32
        in += read(in, &out->lcbPlcfpgdFtn); // U32
        in += read(in, &out->fcAutosaveSource); // U32
        in += read(in, &out->lcbAutosaveSource); // U32
        in += read(in, &out->fcGrpXstAtnOwners); // U32
        in += read(in, &out->lcbGrpXstAtnOwners); // U32
        in += read(in, &out->fcSttbfAtnbkmk); // U32
        in += read(in, &out->lcbSttbfAtnbkmk); // U32
        in += read(in, &out->fcPlcdoaMom); // U32
        in += read(in, &out->lcbPlcdoaMom); // U32
        in += read(in, &out->fcPlcdoaHdr); // U32
        in += read(in, &out->lcbPlcdoaHdr); // U32
        in += read(in, &out->fcPlcspaMom); // U32
        in += read(in, &out->lcbPlcspaMom); // U32
        in += read(in, &out->fcPlcspaHdr); // U32
        in += read(in, &out->lcbPlcspaHdr); // U32
        in += read(in, &out->fcPlcfAtnbkf); // U32
        in += read(in, &out->lcbPlcfAtnbkf); // U32
        in += read(in, &out->fcPlcfAtnbkl); // U32
        in += read(in, &out->lcbPlcfAtnbkl); // U32
        in += read(in, &out->fcPms); // U32
        in += read(in, &out->lcbPms); // U32
        in += read(in, &out->fcFormFldSttbs); // U32
        in += read(in, &out->lcbFormFldSttbs); // U32
        in += read(in, &out->fcPlcfendRef); // U32
        in += read(in, &out->lcbPlcfendRef); // U32
        in += read(in, &out->fcPlcfendTxt); // U32
        in += read(in, &out->lcbPlcfendTxt); // U32
        in += read(in, &out->fcPlcffldEdn); // U32
        in += read(in, &out->lcbPlcffldEdn); // U32
        in += read(in, &out->fcPlcfpgdEdn); // U32
        in += read(in, &out->lcbPlcfpgdEdn); // U32
        in += read(in, &out->fcDggInfo); // U32
        in += read(in, &out->lcbDggInfo); // U32
        in += read(in, &out->fcSttbfRMark); // U32
        in += read(in, &out->lcbSttbfRMark); // U32
        in += read(in, &out->fcSttbCaption); // U32
        in += read(in, &out->lcbSttbCaption); // U32
        in += read(in, &out->fcSttbAutoCaption); // U32
        in += read(in, &out->lcbSttbAutoCaption); // U32
        in += read(in, &out->fcPlcfwkb); // U32
        in += read(in, &out->lcbPlcfwkb); // U32
        in += read(in, &out->fcPlcfspl); // U32
        in += read(in, &out->lcbPlcfspl); // U32
        in += read(in, &out->fcPlcftxbxTxt); // U32
        in += read(in, &out->lcbPlcftxbxTxt); // U32
        in += read(in, &out->fcPlcffldTxbx); // U32
        in += read(in, &out->lcbPlcffldTxbx); // U32
        in += read(in, &out->fcPlcfhdrtxbxTxt); // U32
        in += read(in, &out->lcbPlcfhdrtxbxTxt); // U32
        in += read(in, &out->fcPlcffldHdrTxbx); // U32
        in += read(in, &out->lcbPlcffldHdrTxbx); // U32
        in += read(in, &out->fcStwUser); // U32
        in += read(in, &out->lcbStwUser); // U32
        in += read(in, &out->fcSttbttmbd); // U32
        in += read(in, &out->cbSttbttmbd); // U32
        in += read(in, &out->fcUnused); // U32
        in += read(in, &out->lcbUnused); // U32
        in += read(in, &out->fcPgdMother); // U32
        in += read(in, &out->lcbPgdMother); // U32
        in += read(in, &out->fcBkdMother); // U32
        in += read(in, &out->lcbBkdMother); // U32
        in += read(in, &out->fcPgdFtn); // U32
        in += read(in, &out->lcbPgdFtn); // U32
        in += read(in, &out->fcBkdFtn); // U32
        in += read(in, &out->lcbBkdFtn); // U32
        in += read(in, &out->fcPgdEdn); // U32
        in += read(in, &out->lcbPgdEdn); // U32
        in += read(in, &out->fcBkdEdn); // U32
        in += read(in, &out->lcbBkdEdn); // U32
        in += read(in, &out->fcSttbfIntlFld); // U32
        in += read(in, &out->lcbSttbfIntlFld); // U32
        in += read(in, &out->fcRouteSlip); // U32
        in += read(in, &out->lcbRouteSlip); // U32
        in += read(in, &out->fcSttbSavedBy); // U32
        in += read(in, &out->lcbSttbSavedBy); // U32
        in += read(in, &out->fcSttbFnm); // U32
        in += read(in, &out->lcbSttbFnm); // U32
        in += read(in, &out->fcPlcfLst); // U32
        in += read(in, &out->lcbPlcfLst); // U32
        in += read(in, &out->fcPlfLfo); // U32
        in += read(in, &out->lcbPlfLfo); // U32
        in += read(in, &out->fcPlcftxbxBkd); // U32
        in += read(in, &out->lcbPlcftxbxBkd); // U32
        in += read(in, &out->fcPlcftxbxHdrBkd); // U32
        in += read(in, &out->lcbPlcftxbxHdrBkd); // U32
        in += read(in, &out->fcDocUndo); // U32
        in += read(in, &out->lcbDocUndo); // U32
        in += read(in, &out->fcRgbuse); // U32
        in += read(in, &out->lcbRgbuse); // U32
        in += read(in, &out->fcUsp); // U32
        in += read(in, &out->lcbUsp); // U32
        in += read(in, &out->fcUskf); // U32
        in += read(in, &out->lcbUskf); // U32
        in += read(in, &out->fcPlcupcRgbuse); // U32
        in += read(in, &out->lcbPlcupcRgbuse); // U32
        in += read(in, &out->fcPlcupcUsp); // U32
        in += read(in, &out->lcbPlcupcUsp); // U32
        in += read(in, &out->fcSttbGlsyStyle); // U32
        in += read(in, &out->lcbSttbGlsyStyle); // U32
        in += read(in, &out->fcPlgosl); // U32
        in += read(in, &out->lcbPlgosl); // U32
        in += read(in, &out->fcPlcocx); // U32
        in += read(in, &out->lcbPlcocx); // U32
        in += read(in, &out->fcPlcfbteLvc); // U32
        in += read(in, &out->lcbPlcfbteLvc); // U32
        in += read(in, &out->dwLowDateTime); // U32
        in += read(in, &out->dwHighDateTime); // U32
        in += read(in, &out->fcPlcflvc); // U32
        in += read(in, &out->lcbPlcflvc); // U32
        in += read(in, &out->fcPlcasumy); // U32
        in += read(in, &out->lcbPlcasumy); // U32
        in += read(in, &out->fcPlcfgram); // U32
        in += read(in, &out->lcbPlcfgram); // U32
        in += read(in, &out->fcSttbListNames); // U32
        in += read(in, &out->lcbSttbListNames); // U32
        in += read(in, &out->fcSttbfUssr); // U32
        in += read(in, &out->lcbSttbfUssr); // U32
        out++;
    }
    return count * sizeof(FIB);
} // FIB

unsigned MsWordGenerated::read(const U8 *in, FIBFCLCB *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, &out->fc); // U32
        in += read(in, &out->lcb); // U32
        out++;
    }
    return count * sizeof(FIBFCLCB);
} // FIBFCLCB

unsigned MsWordGenerated::read(const U8 *in, FRD *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, &out->nAuto); // U16
        out++;
    }
    return count * sizeof(FRD);
} // FRD

//unsigned MsWordGenerated::read(const U8 *in, CHPXFKP *out, unsigned count)
//{
//    unsigned long shiftRegister;
//
//    for (unsigned i = 0; i < count; i++)
//    {
//        in += read(in, &out->rgfc[0], ); // FC
//        in += read(in, &out->rgb[0], ); // U8
//        in += read(in, &out->unusedSpace[0], ); // U8
//        in += read(in, &out->grpchpx[0], ); // U8
//        in += read(in, &out->crun); // U8
//        out++;
//    }
//    return count * sizeof(CHPXFKP);
//} // CHPXFKP

//unsigned MsWordGenerated::read(const U8 *in, PAPXFKP *out, unsigned count)
//{
//    unsigned long shiftRegister;
//
//    for (unsigned i = 0; i < count; i++)
//    {
//        in += read(in, &out->rgfc[0], fkp.crun+1); // FC
//        in += read(in, &out->rgbx[0], fkp.crun); // BX
//        in += read(in, &out->grppapx[0], ); // U8
//        in += read(in, &out->crun); // U8
//        out++;
//    }
//    return count * sizeof(PAPXFKP);
//} // PAPXFKP

unsigned MsWordGenerated::read(const U8 *in, LVLF *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, &out->iStartAt); // U32
        in += read(in, &out->nfc); // U8
        in += read(in, (U8 *)&shiftRegister);
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
        in += read(in, &out->rgbxchNums[0], 9); // U8
        in += read(in, &out->ixchFollow); // U8
        in += read(in, &out->dxaSpace); // U32
        in += read(in, &out->dxaIndent); // U32
        in += read(in, &out->cbGrpprlChpx); // U8
        in += read(in, &out->cbGrpprlPapx); // U8
        in += read(in, &out->reserved); // U16
        out++;
    }
    return count * sizeof(LVLF);
} // LVLF

unsigned MsWordGenerated::read(const U8 *in, LSPD *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, &out->dyaLine); // U16
        in += read(in, &out->fMultLinespace); // U16
        out++;
    }
    return count * sizeof(LSPD);
} // LSPD

unsigned MsWordGenerated::read(const U8 *in, LSTF *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, &out->lsid); // U32
        in += read(in, &out->tplc); // U32
        in += read(in, &out->rgistd[0], 9); // U16
        in += read(in, (U8 *)&shiftRegister);
        out->fSimpleList = shiftRegister;
        shiftRegister >>= 1;
        out->fRestartHdn = shiftRegister;
        shiftRegister >>= 1;
        out->unsigned26_2 = shiftRegister;
        shiftRegister >>= 6;
        in += read(in, &out->reserved); // U8
        out++;
    }
    return count * sizeof(LSTF);
} // LSTF

unsigned MsWordGenerated::read(const U8 *in, LFO *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, &out->lsid); // U32
        in += read(in, &out->unused4); // U32
        in += read(in, &out->unused8); // U32
        in += read(in, &out->clfolvl); // U8
        in += read(in, &out->reserved[0], 3); // U8
        out++;
    }
    return count * sizeof(LFO);
} // LFO

unsigned MsWordGenerated::read(const U8 *in, LFOLVL *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, &out->iStartAt); // U32
        in += read(in, (U8 *)&shiftRegister);
        out->ilvl = shiftRegister;
        shiftRegister >>= 4;
        out->fStartAt = shiftRegister;
        shiftRegister >>= 1;
        out->fFormatting = shiftRegister;
        shiftRegister >>= 1;
        out->unsigned4_6 = shiftRegister;
        shiftRegister >>= 2;
        in += read(in, &out->reserved[0], 3); // U8
        out++;
    }
    return count * sizeof(LFOLVL);
} // LFOLVL

unsigned MsWordGenerated::read(const U8 *in, OLST *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, &out->rganlv[9]); // ANLV
        in += read(in, &out->fRestartHdr); // U8
        in += read(in, &out->fSpareOlst2); // U8
        in += read(in, &out->fSpareOlst3); // U8
        in += read(in, &out->fSpareOlst4); // U8
        in += read(in, &out->rgxch[0], 32); // XCHAR
        out++;
    }
    return count * sizeof(OLST);
} // OLST

unsigned MsWordGenerated::read(const U8 *in, NUMRM *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, &out->fNumRM); // U8
        in += read(in, &out->unused1); // U8
        in += read(in, &out->ibstNumRM); // U16
        in += read(in, &out->dttmNumRM); // DTTM
        in += read(in, &out->rgbxchNums[0], 9); // U8
        in += read(in, &out->rgnfc[0], 9); // U8
        in += read(in, &out->unused26); // U16
        in += read(in, &out->PNBR[0], 9); // U32
        in += read(in, &out->xst[0], 32); // XCHAR
        out++;
    }
    return count * sizeof(NUMRM);
} // NUMRM

unsigned MsWordGenerated::read(const U8 *in, PGD *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, (U16 *)&shiftRegister);
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
        in += read(in, &out->lnn); // U16
        in += read(in, &out->pgn); // U16
        in += read(in, &out->dym); // U32
        out++;
    }
    return count * sizeof(PGD);
} // PGD

unsigned MsWordGenerated::read(const U8 *in, PHE *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, (U16 *)&shiftRegister);
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
        in += read(in, &out->unused2); // U16
        in += read(in, &out->dxaCol); // U32
        in += read(in, &out->dym); // U32
        out++;
    }
    return count * sizeof(PHE);
} // PHE

unsigned MsWordGenerated::read(const U8 *in, PAP *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, &out->istd); // U16
        in += read(in, &out->jc); // U8
        in += read(in, &out->fKeep); // U8
        in += read(in, &out->fKeepFollow); // U8
        in += read(in, &out->fPageBreakBefore); // U8
        in += read(in, (U8 *)&shiftRegister);
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
        in += read(in, &out->brcp); // U8
        in += read(in, &out->brcl); // U8
        in += read(in, &out->unused9); // U8
        in += read(in, &out->ilvl); // U8
        in += read(in, &out->fNoLnn); // U8
        in += read(in, &out->ilfo); // U16
        in += read(in, &out->nLvlAnm); // U8
        in += read(in, &out->unused15); // U8
        in += read(in, &out->fSideBySide); // U8
        in += read(in, &out->unused17); // U8
        in += read(in, &out->fNoAutoHyph); // U8
        in += read(in, &out->fWidowControl); // U8
        in += read(in, &out->dxaRight); // U32
        in += read(in, &out->dxaLeft); // U32
        in += read(in, &out->dxaLeft1); // U32
        in += read(in, &out->lspd); // LSPD
        in += read(in, &out->dyaBefore); // U32
        in += read(in, &out->dyaAfter); // U32
        in += read(in, &out->phe); // PHE
        in += read(in, &out->fCrLf); // U8
        in += read(in, &out->fUsePgsuSettings); // U8
        in += read(in, &out->fAdjustRight); // U8
        in += read(in, &out->unused59); // U8
        in += read(in, &out->fKinsoku); // U8
        in += read(in, &out->fWordWrap); // U8
        in += read(in, &out->fOverflowPunct); // U8
        in += read(in, &out->fTopLinePunct); // U8
        in += read(in, &out->fAutoSpaceDE); // U8
        in += read(in, &out->fAtuoSpaceDN); // U8
        in += read(in, &out->wAlignFont); // U16
        in += read(in, (U16 *)&shiftRegister);
        out->fVertical = shiftRegister;
        shiftRegister >>= 1;
        out->fBackward = shiftRegister;
        shiftRegister >>= 1;
        out->fRotateFont = shiftRegister;
        shiftRegister >>= 1;
        out->unused68_3 = shiftRegister;
        shiftRegister >>= 13;
        in += read(in, &out->unused70); // U16
        in += read(in, &out->fInTable); // U8
        in += read(in, &out->fTtp); // U8
        in += read(in, &out->wr); // U8
        in += read(in, &out->fLocked); // U8
        in += read(in, &out->ptap); // U32
        in += read(in, &out->dxaAbs); // U32
        in += read(in, &out->dyaAbs); // U32
        in += read(in, &out->dxaWidth); // U32
        in += read(in, &out->brcTop); // BRC
        in += read(in, &out->brcLeft); // BRC
        in += read(in, &out->brcBottom); // BRC
        in += read(in, &out->brcRight); // BRC
        in += read(in, &out->brcBetween); // BRC
        in += read(in, &out->brcBar); // BRC
        in += read(in, &out->dxaFromText); // U32
        in += read(in, &out->dyaFromText); // U32
        in += read(in, (U16 *)&shiftRegister);
        out->dyaHeight = shiftRegister;
        shiftRegister >>= 15;
        out->fMinHeight = shiftRegister;
        shiftRegister >>= 1;
        in += read(in, &out->shd); // SHD
        in += read(in, &out->dcs); // DCS
        in += read(in, &out->lvl); // U8
        in += read(in, &out->fNumRMIns); // U8
        in += read(in, &out->anld); // ANLD
        in += read(in, &out->fPropRMark); // U16
        in += read(in, &out->ibstPropRMark); // U16
        in += read(in, &out->dttmPropRMark); // DTTM
        in += read(in, &out->numrm); // NUMRM
        in += read(in, &out->itbdMac); // U16
        in += read(in, &out->rgdxaTab[0], 64); // U16
        in += read(in, &out->rgtbd[0], 64); // U16
        out++;
    }
    return count * sizeof(PAP);
} // PAP

//unsigned MsWordGenerated::read(const U8 *in, PAPX *out, unsigned count)
//{
//    unsigned long shiftRegister;
//
//    for (unsigned i = 0; i < count; i++)
//    {
//        in += read(in, &out->cb); // U8
//        in += read(in, &out->cw); // U8
//        in += read(in, &out->(cw)); // U8
//        in += read(in, &out->istd); // U16
//        in += read(in, &out->grpprl); // character array
//        out++;
//    }
//    return count * sizeof(PAPX);
//} // PAPX

unsigned MsWordGenerated::read(const U8 *in, PICF *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, &out->lcb); // U32
        in += read(in, &out->cbHeader); // U16
        in += read(in, &out->mfp_mm); // U16
        in += read(in, &out->mfp_xExt); // U16
        in += read(in, &out->mfp_yExt); // U16
        in += read(in, &out->mfp_hMF); // U16
        out++;
    }
    return count * sizeof(PICF);
} // PICF

unsigned MsWordGenerated::read(const U8 *in, PCD *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, (U16 *)&shiftRegister);
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
        in += read(in, &out->fc); // U32
        in += read(in, &out->prm); // PRM
        out++;
    }
    return count * sizeof(PCD);
} // PCD

//unsigned MsWordGenerated::read(const U8 *in, PLCF *out, unsigned count)
//{
//    unsigned long shiftRegister;
//
//    for (unsigned i = 0; i < count; i++)
//    {
//        in += read(in, &out->rgfc[0], ); // FC
//        in += read(in, &out->rgstruct[0], ); // U8
//        out++;
//    }
//    return count * sizeof(PLCF);
//} // PLCF

unsigned MsWordGenerated::read(const U8 *in, PRM2 *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, (U16 *)&shiftRegister);
        out->fComplex = shiftRegister;
        shiftRegister >>= 1;
        out->igrpprl = shiftRegister;
        shiftRegister >>= 15;
        out++;
    }
    return count * sizeof(PRM2);
} // PRM2

unsigned MsWordGenerated::read(const U8 *in, RS *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, &out->fRouted); // U16
        in += read(in, &out->fReturnOrig); // U16
        in += read(in, &out->fTrackStatus); // U16
        in += read(in, &out->fDirty); // U16
        in += read(in, &out->nProtect); // U16
        in += read(in, &out->iStage); // U16
        in += read(in, &out->delOption); // U16
        in += read(in, &out->cRecip); // U16
        out++;
    }
    return count * sizeof(RS);
} // RS

unsigned MsWordGenerated::read(const U8 *in, RR *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, &out->cb); // U16
        in += read(in, &out->cbSzRecip); // U16
        out++;
    }
    return count * sizeof(RR);
} // RR

unsigned MsWordGenerated::read(const U8 *in, SED *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, &out->fn); // U16
        in += read(in, &out->fcSepx); // U32
        in += read(in, &out->fnMpr); // U16
        in += read(in, &out->fcMpr); // U32
        out++;
    }
    return count * sizeof(SED);
} // SED

unsigned MsWordGenerated::read(const U8 *in, SEP *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, &out->bkc); // U8
        in += read(in, &out->fTitlePage); // U8
        in += read(in, &out->fAutoPgn); // U8
        in += read(in, &out->nfcPgn); // U8
        in += read(in, &out->fUnlocked); // U8
        in += read(in, &out->cnsPgn); // U8
        in += read(in, &out->fPgnRestart); // U8
        in += read(in, &out->fEndNote); // U8
        in += read(in, &out->lnc); // U8
        in += read(in, &out->grpfIhdt); // U8
        in += read(in, &out->nLnnMod); // U16
        in += read(in, &out->dxaLnn); // U32
        in += read(in, &out->dxaPgn); // U16
        in += read(in, &out->dyaPgn); // U16
        in += read(in, &out->fLBetween); // U8
        in += read(in, &out->vjc); // U8
        in += read(in, &out->dmBinFirst); // U16
        in += read(in, &out->dmBinOther); // U16
        in += read(in, &out->dmPaperReq); // U16
        in += read(in, &out->brcTop); // BRC
        in += read(in, &out->brcLeft); // BRC
        in += read(in, &out->brcBottom); // BRC
        in += read(in, &out->brcRight); // BRC
        in += read(in, &out->fPropRMark); // U16
        in += read(in, &out->ibstPropRMark); // U16
        in += read(in, &out->dttmPropRMark); // DTTM
        in += read(in, &out->dxtCharSpace); // U32
        in += read(in, &out->dyaLinePitch); // U32
        in += read(in, &out->clm); // U16
        in += read(in, &out->unused62); // U16
        in += read(in, &out->dmOrientPage); // U8
        in += read(in, &out->iHeadingPgn); // U8
        in += read(in, &out->pgnStart); // U16
        in += read(in, &out->lnnMin); // U16
        in += read(in, &out->wTextFlow); // U16
        in += read(in, &out->unused72); // U16
        in += read(in, &out->pgbProp); // U16
        in += read(in, (U16 *)&shiftRegister);
        out->pgbApplyTo = shiftRegister;
        shiftRegister >>= 3;
        out->pgbPageDepth = shiftRegister;
        shiftRegister >>= 2;
        out->pgbOffsetFrom = shiftRegister;
        shiftRegister >>= 3;
        in += read(in, &out->xaPage); // U32
        in += read(in, &out->yaPage); // U32
        in += read(in, &out->xaPageNUp); // U32
        in += read(in, &out->yaPageNUp); // U32
        in += read(in, &out->dxaLeft); // U32
        in += read(in, &out->dxaRight); // U32
        in += read(in, &out->dyaTop); // U32
        in += read(in, &out->dyaBottom); // U32
        in += read(in, &out->dzaGutter); // U32
        in += read(in, &out->dyaHdrTop); // U32
        in += read(in, &out->dyaHdrBottom); // U32
        in += read(in, &out->ccolM1); // U16
        in += read(in, &out->fEvenlySpaced); // U8
        in += read(in, &out->unused123); // U8
        in += read(in, &out->dxaColumns); // U32
        in += read(in, &out->rgdxaColumnWidthSpacing[0], 89); // U32
        in += read(in, &out->dxaColumnWidth); // U32
        in += read(in, &out->dmOrientFirst); // U8
        in += read(in, &out->fLayout); // U8
        in += read(in, &out->unused490); // U16
        in += read(in, &out->olstAnm); // OLST
        out++;
    }
    return count * sizeof(SEP);
} // SEP

//unsigned MsWordGenerated::read(const U8 *in, SEPX *out, unsigned count)
//{
//    unsigned long shiftRegister;
//
//    for (unsigned i = 0; i < count; i++)
//    {
//        in += read(in, &out->cb); // U16
//        in += read(in, &out->grpprl[0], 0); // U8
//        out++;
//    }
//    return count * sizeof(SEPX);
//} // SEPX

unsigned MsWordGenerated::read(const U8 *in, STSHI *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, &out->cstd); // U16
        in += read(in, &out->cbSTDBaseInFile); // U16
        in += read(in, (U16 *)&shiftRegister);
        out->fStdStylenamesWritten = shiftRegister;
        shiftRegister >>= 1;
        out->unused4_2 = shiftRegister;
        shiftRegister >>= 15;
        in += read(in, &out->stiMaxWhenSaved); // U16
        in += read(in, &out->istdMaxFixedWhenSaved); // U16
        in += read(in, &out->nVerBuiltInNamesWhenSaved); // U16
        in += read(in, &out->rgftcStandardChpStsh[0], 3); // U16
        out++;
    }
    return count * sizeof(STSHI);
} // STSHI

//unsigned MsWordGenerated::read(const U8 *in, STD *out, unsigned count)
//{
//    unsigned long shiftRegister;
//
//    for (unsigned i = 0; i < count; i++)
//    {
//        in += read(in, (U16 *)&shiftRegister);
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
//        in += read(in, (U16 *)&shiftRegister);
//        out->sgc = shiftRegister;
//        shiftRegister >>= 4;
//        out->istdBase = shiftRegister;
//        shiftRegister >>= 12;
//        in += read(in, (U16 *)&shiftRegister);
//        out->cupx = shiftRegister;
//        shiftRegister >>= 4;
//        out->istdNext = shiftRegister;
//        shiftRegister >>= 12;
//        in += read(in, &out->bchUpe); // U16
//        in += read(in, (U16 *)&shiftRegister);
//        out->fAutoRedef = shiftRegister;
//        shiftRegister >>= 1;
//        out->fHidden = shiftRegister;
//        shiftRegister >>= 1;
//        out->unused8_3 = shiftRegister;
//        shiftRegister >>= 14;
//        in += read(in, &out->xstzName[2]); // XCHAR
//        out++;
//    }
//    return count * sizeof(STD);
//} // STD

unsigned MsWordGenerated::read(const U8 *in, TBD *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, (U8 *)&shiftRegister);
        out->jc = shiftRegister;
        shiftRegister >>= 3;
        out->tlc = shiftRegister;
        shiftRegister >>= 3;
        out->unused0_6 = shiftRegister;
        shiftRegister >>= 2;
        out++;
    }
    return count * sizeof(TBD);
} // TBD

unsigned MsWordGenerated::read(const U8 *in, TC *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, (U16 *)&shiftRegister);
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
        in += read(in, &out->wUnused); // U16
        in += read(in, &out->brcTop); // BRC
        in += read(in, &out->brcLeft); // BRC
        in += read(in, &out->brcBottom); // BRC
        in += read(in, &out->brcRight); // BRC
        out++;
    }
    return count * sizeof(TC);
} // TC

unsigned MsWordGenerated::read(const U8 *in, TLP *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, &out->itl); // U16
        in += read(in, (U16 *)&shiftRegister);
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
    return count * sizeof(TLP);
} // TLP

unsigned MsWordGenerated::read(const U8 *in, TAP *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, &out->jc); // U16
        in += read(in, &out->dxaGapHalf); // U32
        in += read(in, &out->dyaRowHeight); // U32
        in += read(in, &out->fCantSplit); // U8
        in += read(in, &out->fTableHeader); // U8
        in += read(in, &out->tlp); // TLP
        in += read(in, &out->lwHTMLProps); // U32
        in += read(in, (U16 *)&shiftRegister);
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
        in += read(in, &out->itcMac); // U16
        in += read(in, &out->dxaAdjust); // U32
        in += read(in, &out->dxaScale); // U32
        in += read(in, &out->dxsInch); // U32
        in += read(in, &out->rgdxaCenter[0], 65); // S16
        in += read(in, &out->rgdxaCenterPrint[0], 65); // U16
        in += read(in, &out->rgtc[0], 64); // TC
        in += read(in, &out->rgshd[0], 64); // SHD
        in += read(in, &out->rgbrcTable[0], 6); // BRC
        out++;
    }
    return count * sizeof(TAP);
} // TAP

unsigned MsWordGenerated::read(const U8 *in, FTXBXS *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, &out->cTxbx); // U32
        in += read(in, &out->iNextReuse); // U32
        in += read(in, &out->cReusable); // U32
        in += read(in, &out->fReusable); // U16
        in += read(in, &out->lid); // U32
        in += read(in, &out->txidUndo); // U32
        out++;
    }
    return count * sizeof(FTXBXS);
} // FTXBXS

unsigned MsWordGenerated::read(const U8 *in, WKB *out, unsigned count)
{
    unsigned long shiftRegister;

    for (unsigned i = 0; i < count; i++)
    {
        in += read(in, &out->fn); // U16
        in += read(in, &out->grfwkb); // U16
        in += read(in, &out->lvl); // U16
        in += read(in, (U16 *)&shiftRegister);
        out->fnpt = shiftRegister;
        shiftRegister >>= 4;
        out->fnpd = shiftRegister;
        shiftRegister >>= 12;
        in += read(in, &out->doc); // U32
        out++;
    }
    return count * sizeof(WKB);
} // WKB

