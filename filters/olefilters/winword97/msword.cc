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

    The code in this file is mostly derived from the Microsoft specifications,
    but some of the workarounds for the broken specifications etc. come from
    the pioneering work on the "wvWare" library by Caolan McNamara (see
    http://www.wvWare.com).
*/

#include <kdebug.h>
#include <msword.h>
#include <string.h>

short MsWord::char2unicode(unsigned char c)
{
    static const short CP2UNI[] =
    {
        0x20ac, 0x0000, 0x201a, 0x0192,
        0x201e, 0x2026, 0x2020, 0x2021,
        0x02c6, 0x2030, 0x0160, 0x2039,
        0x0152, 0x0000, 0x017d, 0x0000,
        0x0000, 0x2018, 0x2019, 0x201c,
        0x201d, 0x2022, 0x2013, 0x2014,
        0x02dc, 0x2122, 0x0161, 0x203a,
        0x0153, 0x0000, 0x017e, 0x0178
    };

    if (c <= 0x7f || c >= 0xa0)
        return static_cast<short>(c);
    else
        return CP2UNI[c-0x80];
}

void MsWord::constructionError(unsigned line, const char *reason)
{
    m_constructionError.sprintf("[" __FILE__ ":%u] %s", line, reason);
    kdError(s_area) << m_constructionError << endl;
}

void MsWord::decodeParagraph(const QString &text, PHE &layout, PAPXFKP &style)
{
    PAP pap;

    // Work out the paragraph details.

    paragraphStyleCreate(&pap);
    paragraphStyleModify(&pap, style);
    paragraphStyleModify(&pap, layout);

    // We treat table paragraphs somewhat differently...so edal with
    // them first.

    if (pap.fInTable)
    {
        if (!m_wasInTable)
        {
            gotTableBegin();
            m_tableColumn = 0;
        }
        m_wasInTable = true;

        // When we get to the end of the row, output the whole lot.

        if (pap.fTtp)
        {
            TAP tap;

            // A TAP describes the row.

            memset(&tap, 0, sizeof(tap));
            paragraphStyleModify(&pap, &tap, style.grpprl, style.grpprlBytes);
            gotTableRow(m_tableText, m_tableStyle, tap);
            m_tableColumn = 0;
        }
        else
        {
            m_tableText[m_tableColumn] = text;
            m_tableStyle[m_tableColumn] = pap;
            m_tableColumn++;
        }
        return;
    }
    if (m_wasInTable)
        gotTableEnd();
    m_wasInTable = false;

    // What kind of paragraph was this?

    if ((pap.istd >= 1) && (pap.istd <= 9))
    {
        gotHeadingParagraph(text, pap);
    }
    else
    if (pap.ilfo)
    {
        const U8 *ptr = m_tableStream + m_fib.fcPlfLfo; //lcbPlfLfo.
        const U8 *ptr2;
        const U8 *ptr3;
        U32 lfoCount;
        int i;

        // Find the number of LFOs.

        ptr += MsWordGenerated::read(ptr, &lfoCount);
        ptr2 = ptr + lfoCount * sizeof(LFO);
        if (lfoCount < pap.ilfo)
            kdError(s_area) << "MsWord::error finding LFO[" << pap.ilfo << "]" << endl;

        // Skip all the LFOs before our one, so that we can traverse the variable
        // length LFOLVL arrays.

        for (i = 1; i < pap.ilfo; i++)
        {
            LFO data;
            LFOLVL levelOverride;
            LVLF level;
            U16 numberTextLength;
            QString numberText;

            // Read the LFO, and then skip any LFOLVLs.

            ptr += MsWordGenerated::read(ptr, &data);
            for (unsigned j = 0; j < data.clfolvl; j++)
            {
                ptr2 += MsWordGenerated::read(ptr2, &levelOverride);
                if (levelOverride.fFormatting)
                {
                    ptr2 += MsWordGenerated::read(ptr2, &level);
                    ptr3 = ptr2;
                    ptr2 += level.cbGrpprlPapx;
                    ptr2 += level.cbGrpprlChpx;
                    ptr2 += MsWordGenerated::read(ptr2, &numberTextLength);
                    ptr2 += read(ptr2, &numberText, numberTextLength, true);
                }
            }
        }

        // We have found the LFO from its 1-based array. Check to see if there are any overrides for this particular level.

        LFO data;
        LFOLVL levelOverride;
        LVLF level;
        U16 numberTextLength;
        QString numberText;

        // Read our LFO, and then search any LFOLVLs for a matching level.

        ptr += MsWordGenerated::read(ptr, &data);
        for (i = 0; i < data.clfolvl; i++)
        {
            ptr2 += MsWordGenerated::read(ptr2, &levelOverride);
            if (levelOverride.fFormatting)
            {
                ptr2 += MsWordGenerated::read(ptr2, &level);
                ptr3 = ptr2;
                ptr2 += level.cbGrpprlPapx;
                ptr2 += level.cbGrpprlChpx;
                ptr2 += MsWordGenerated::read(ptr2, &numberTextLength);
                ptr2 += read(ptr2, &numberText, numberTextLength, true);
            }

            // If this LFOLVL is ours, we are done!

            if (pap.ilvl == levelOverride.ilvl)
            {
                break;
            };
        }
        if (i == data.clfolvl)
        {
            // No overriding LFOLVL was found.

            levelOverride.fFormatting = false;
            levelOverride.fStartAt = false;
        }

        // If the LFOLVL was not a complete override, resort to the LSTs for whatever
        // is missing.

        paragraphStyleModify(&pap, data, !levelOverride.fFormatting, !levelOverride.fStartAt);
        if (levelOverride.fStartAt)
        {
            // Apply the startAt.

            pap.anld.iStartAt = levelOverride.iStartAt;
            kdDebug(s_area) << "got startAt " << pap.anld.iStartAt << " from LFOLVL" << endl;
        }
        if (levelOverride.fFormatting)
        {
            // Apply the grpprl.

            kdDebug(s_area) << "getting formatting from LFO" << endl;
            paragraphStyleModify(&pap, NULL, ptr3, level.cbGrpprlPapx);

            // Apply the startAt.

            pap.anld.iStartAt = level.iStartAt;
            kdDebug(s_area) << "got startAt " << pap.anld.iStartAt << " from LVLF" << endl;
        }
        gotListParagraph(text, pap);
    }
    else
    {
        gotParagraph(text, pap);
    }
}

template <class T1, class T2>
MsWord::Fkp<T1, T2>::Fkp(FIB &fib) :
    m_fib(fib)
{
};

//
// Get the next entry in an FKP.
//
template <class T1, class T2>
bool MsWord::Fkp<T1, T2>::getNext(
    U32 *startFc,
    U32 *endFc,
    U8 *rgb,
    T1 *data1,
    T2 *data2)
{
    // Sanity check accesses beyond end of Fkp.

    if (m_i >= m_crun)
    {
        return false;
    }

    // Get fc range.

    m_fcNext += MsWordGenerated::read(m_fcNext, startFc);
    MsWordGenerated::read(m_fcNext, endFc);

    // Get word offset to the second piece of data, and the first piece of data.

    m_dataNext += MsWordGenerated::read(m_dataNext, rgb);
    m_dataNext += read(m_fib.nFib, m_dataNext, data1);

    // If the word offset is zero, then the second piece of data is not explicitly
    // stored.

    if (!(*rgb))
    {
        kdDebug(s_area) << "MsWord::Fkp::getNext: " << *startFc << ":" << endFc
                << ": default PAPX/CHPX, rgb: " << *rgb << endl;
    }
    else
    {
        // Get the second piece of data.
        MsWord::read(m_fib.nFib, m_fkp + (2 * (*rgb)), data2);
    }
    return (m_i++ < m_crun);
}

template <class T1, class T2>
void MsWord::Fkp<T1, T2>::startIteration(const U8 *fkp)
{
    U32 startFc;

    // Get the number of entries in the FKP.

    m_fkp = fkp;
    MsWordGenerated::read(m_fkp + 511, &m_crun);
    m_fcNext = m_fkp;
    m_dataNext = m_fkp + ((m_crun + 1) * sizeof(startFc));
    m_i = 0;
}

void MsWord::getPAPXFKP(const U8 *textStartFc, U32 textLength, bool unicode)
{
    // A bin table is a plex of BTEs.

    Plex<BTE, 2> btes = Plex<BTE, 2>(m_fib);
    U32 startFc;
    U32 endFc;
    BTE data;

    // Walk the BTEs.

    btes.startIteration(m_tableStream + m_fib.fcPlcfbtePapx, m_fib.lcbPlcfbtePapx);
    while (btes.getNext(&startFc, &endFc, &data))
    {
        getPAPX(
            m_mainStream + (data.pn * 512),
            textStartFc,
            textLength,
            unicode);
    }
}

void MsWord::getPAPX(
    const U8 *fkp,
    const U8 *textStartFc,
    U32 textLength,
    bool unicode)
{
    // A PAPX FKP contains PHEs.

    Fkp<PHE, PAPXFKP> papx = Fkp<PHE, PAPXFKP>(m_fib);

    U32 startFc;
    U32 endFc;
    U8 rgb;
    PHE layout;
    PAPXFKP style;

    papx.startIteration(fkp);
    while (papx.getNext(&startFc, &endFc, &rgb, &layout, &style))
    {
        QString text;

        read(m_mainStream + startFc, &text, endFc - startFc, unicode);
        decodeParagraph(text, layout, style);
    }
}

// Create a cache of information about lists.
//
//    m_listLevels: an array of arrays of pointers to LVLFs for each list style in the
//    LST array. The entries must be looked up using the list id and list level.

void MsWord::getListStyles()
{
    const U8 *ptr = m_tableStream + m_fib.fcPlcfLst; //lcbPlcfLst.
    const U8 *ptr2;
    U16 lstfCount;

    // Failsafe for simple documents.

    m_listStyles = NULL;
    if (!m_fib.lcbPlcfLst)
    {
        kdDebug(s_area) << "MsWord::getListStyles: no data " << endl;
        return;
    }

    // Find the number of LSTFs.

    ptr += MsWordGenerated::read(ptr, &lstfCount);
    ptr2 = ptr + lstfCount * sizeof(LSTF);

    // Construct the array of styles, and then walk the array reading in the style definitions.

    m_listStyles = new LVLF **[lstfCount];
    for (unsigned i = 0; i < lstfCount; i++)
    {
        LSTF data;
        unsigned levelCount;

        ptr += MsWordGenerated::read(ptr, &data);
        if (data.fSimpleList)
            levelCount = 1;
        else
            levelCount = 9;

        // Create an array of LVLF pointers, one for each level in the list.

        m_listStyles[i] = new LVLF *[levelCount];
        for (unsigned j = 0; j < levelCount; j++)
        {
            m_listStyles[i][j] = (LVLF *)ptr2;

            // Skip the variable length parts.

            LVLF level;
            U16 numberTextLength;
            QString numberText;

            ptr2 += MsWordGenerated::read(ptr2, &level);
            ptr2 += level.cbGrpprlPapx;
            ptr2 += level.cbGrpprlChpx;
            ptr2 += MsWordGenerated::read(ptr2, &numberTextLength);
            ptr2 += read(ptr2, &numberText, numberTextLength, true);
        }
    }
}

// Create a cache of information about built-in styles.
//
// The cache consists of:
//
//    m_styles: an array of fully-decoded PAPs for each built in style
//    indexed by istd.

void MsWord::getStyles()
{
    const U8 *ptr = m_tableStream + m_fib.fcStshf;
    U16 cbStshi;
    STSHI stshi;

    // Failsafe for simple documents.

    m_styles = NULL;
    if (!m_fib.lcbStshf)
    {
        kdError(s_area) << "MsWord::getListStyles: no data " << endl;
        return;
    }

    // Fetch the STSHI.

    ptr += MsWordGenerated::read(ptr, &cbStshi);
    if (cbStshi > sizeof(stshi))
    {
        kdError(s_area) << "MsWord::getStyles: unsupported STSHI size " << cbStshi << endl;
        return;
    }

    // We know that older/smaller STSHIs can simply be zero extended into our STSHI.
    // So, we overwrite anything that is not valid with zeros.

    ptr += MsWordGenerated::read(ptr, &stshi);
    memset(((char *)&stshi) + cbStshi, 0, sizeof(stshi) - cbStshi);
    ptr -= sizeof(stshi) - cbStshi;

    // Construct the array of styles, and then walk the array reading in the style definitions.

    m_styles = new PAP [stshi.cstd];
    for (unsigned i = 0; i < stshi.cstd; i++)
    {
        U16 cbStd;
        STD std;

        ptr += MsWordGenerated::read(ptr, &cbStd);
        if (cbStd)
        {
            read(ptr, stshi.cbSTDBaseInFile, &std);
            kdDebug(s_area) << "MsWord::getStyles: style: " << std.xstzName <<
                ", types: " << std.cupx <<
                endl;

            // If this is a paragraph style, fill it.

            if (std.sgc == 1)
            {
                paragraphStyleCreate(&m_styles[i]);
                paragraphStyleModify(&m_styles[i], std);
            }
        }
        else
        {
            // Set the style to be the same as stiNormal. This is a purely
            // defensive thing...and relies on a viable 0th entry.

            m_styles[i] = m_styles[0];
        }
        ptr += cbStd;
    }
}

void MsWord::gotParagraph(const QString &text, PAP &style)
{
    kdDebug(s_area) << "MsWord::gotParagraph: normal" << endl;
}

void MsWord::gotHeadingParagraph(const QString &text, PAP &style)
{
    kdDebug(s_area) << "MsWord::gotParagraph: heading level: " << style.istd << ": " << text << endl;
}

void MsWord::gotListParagraph(const QString &text, PAP &style)
{
    kdDebug(s_area) << "MsWord::gotParagraph: list level: " << style.ilvl << endl;
}

void MsWord::gotTableBegin()
{
    kdDebug(s_area) << "MsWord::gotParagraph: table begin" << endl;
}

void MsWord::gotTableEnd()
{
    kdDebug(s_area) << "MsWord::gotParagraph: table end" << endl;
}

void MsWord::gotTableRow(const QString texts[], const PAP styles[], TAP &row)
{
    kdDebug(s_area) << "MsWord::gotParagraph: table row: cells: " << row.itcMac << endl;
}

MsWord::MsWord(
        const U8 *mainStream,
        const U8 *table0Stream,
        const U8 *table1Stream,
        const U8 *dataStream)
{
    m_constructionError = QString("");
    m_fib.nFib = s_minWordVersion;
    read(mainStream, &m_fib);
    if (m_fib.nFib <= s_minWordVersion)
    {
        constructionError(__LINE__, "the document was created using an unsupported version of Word");
        return;
    }
    if (m_fib.fEncrypted)
    {
        constructionError(__LINE__, "the document is encrypted");
        return;
    }
    kdDebug(s_area) << "MsWord::MsWord: nFib: " << m_fib.nFib << endl;

    // Store away the streams for future use. Note that we do not
    // copy the contents of the streams, and that we rely on the storage
    // being present until we are destroyed.

    m_mainStream = mainStream;
    m_tableStream = m_fib.fWhichTblStm ? table1Stream : table0Stream;
    m_dataStream = dataStream;
    if (!m_tableStream)
    {
        // Older versions of Word had no separate table stream.

        kdDebug(s_area) << "MsWord::MsWord: no table stream" << endl;
        m_tableStream = m_mainStream;
    }
    if (!m_dataStream)
    {
        // Older versions of Word had no separate data stream.

        kdDebug(s_area) << "MsWord::MsWord: no data stream" << endl;
        m_dataStream = m_mainStream;
    }
    getStyles();
    getListStyles();
}

MsWord::~MsWord()
{
}

// Set PAP to its initial value.

void MsWord::paragraphStyleCreate(PAP *pap)
{
    memset(pap, 0, sizeof(*pap));
    pap->fWidowControl = 1;
    pap->lspd.fMultLinespace = 1;
    pap->lspd.dyaLine = 240;
    pap->lvl = 9;
}

// Apply a base style.

void MsWord::paragraphStyleModify(PAP *pap, unsigned style)
{
    unsigned originalStyle;

    // Record the style index.

    originalStyle = pap->istd;

    // Walk the grpprl, then restore the style index.

    *pap = m_styles[style];
    pap->istd = originalStyle;
}


// Apply a grpprl.

void MsWord::paragraphStyleModify(PAP *pap, TAP *tap, const U8 *grpprl, unsigned count)
{
    // Encodings of all Word97 sprms.

    typedef enum
    {
        sprmCFRMarkDel = 0x0800,
        sprmCFRMark = 0x0801,
        sprmCFFldVanish = 0x0802,
        sprmCFData = 0x0806,
        sprmCFOle2 = 0x080A,
        sprmCFBold = 0x0835,
        sprmCFItalic = 0x0836,
        sprmCFStrike = 0x0837,
        sprmCFOutline = 0x0838,
        sprmCFShadow = 0x0839,
        sprmCFSmallCaps = 0x083A,
        sprmCFCaps = 0x083B,
        sprmCFVanish = 0x083C,
        sprmCFImprint = 0x0854,
        sprmCFSpec = 0x0855,
        sprmCFObj = 0x0856,
        sprmCFEmboss = 0x0858,
        sprmCFBiDi = 0x085A,
        sprmCFDiacColor = 0x085B,
        sprmCFBoldBi = 0x085C,
        sprmCFItalicBi = 0x085D,
        sprmCFUsePgsuSettings = 0x0868,
        sprmPJc = 0x2403,
        sprmPFSideBySide = 0x2404,
        sprmPFKeep = 0x2405,
        sprmPFKeepFollow = 0x2406,
        sprmPFPageBreakBefore = 0x2407,
        sprmPBrcl = 0x2408,
        sprmPBrcp = 0x2409,
        sprmPFNoLineNumb = 0x240C,
        sprmPFInTable = 0x2416,
        sprmPFTtp = 0x2417,
        sprmPWr = 0x2423,
        sprmPFNoAutoHyph = 0x242A,
        sprmPFLocked = 0x2430,
        sprmPFWidowControl = 0x2431,
        sprmPFKinsoku = 0x2433,
        sprmPFWordWrap = 0x2434,
        sprmPFOverflowPunct = 0x2435,
        sprmPFTopLinePunct = 0x2436,
        sprmPFAutoSpaceDE = 0x2437,
        sprmPFAutoSpaceDN = 0x2438,
        sprmPISnapBaseLine = 0x243B,
        sprmPFBiDi = 0x2441,
        sprmPFNumRMIns = 0x2443,
        sprmPCrLf = 0x2444,
        sprmPFUsePgsuSettings = 0x2447,
        sprmPFAdjustRight = 0x2448,
        sprmPIncLvl = 0x2602,
        sprmPIlvl = 0x260A,
        sprmPPc = 0x261B,
        sprmPOutLvl = 0x2640,
        sprmCSfxText = 0x2859,
        sprmCIdctHint = 0x286F,
        sprmCHighlight = 0x2A0C,
        sprmCFFtcAsciSymb = 0x2A10,
        sprmCDefault = 0x2A32,
        sprmCPlain = 0x2A33,
        sprmCKcd = 0x2A34,
        sprmCKul = 0x2A3E,
        sprmCIco = 0x2A42,
        sprmCHpsInc = 0x2A44,
        sprmCHpsPosAdj = 0x2A46,
        sprmCIss = 0x2A48,
        sprmCFDStrike = 0x2A53,
        sprmPicBrcl = 0x2E00,
        sprmScnsPgn = 0x3000,
        sprmSiHeadingPgn = 0x3001,
        sprmSFEvenlySpaced = 0x3005,
        sprmSFProtected = 0x3006,
        sprmSBkc = 0x3009,
        sprmSFTitlePage = 0x300A,
        sprmSFAutoPgn = 0x300D,
        sprmSNfcPgn = 0x300E,
        sprmSFPgnRestart = 0x3011,
        sprmSFEndnote = 0x3012,
        sprmSLnc = 0x3013,
        sprmSGprfIhdt = 0x3014,
        sprmSLBetween = 0x3019,
        sprmSVjc = 0x301A,
        sprmSBOrientation = 0x301D,
        sprmSBCustomize = 0x301E,
        sprmSFBiDi = 0x3228,
        sprmSFFacingCol = 0x3229,
        sprmSFRTLGutter = 0x322A,
        sprmTFCantSplit = 0x3403,
        sprmTTableHeader = 0x3404,
        sprmPWHeightAbs = 0x442B,
        sprmPDcs = 0x442C,
        sprmPShd = 0x442D,
        sprmPWAlignFont = 0x4439,
        sprmPFrameTextFlow = 0x443A,
        sprmPIstd = 0x4600,
        sprmPIlfo = 0x460B,
        sprmPNest = 0x4610,
        sprmPBrcTop10 = 0x461C,
        sprmPBrcLeft10 = 0x461D,
        sprmPBrcBottom10 = 0x461E,
        sprmPBrcRight10 = 0x461F,
        sprmPBrcBetween10 = 0x4620,
        sprmPBrcBar10 = 0x4621,
        sprmPDxaFromText10 = 0x4622,
        sprmCIbstRMark = 0x4804,
        sprmCIdslRMark = 0x4807,
        sprmCIdCharType = 0x480B,
        sprmCHpsPos = 0x4845,
        sprmCHpsKern = 0x484B,
        sprmCYsri = 0x484E,
        sprmCCharScale = 0x4852,
        sprmCLidBi = 0x485F,
        sprmCIbstRMarkDel = 0x4863,
        sprmCShd = 0x4866,
        sprmCIdslRMarkDel = 0x4867,
        sprmCCpg = 0x486B,
        sprmCRgLid0 = 0x486D,
        sprmCRgLid1 = 0x486E,
        sprmCIstd = 0x4A30,
        sprmCFtcDefault = 0x4A3D,
        sprmCLid = 0x4A41,
        sprmCHps = 0x4A43,
        sprmCHpsMul = 0x4A4D,
        sprmCRgFtc0 = 0x4A4F,
        sprmCRgFtc1 = 0x4A50,
        sprmCRgFtc2 = 0x4A51,
        sprmCFtcBi = 0x4A5E,
        sprmCIcoBi = 0x4A60,
        sprmCHpsBi = 0x4A61,
        sprmSDmBinFirst = 0x5007,
        sprmSDmBinOther = 0x5008,
        sprmSCcolumns = 0x500B,
        sprmSNLnnMod = 0x5015,
        sprmSLnnMin = 0x501B,
        sprmSPgnStart = 0x501C,
        sprmSDmPaperReq = 0x5026,
        sprmSClm = 0x5032,
        sprmSTextFlow = 0x5033,
        sprmSPgbProp = 0x522F,
        sprmTJc = 0x5400,
        sprmTFBiDi = 0x560B,
        sprmTDelete = 0x5622,
        sprmTMerge = 0x5624,
        sprmTSplit = 0x5625,
        sprmPDyaLine = 0x6412,
        sprmPBrcTop = 0x6424,
        sprmPBrcLeft = 0x6425,
        sprmPBrcBottom = 0x6426,
        sprmPBrcRight = 0x6427,
        sprmPBrcBetween = 0x6428,
        sprmPBrcBar = 0x6629,
        sprmPHugePapx = 0x6645,
        sprmCDttmRMark = 0x6805,
        sprmCObjLocation = 0x680E,
        sprmCDttmRMarkDel = 0x6864,
        sprmCBrc = 0x6865,
        sprmCPicLocation = 0x6A03,
        sprmCSymbol = 0x6A09,
        sprmPicBrcTop = 0x6C02,
        sprmPicBrcLeft = 0x6C03,
        sprmPicBrcBottom = 0x6C04,
        sprmPicBrcRight = 0x6C05,
        sprmSBrcTop = 0x702B,
        sprmSBrcLeft = 0x702C,
        sprmSBrcBottom = 0x702D,
        sprmSBrcRight = 0x702E,
        sprmSDxtCharSpace = 0x7030,
        sprmTTlp = 0x740A,
        sprmTHTMLProps = 0x740C,
        sprmTInsert = 0x7621,
        sprmTDxaCol = 0x7623,
        sprmTSetShd = 0x7627,
        sprmTSetShdOdd = 0x7628,
        sprmTTextFlow = 0x7629,
        sprmPDxaRight = 0x840E,
        sprmPDxaLeft = 0x840F,
        sprmPDxaLeft1 = 0x8411,
        sprmPDxaAbs = 0x8418,
        sprmPDyaAbs = 0x8419,
        sprmPDxaWidth = 0x841A,
        sprmPDyaFromText = 0x842E,
        sprmPDxaFromText = 0x842F,
        sprmCDxaSpace = 0x8840,
        sprmSDxaColumns = 0x900C,
        sprmSDxaLnn = 0x9016,
        sprmSDyaTop = 0x9023,
        sprmSDyaBottom = 0x9024,
        sprmSDyaLinePitch = 0x9031,
        sprmTDyaRowHeight = 0x9407,
        sprmTDxaLeft = 0x9601,
        sprmTDxaGapHalf = 0x9602,
        sprmPDyaBefore = 0xA413,
        sprmPDyaAfter = 0xA414,
        sprmSDyaPgn = 0xB00F,
        sprmSDxaPgn = 0xB010,
        sprmSDyaHdrTop = 0xB017,
        sprmSDyaHdrBottom = 0xB018,
        sprmSXaPage = 0xB01F,
        sprmSYaPage = 0xB020,
        sprmSDxaLeft = 0xB021,
        sprmSDxaRight = 0xB022,
        sprmSDzaGutter = 0xB025,
        sprmPIstdPermute = 0xC601,
        sprmPChgTabsPapx = 0xC60D,
        sprmPChgTabs = 0xC615,
        sprmPRuler = 0xC632,
        sprmPAnld = 0xC63E,
        sprmPPropRMark = 0xC63F,
        sprmPNumRM = 0xC645,
        sprmCIstdPermute = 0xCA31,
        sprmCMajority = 0xCA47,
        sprmCHpsNew50 = 0xCA49,
        sprmCHpsInc1 = 0xCA4A,
        sprmCMajority50 = 0xCA4C,
        sprmCPropRMark = 0xCA57,
        sprmCDispFldRMark = 0xCA62,
        sprmPicScale = 0xCE01,
        sprmSOlstAnm = 0xD202,
        sprmSPropRMark = 0xD227,
        sprmTTableBorders = 0xD605,
        sprmTDefTable10 = 0xD606,
        sprmTDefTable = 0xD608,
        sprmTDefTableShd = 0xD609,
        sprmTSetBrc = 0xD620,
        sprmTSetBrc10 = 0xD626,
        sprmTDiagLine = 0xD62A,
        sprmTVertMerge = 0xD62B,
        sprmTVertAlign = 0xD62C,
        sprmCChs = 0xEA08,
        sprmCSizePos = 0xEA3F,
        sprmSDxaColWidth = 0xF203,
        sprmSDxaColSpacing = 0xF204
    } opcodes;
    union
    {
        U16 value;
        struct
        {
            U16 ispmd: 9;
            U16 fSpec: 1;
            U16 sgc: 3;
            U16 spra: 3;
        } bits;
    } opcode;
    unsigned operandSize;
    unsigned operandSizes[8] =
    {
        1,
        1,
        2,
        4,
        2,
        2,
        0,
        3
    };
    const U8 *in = grpprl;
    unsigned bytes = 0;

    // Walk the grpprl.

    while (bytes < count)
    {
        if (m_fib.nFib > s_maxWord6Version)
        {
            bytes += MsWordGenerated::read(in + bytes, &opcode.value);
        }
        else
        {
            U8 sprm;

            // Convert sprm to new format.

            bytes += MsWordGenerated::read(in + bytes, &sprm);
            switch (sprm)
            {
            case 2:
                opcode.value = sprmPIstd;
                break;
            case 3:
                opcode.value = sprmPIstdPermute;
                break;
            case 4:
                opcode.value = sprmPIncLvl;
                break;
            case 5:
                opcode.value = sprmPJc;
                break;
            case 6:
                opcode.value = sprmPFSideBySide;
                break;
            case 7:
                opcode.value = sprmPFKeep;
                break;
            case 8:
                opcode.value = sprmPFKeepFollow;
                break;
            case 9:
                opcode.value = sprmPFPageBreakBefore;
                break;
            case 10:
                opcode.value = sprmPBrcl;
                break;
            case 11:
                opcode.value = sprmPBrcp;
                break;
            case 12:
                opcode.value = sprmPAnld;
                break;
            case 13:
                opcode.value = sprmPIlvl;
                break;
            case 14:
                opcode.value = sprmPFNoLineNumb;
                break;
            case 15:
                opcode.value = sprmPChgTabsPapx;
                break;
            case 16:
                opcode.value = sprmPDxaRight;
                break;
            case 17:
                opcode.value = sprmPDxaLeft;
                break;
            case 18:
                opcode.value = sprmPNest;
                break;
            case 19:
                opcode.value = sprmPDxaLeft1;
                break;
            case 20:
                opcode.value = sprmPDyaLine;
                break;
            case 21:
                opcode.value = sprmPDyaBefore;
                break;
            case 22:
                opcode.value = sprmPDyaAfter;
                break;
            case 23:
                opcode.value = sprmPChgTabs;
                break;
            case 24:
                opcode.value = sprmPFInTable;
                break;
            case 25:
                opcode.value = sprmPFTtp;
                break;
            case 26:
                opcode.value = sprmPDxaAbs;
                break;
            case 27:
                opcode.value = sprmPDyaAbs;
                break;
            case 28:
                opcode.value = sprmPDxaWidth;
                break;
            case 29:
                opcode.value = sprmPPc;
                break;
            case 30:
                opcode.value = sprmPBrcTop10;
                break;
            case 31:
                opcode.value = sprmPBrcLeft10;
                break;
            case 32:
                opcode.value = sprmPBrcBottom10;
                break;
            case 33:
                opcode.value = sprmPBrcRight10;
                break;
            case 34:
                opcode.value = sprmPBrcBetween10;
                break;
            case 35:
                opcode.value = sprmPBrcBar10;
                break;
            case 36:
                opcode.value = sprmPDxaFromText10;
                break;
            case 37:
                opcode.value = sprmPWr;
                break;
            case 38:
                opcode.value = sprmPBrcTop;
                break;
            case 39:
                opcode.value = sprmPBrcLeft;
                break;
            case 40:
                opcode.value = sprmPBrcBottom;
                break;
            case 41:
                opcode.value = sprmPBrcRight;
                break;
            case 42:
                opcode.value = sprmPBrcBetween;
                break;
            case 43:
                opcode.value = sprmPBrcBar;
                break;
            case 44:
                opcode.value = sprmPFNoAutoHyph;
                break;
            case 45:
                opcode.value = sprmPWHeightAbs;
                break;
            case 46:
                opcode.value = sprmPDcs;
                break;
            case 47:
                opcode.value = sprmPShd;
                break;
            case 48:
                opcode.value = sprmPDyaFromText;
                break;
            case 49:
                opcode.value = sprmPDxaFromText;
                break;
            case 50:
                opcode.value = sprmPFLocked;
                break;
            case 51:
                opcode.value = sprmPFWidowControl;
                break;
            case 52:
                opcode.value = sprmPRuler;
                break;
            case 65:
                opcode.value = sprmCFRMarkDel;
                break;
            case 66:
                opcode.value = sprmCFRMark;
                break;
            case 67:
                opcode.value = sprmCFFldVanish;
                break;
            case 68:
                opcode.value = sprmCPicLocation;
                break;
            case 69:
                opcode.value = sprmCIbstRMark;
                break;
            case 70:
                opcode.value = sprmCDttmRMark;
                break;
            case 71:
                opcode.value = sprmCFData;
                break;
            case 72:
                opcode.value = sprmCIdslRMark;
                break;
            case 73:
                opcode.value = sprmCChs;
                break;
            case 74:
                opcode.value = sprmCSymbol;
                break;
            case 75:
                opcode.value = sprmCFOle2;
                break;
            case 80:
                opcode.value = sprmCIstd;
                break;
            case 81:
                opcode.value = sprmCIstdPermute;
                break;
            case 82:
                opcode.value = sprmCDefault;
                break;
            case 83:
                opcode.value = sprmCPlain;
                break;
            case 85:
                opcode.value = sprmCFBold;
                break;
            case 86:
                opcode.value = sprmCFItalic;
                break;
            case 87:
                opcode.value = sprmCFStrike;
                break;
            case 88:
                opcode.value = sprmCFOutline;
                break;
            case 89:
                opcode.value = sprmCFShadow;
                break;
            case 90:
                opcode.value = sprmCFSmallCaps;
                break;
            case 91:
                opcode.value = sprmCFCaps;
                break;
            case 92:
                opcode.value = sprmCFVanish;
                break;
            case 93:
                opcode.value = sprmCRgFtc0;
                break;
            case 94:
                opcode.value = sprmCKul;
                break;
            case 95:
                opcode.value = sprmCSizePos;
                break;
            case 96:
                opcode.value = sprmCDxaSpace;
                break;
            case 97:
                opcode.value = sprmCLid;
                break;
            case 98:
                opcode.value = sprmCIco;
                break;
            case 99:
                opcode.value = sprmCHps;
                break;
            case 100:
                opcode.value = sprmCHpsInc;
                break;
            case 101:
                opcode.value = sprmCHpsPos;
                break;
            case 102:
                opcode.value = sprmCHpsPosAdj;
                break;
            case 103:
                opcode.value = sprmCMajority;
                break;
            case 104:
                opcode.value = sprmCIss;
                break;
            case 105:
                opcode.value = sprmCHpsNew50;
                break;
            case 106:
                opcode.value = sprmCHpsInc1;
                break;
            case 107:
                opcode.value = sprmCHpsKern;
                break;
            case 108:
                opcode.value = sprmCMajority50;
                break;
            case 109:
                opcode.value = sprmCHpsMul;
                break;
            case 110:
                opcode.value = sprmCYsri;
                break;
            case 117:
                opcode.value = sprmCFSpec;
                break;
            case 118:
                opcode.value = sprmCFObj;
                break;
            case 119:
                opcode.value = sprmPicBrcl;
                break;
            case 120:
                opcode.value = sprmPicScale;
                break;
            case 121:
                opcode.value = sprmPicBrcTop;
                break;
            case 122:
                opcode.value = sprmPicBrcLeft;
                break;
            case 123:
                opcode.value = sprmPicBrcBottom;
                break;
            case 124:
                opcode.value = sprmPicBrcRight;
                break;
            case 131:
                opcode.value = sprmScnsPgn;
                break;
            case 132:
                opcode.value = sprmSiHeadingPgn;
                break;
            case 133:
                opcode.value = sprmSOlstAnm;
                break;
            case 136:
                opcode.value = sprmSDxaColWidth;
                break;
            case 137:
                opcode.value = sprmSDxaColSpacing;
                break;
            case 138:
                opcode.value = sprmSFEvenlySpaced;
                break;
            case 139:
                opcode.value = sprmSFProtected;
                break;
            case 140:
                opcode.value = sprmSDmBinFirst;
                break;
            case 141:
                opcode.value = sprmSDmBinOther;
                break;
            case 142:
                opcode.value = sprmSBkc;
                break;
            case 143:
                opcode.value = sprmSFTitlePage;
                break;
            case 144:
                opcode.value = sprmSCcolumns;
                break;
            case 145:
                opcode.value = sprmSDxaColumns;
                break;
            case 146:
                opcode.value = sprmSFAutoPgn;
                break;
            case 147:
                opcode.value = sprmSNfcPgn;
                break;
            case 148:
                opcode.value = sprmSDyaPgn;
                break;
            case 149:
                opcode.value = sprmSDxaPgn;
                break;
            case 150:
                opcode.value = sprmSFPgnRestart;
                break;
            case 151:
                opcode.value = sprmSFEndnote;
                break;
            case 152:
                opcode.value = sprmSLnc;
                break;
            case 153:
                opcode.value = sprmSGprfIhdt;
                break;
            case 154:
                opcode.value = sprmSNLnnMod;
                break;
            case 155:
                opcode.value = sprmSDxaLnn;
                break;
            case 156:
                opcode.value = sprmSDyaHdrTop;
                break;
            case 157:
                opcode.value = sprmSDyaHdrBottom;
                break;
            case 158:
                opcode.value = sprmSLBetween;
                break;
            case 159:
                opcode.value = sprmSVjc;
                break;
            case 160:
                opcode.value = sprmSLnnMin;
                break;
            case 161:
                opcode.value = sprmSPgnStart;
                break;
            case 162:
                opcode.value = sprmSBOrientation;
                break;
            case 163:
                opcode.value = sprmSBCustomize;
                break;
            case 164:
                opcode.value = sprmSXaPage;
                break;
            case 165:
                opcode.value = sprmSYaPage;
                break;
            case 166:
                opcode.value = sprmSDxaLeft;
                break;
            case 167:
                opcode.value = sprmSDxaRight;
                break;
            case 168:
                opcode.value = sprmSDyaTop;
                break;
            case 169:
                opcode.value = sprmSDyaBottom;
                break;
            case 170:
                opcode.value = sprmSDzaGutter;
                break;
            case 171:
                opcode.value = sprmSDmPaperReq;
                break;
            case 182:
                opcode.value = sprmTJc;
                break;
            case 183:
                opcode.value = sprmTDxaLeft;
                break;
            case 184:
                opcode.value = sprmTDxaGapHalf;
                break;
            case 185:
                opcode.value = sprmTFCantSplit;
                break;
            case 186:
                opcode.value = sprmTTableHeader;
                break;
            case 187:
                opcode.value = sprmTTableBorders;
                break;
            case 188:
                opcode.value = sprmTDefTable10;
                break;
            case 189:
                opcode.value = sprmTDyaRowHeight;
                break;
            case 190:
                opcode.value = sprmTDefTable;
                break;
            case 191:
                opcode.value = sprmTDefTableShd;
                break;
            case 192:
                opcode.value = sprmTTlp;
                break;
            case 193:
                opcode.value = sprmTSetBrc;
                break;
            case 194:
                opcode.value = sprmTInsert;
                break;
            case 195:
                opcode.value = sprmTDelete;
                break;
            case 196:
                opcode.value = sprmTDxaCol;
                break;
            case 197:
                opcode.value = sprmTMerge;
                break;
            case 198:
                opcode.value = sprmTSplit;
                break;
            case 199:
                opcode.value = sprmTSetBrc10;
                break;
            case 200:
                opcode.value = sprmTSetShd;
                break;
            default:
                opcode.value = sprm;
            }
        }
        operandSize = operandSizes[opcode.bits.spra];
        if (!operandSize)
        {
            U8 t8;
            U16 t16;

            // Get length of variable size operand.

            switch (opcode.value)
            {
            case sprmPChgTabs:
                bytes += MsWordGenerated::read(in + bytes, &t8);
                operandSize = t8;
                if (operandSize == 255)
                    kdError(s_area) << "MsWord::paragraphStyleModify: cannot parse sprmPChgTabs" << endl;
                break;
            case sprmTDefTable10:
            case sprmTDefTable:
                bytes += MsWordGenerated::read(in + bytes, &t16);
                operandSize = t16 - 1;
                break;
            default:
                bytes += MsWordGenerated::read(in + bytes, &t8);
                operandSize = t8;
                break;
            }
        }

        // Apply known opcodes.

        U8 tmp;

        //kdDebug(s_area) << "MsWord::paragraphStyleModify: opcode:" << opcode.value << endl;
        switch (opcode.value)
        {
        case sprmPJc: // 0x2403
            MsWordGenerated::read(in + bytes, &pap->jc);
            break;
        case sprmPFSideBySide: // 0x2404
            MsWordGenerated::read(in + bytes, &tmp);
            pap->fSideBySide = tmp;
            break;
        case sprmPFKeep:
            MsWordGenerated::read(in + bytes, &tmp);
            pap->fKeep = tmp;
            break;
        case sprmPFKeepFollow:
            MsWordGenerated::read(in + bytes, &tmp);
            pap->fKeepFollow = tmp;
            break;
        case sprmPFPageBreakBefore: // 0x2407
            MsWordGenerated::read(in + bytes, &tmp);
            pap->fPageBreakBefore = tmp;
            break;
        case sprmPFInTable: // 0x2416
            MsWordGenerated::read(in + bytes, &tmp);
            pap->fInTable = tmp;
            break;
        case sprmPFTtp:
            MsWordGenerated::read(in + bytes, &tmp);
            pap->fTtp = tmp;
            break;
        case sprmPIlvl: // 0x260a
            MsWordGenerated::read(in + bytes, &pap->ilvl);
            break;
        case sprmPOutLvl: // 0x2640
            MsWordGenerated::read(in + bytes, &pap->lvl);
            break;
        case sprmPIstd: // 0x4600
            MsWordGenerated::read(in + bytes, &pap->istd);
            break;
        case sprmPIlfo: // 0x460B
            MsWordGenerated::read(in + bytes, &pap->ilfo);
            break;
        case sprmPBrcTop: // 0x6424
            MsWordGenerated::read(in + bytes, &pap->brcTop);
            break;
        case sprmPBrcLeft: // 0x6425
            MsWordGenerated::read(in + bytes, &pap->brcLeft);
            break;
        case sprmPBrcBottom: // 0x6426
            MsWordGenerated::read(in + bytes, &pap->brcBottom);
            break;
        case sprmPBrcRight: // 0x6427
            MsWordGenerated::read(in + bytes, &pap->brcRight);
            break;
        case sprmPDxaRight: // 0x840E
            MsWordGenerated::read(in + bytes, &pap->dxaRight);
            break;
        case sprmPDxaLeft: // 0x840F
            MsWordGenerated::read(in + bytes, &pap->dxaLeft);
            break;
        case sprmPDxaLeft1: // 0x8411
            MsWordGenerated::read(in + bytes, &pap->dxaLeft1);
            break;
        case sprmPDxaAbs: // 0x8418
            MsWordGenerated::read(in + bytes, &pap->dxaAbs);
            break;
        case sprmPDyaAbs: // 0x8419
            MsWordGenerated::read(in + bytes, &pap->dyaAbs);
            break;
        case sprmPDxaWidth: // 0x841A
            MsWordGenerated::read(in + bytes, &pap->dxaWidth);
            break;
        case sprmPDyaBefore: // 0xA413
            MsWordGenerated::read(in + bytes, &pap->dyaBefore);
            break;
        case sprmPDyaAfter: // 0xA414
            MsWordGenerated::read(in + bytes, &pap->dyaAfter);
            break;

        // TAP-specific stuff...

        case sprmTTableBorders:
            if (tap)
            {
                MsWordGenerated::read(in + bytes, &tap->rgbrcTable[0], 6);
            }
            break;
        case sprmTDefTable:
            if (tap)
            {
                // Get cell count.

                MsWordGenerated::read(in + bytes, &tmp);
                tap->itcMac = tmp;
                tmp = 1;

                // Get cell boundaries and descriptions.

                tmp += MsWordGenerated::read(in + bytes + tmp, (U16 *)&tap->rgdxaCenter[0], tap->itcMac + 1);
                tmp += MsWordGenerated::read(in + bytes + tmp, &tap->rgtc[0], tap->itcMac);
            }
            break;
        default:
            if (!(m_fib.nFib > s_maxWord6Version) &&
                (bytes == count) &&
                (opcode.value == 0))
            {
                // The last byte of a Word6 grpprl can be a zero padding byte.
            }
            else
            {
                kdWarning(s_area) << "MsWord::paragraphStyleModify: unsupported opcode:" << opcode.value << endl;
            }
            break;
        }
        bytes += operandSize;
    }
}

// Apply list formatting.

void MsWord::paragraphStyleModify(PAP *pap, LFO &style, bool useFormatting, bool useStartAt)
{
    const U8 *ptr = m_tableStream + m_fib.fcPlcfLst; //lcbPlcfLst.
    U16 lstfCount;
    LSTF data;
    int i;

    // Find the number of LSTFs.

    ptr += MsWordGenerated::read(ptr, &lstfCount);

    // Walk the LSTFs.

    for (i = 0; i < lstfCount; i++)
    {
        ptr += MsWordGenerated::read(ptr, &data);
        if (data.lsid == style.lsid)
        {
            // Record the style index.

            pap->istd = data.rgistd[pap->ilvl];

            // Build the base PAP if required.

            if (pap->istd != 4095)
                paragraphStyleModify(pap, pap->istd);

            U8 *ptr2 = (U8 *)m_listStyles[i][pap->ilvl];
            LVLF level;
            U16 numberTextLength;
            QString numberText;

            // Apply the LVLF.

            ptr2 += MsWordGenerated::read(ptr2, &level);
            pap->anld.nfc = level.nfc;
            pap->anld.jc = level.jc;
            if (useStartAt)
            {
                // Apply the startAt.

                pap->anld.iStartAt = level.iStartAt;
            }

            // Apply the variable length parts.

            if (useFormatting)
            {
                // Apply the grpprl.

                paragraphStyleModify(pap, NULL, ptr2, level.cbGrpprlPapx);
            }
            ptr2 += level.cbGrpprlPapx;
            ptr2 += level.cbGrpprlChpx;
            ptr2 += MsWordGenerated::read(ptr2, &numberTextLength);
            ptr2 += read(ptr2, &numberText, numberTextLength, true);
            break;
        }
    }
    if (i == lstfCount)
        kdError(s_area) << "MsWord::error finding LSTF[" << style.lsid << "]" << endl;
}

// Apply a PAPX.

void MsWord::paragraphStyleModify(PAP *pap, PAPXFKP &style)
{
    // Record the style index.

    pap->istd = style.istd;

    // Build the base PAP then walk the grpprl.

    paragraphStyleModify(pap, style.istd);
    paragraphStyleModify(pap, NULL, style.grpprl, style.grpprlBytes);
}

// Apply a layout.

void MsWord::paragraphStyleModify(PAP *pap, PHE &layout)
{
    pap->phe = layout;
}

// Extract the paragraph style for an STD.

void MsWord::paragraphStyleModify(PAP *pap, STD &style)
{
    if (style.sgc != 1)
    {
        kdError(s_area) << "MsWord::paragraphStyleModify: not a paragraph style: " << style.sgc << endl;
        return;
    }

    const U8 *grpprl;
    U16 cbUpx;

    // Align to an even-byte position.

    grpprl = style.grupx;
    if ((int)grpprl & 1)
        grpprl++;
    grpprl += MsWordGenerated::read(grpprl, &cbUpx);

    // Record the style index.

    grpprl += MsWordGenerated::read(grpprl, &pap->istd);

    // Build the base PAP then walk the grpprl.

    paragraphStyleModify(pap, pap->istd);
    paragraphStyleModify(pap, NULL, grpprl, cbUpx - 2);
}

void MsWord::parse()
{
    if (m_constructionError.length())
    {
       gotError(m_constructionError);
       return;
    }

    // Fill the style cache.

    m_wasInTable = false;

    // Note that we test for the presence of complex structure, rather than
    // m_fib.fComplex. This allows us to treat newer files which always seem
    // to have piece tables in a consistent manner.
    //
    // There is also the implication that without the complex structures, the
    // text cannot be in unicode form.

    if (m_fib.lcbClx)
    {
        // Start with the grpprl and PCD.
        //
        // For the grpprl array, we store the offset to the
        // byte count preceeding the first entry, and the number of entries.
        //
        // For the text plex, we store the start and size of the plex in the table
    
        typedef enum
        {
            clxtGrpprl = 1,
            clxtPlcfpcd = 2
        };
    
        struct
        {
            U32 byteCountOffset;
            U32 count;
        } grpprls;
    
        struct
        {
            const U8 *ptr;
            U32 byteCount;
        } textPlex;
    
        unsigned count = 0;
        const U8 *ptr;
        const U8 *end;
        U8 clxt = 0;
        U16 cb;
        U32 lcb;
    
        // First skip the grpprls.
    
        ptr = m_tableStream + m_fib.fcClx;
        end = ptr + m_fib.lcbClx;
        grpprls.byteCountOffset = (ptr + 1) - m_tableStream;
        grpprls.count = 0;
        while (ptr < end)
        {
            ptr += MsWordGenerated::read(ptr, &clxt);
            if (clxt != clxtGrpprl)
            {
                ptr--;
                break;
            }
            grpprls.count++;
            ptr += MsWordGenerated::read(ptr, &cb);
            ptr += cb;
        }
    
        // Now locate the piece table.
    
        while (ptr < end)
        {
            ptr += MsWordGenerated::read(ptr, &clxt);
            if (clxt != clxtPlcfpcd)
            {
                ptr--;
                break;
            }
            count++;
            ptr += MsWordGenerated::read(ptr, &lcb);
            textPlex.byteCount = lcb;
            textPlex.ptr = ptr;
            ptr += lcb;
        }
        if ((clxt != clxtPlcfpcd) ||
            (count != 1))
        {
            constructionError(__LINE__, "cannot locate the piece table");
            return;
        };

        // Locate the piece table in a complex document.

        Plex<PCD, 8> *pieceTable = new Plex<PCD, 8>(m_fib);
    
        U32 startFc;
        U32 endFc;
        PCD data;
        const U32 codepage1252mask = 0x40000000;
        bool unicode;

        pieceTable->startIteration(textPlex.ptr, textPlex.byteCount);
        while (pieceTable->getNext(&startFc, &endFc, &data))
        {
            unicode = ((data.fc & codepage1252mask) != codepage1252mask);
            if (!unicode)
            {
                data.fc &= ~ codepage1252mask;
                data.fc /= 2;
            }
            getPAPXFKP(m_mainStream + data.fc, endFc - startFc, unicode);
        }
    }
    else
    {
        getPAPXFKP(
            m_mainStream + m_fib.fcMin,
            m_fib.fcMac - m_fib.fcMin,
            false);
    }
}

template <class T, int word6Size>
MsWord::Plex<T, word6Size>::Plex(FIB &fib) :
    m_fib(fib)
{
};

template <class T, int word6Size>
bool MsWord::Plex<T, word6Size>::getNext(U32 *startFc, U32 *endFc, T *data)
{
    // Sanity check accesses beyond end of Plex.

    if (m_i >= m_crun)
    {
        return false;
    }
    m_fcNext += MsWordGenerated::read(m_fcNext, startFc);
    MsWordGenerated::read(m_fcNext, endFc);
    m_dataNext += MsWord::read(m_fib.nFib, m_dataNext, data);
    m_i++;
    return true;
}

template <class T, int word6Size>
void MsWord::Plex<T, word6Size>::startIteration(const U8 *plex, const U32 byteCount)
{
    U32 startFc;

    m_plex = plex;
    m_byteCount = byteCount;

    // Calculate the number of entries in the plex.

    if (m_fib.nFib > s_maxWord6Version)
        m_crun = (m_byteCount - sizeof(startFc)) / (sizeof(T) + sizeof(startFc));
    else
        m_crun = (m_byteCount - sizeof(startFc)) / (word6Size + sizeof(startFc));
    kdDebug(s_area) << "MsWord::Plex::startIteration: " << m_crun << endl;
    m_fcNext = m_plex;
    m_dataNext = m_plex + ((m_crun + 1) * sizeof(startFc));
    m_i = 0;
}

// Read a string, converting to unicode if needed.
unsigned MsWord::read(const U8 *in, QString *out, unsigned count, bool unicode)
{
    U16 char16;
    U8 char8;
    unsigned bytes = 0;

    *out = QString("");
    if (unicode)
    {
        for (unsigned i = 0; i < count; i++)
        {
            bytes += MsWordGenerated::read(in + bytes, &char16);
            *out += QChar(char16);
        }
    }
    else
    {
        for (unsigned i = 0; i < count; i++)
        {
            bytes += MsWordGenerated::read(in + bytes, &char8);
            *out += QChar(char2unicode(char8));
        }
    }
    return bytes;
}

//
// Read a PAPX as stored in an FKP.
//
unsigned MsWord::read(unsigned nFib, const U8 *in, PAPXFKP *out)
{
    unsigned bytes = 0;
    U8 cw;

    bytes += MsWordGenerated::read(in + bytes, &cw);
    if (nFib > s_maxWord6Version)
    {
        if (!cw)
        {
            bytes += MsWordGenerated::read(in + bytes, &cw);
            out->grpprlBytes = 2 * (cw - 1);
        }
        else
        {
            out->grpprlBytes = 2 * (cw - 1) - 1;
        }
        bytes += MsWordGenerated::read(in + bytes, &out->istd);
        out->grpprl = (U8 *)(in + bytes);
        bytes += out->grpprlBytes;
    }
    else
    {
        U8 tmp;

        out->grpprlBytes = 2 * (cw - 1);
        // The spec says that the Word6 istd is a byte, but that seems to be wrong.
        bytes += MsWordGenerated::read(in + bytes, &out->istd);
        out->grpprl = (U8 *)(in + bytes);
        bytes += out->grpprlBytes;
    }
    return bytes;
}

unsigned MsWord::read(const U8 *in, unsigned baseInFile, STD *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        U8 offset;

        offset = 0;
        offset += MsWordGenerated::read(in + offset, (U16 *)(ptr + bytes), 5);
        memset((ptr + bytes) + baseInFile, 0, 10 - baseInFile);
        offset -= 10 - baseInFile;

        // If the baseInFile is less than 10, then the style name is not stored in unicode!

        if (baseInFile < 10)
        {
            U8 nameLength;
            U8 terminator;

            offset += MsWordGenerated::read(in + offset, &nameLength);
            offset += read(in + offset, &out->xstzName, nameLength, false);
            offset += MsWordGenerated::read(in + offset, &terminator);
        }
        else
        {
            U16 nameLength;
            U16 terminator;

            offset += MsWordGenerated::read(in + offset, &nameLength);
            offset += read(in + offset, &out->xstzName, nameLength, true);
            offset += MsWordGenerated::read(in + offset, &terminator);
        }
        out->grupx = in + offset;
        if ((int)out->grupx & 1)
            out->grupx++;
        bytes += out->bchUpe;
        out++;
    }
    return bytes;
} // STD

unsigned MsWord::read(const U8 *in, FIB *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        // What version of Word are we dealing with?
        // Word 6 for the PC writes files with nFib = 101-102.
        // Word 6 for the Mac writes files with nFib = 103-104.
        // Word 8 (a.k.a. Winword 97) and later products write files with nFib > 105.

        // Bytes 0 to 31 are common.

        bytes += MsWordGenerated::read(in + bytes, (U16 *)(ptr + bytes), 7);
        bytes += MsWordGenerated::read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += MsWordGenerated::read(in + bytes, (U8 *)(ptr + bytes), 2);
        bytes += MsWordGenerated::read(in + bytes, (U16 *)(ptr + bytes), 2);
        bytes += MsWordGenerated::read(in + bytes, (U32 *)(ptr + bytes), 2);
        if (out->nFib > s_maxWord6Version)
        {
            bytes += MsWordGenerated::read(in + bytes, (U16 *)(ptr + bytes), 16);
            bytes += MsWordGenerated::read(in + bytes, (U32 *)(ptr + bytes), 22);
            bytes += MsWordGenerated::read(in + bytes, (U16 *)(ptr + bytes), 1);
            bytes += MsWordGenerated::read(in + bytes, (U32 *)(ptr + bytes), 186);
        }
        else
        if (out->nFib > s_minWordVersion)
        {
            // We will convert the FIB into the same form as for Winword

            out->csw = 14;
            out->wMagicCreated = 0;
            out->wMagicRevised = 0;
            out->wMagicCreatedPrivate = 0;
            out->wMagicRevisedPrivate = 0;
            out->pnFbpChpFirst_W6 = 0;
            out->pnChpFirst_W6 = 0;
            out->cpnBteChp_W6 = 0;
            out->pnFbpPapFirst_W6 = 0;
            out->pnPapFirst_W6 = 0;
            out->cpnBtePap_W6 = 0;
            out->pnFbpLvcFirst_W6 = 0;
            out->pnLvcFirst_W6 = 0;
            out->cpnBteLvc_W6 = 0;
            out->lidFE = out->lid;
            out->clw = 22;
            bytes += MsWordGenerated::read(in + bytes, &out->cbMac);
            bytes += 16;
            out->lProductCreated = 0;
            out->lProductRevised = 0;

            // ccpText through ccpHdrTxbx.

            bytes += MsWordGenerated::read(in + bytes, &out->ccpText, 8);

            // ccpSpare2.

            bytes += 4;
            out->cfclcb = 93;

            // fcStshfOrig through lcbSttbfAtnbkmk.

            bytes += MsWordGenerated::read(in + bytes, &out->fcStshfOrig, 76);

            // wSpare4Fib.

            bytes += 2;

            // pnChpFirst through cpnBtePap.

            U16 tmp;
            bytes += MsWordGenerated::read(in + bytes, &tmp);
            out->pnChpFirst = tmp;
            bytes += MsWordGenerated::read(in + bytes, &tmp);
            out->pnPapFirst = tmp;
            bytes += MsWordGenerated::read(in + bytes, &tmp);
            out->cpnBteChp = tmp;
            bytes += MsWordGenerated::read(in + bytes, &tmp);
            out->cpnBtePap = tmp;

            // fcPlcdoaMom through lcbSttbFnm.

            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcdoaMom, 70);
        }
        else
        {
            // We don't support this.

            kdError(s_area) << "unsupported version of Word (nFib" << out->nFib << ")";
            break;
        }
        out++;
    }
    return bytes;
} // FIB

unsigned MsWord::read(unsigned nFib, const U8 *in, BTE *out)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;
    U16 tmp;

    if (nFib > s_maxWord6Version)
    {
        bytes = MsWordGenerated::read(in, out);
    }
    else
    {
        bytes += MsWordGenerated::read(in + bytes, &tmp);
        out->pn = tmp;
    }
    return bytes;
} // BTE

unsigned MsWord::read(unsigned nFib, const U8 *in, PCD *out)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    bytes = MsWordGenerated::read(in, out);
    return bytes;
} // PCD

unsigned MsWord::read(unsigned nFib, const U8 *in, PHE *out)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;
    U16 tmp;

    if (nFib > s_maxWord6Version)
    {
        bytes = MsWordGenerated::read(in, out);
    }
    else
    {
        bytes += MsWordGenerated::read(in + bytes, (U16 *)(ptr + bytes));
        bytes += MsWordGenerated::read(in + bytes, &tmp);
        out->dxaCol = tmp;
        bytes += MsWordGenerated::read(in + bytes, &tmp);
        out->dym = tmp;
    }
    return bytes;
} // PHE

