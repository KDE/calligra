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

void MsWord::decodeParagraph(const QString &text, PAP &style)
{
    if (style.fInTable)
    {
        if (!m_wasInTable)
            gotTableStart();
        m_wasInTable = true;
        gotTableParagraph(text, style);
    }
    else
    {
        if (m_wasInTable)
            gotTableEnd();
        m_wasInTable = false;
        if ((style.istd >= 1) && (style.istd <= 9))
        {
            gotHeadingParagraph(text, style);
        }
        else
        if (style.ilfo)
        {
            gotListParagraph(text, style);
        }
        else
        {
            gotParagraph(text, style);
        }
    }
}

template <class T1, class T2>
MsWord::Fkp<T1, T2>::Fkp(MsWord *client, const U8 *fkp) :
    m_client(client),
    m_fkp(fkp)
{
    // Get the number of entries in the FKP.

    MsWordGenerated::read(m_fkp + 511, &m_crun);
    kdDebug(s_area) << "MsWord::Fkp::Fkp: crun: " << (unsigned)m_crun << endl;
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
    m_dataNext += MsWordGenerated::read(m_dataNext, data1);

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
        MsWord::read(m_fkp + (2 * (*rgb)), data2);
    }
    return (m_i++ < m_crun);
}

template <class T1, class T2>
void MsWord::Fkp<T1, T2>::startIteration()
{
    U32 startFc;

    m_fcNext = m_fkp;
    m_dataNext = m_fkp + ((m_crun + 1) * sizeof(startFc));
    m_i = 0;
}

void MsWord::getPAPXFKP(const U8 *textStartFc, U32 textLength, bool unicode)
{
    // A bin table is a plex of BTEs.

    Plex<BTE> btes = Plex<BTE>(
                       this,
                       m_tableStream + m_fib.fcPlcfbtePapx,
                       m_fib.lcbPlcfbtePapx);
    U32 startFc;
    U32 endFc;
    BTE data;

    // Walk the BTEs.

    btes.startIteration();
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

    Fkp<PHE, PAPXFKP> papx = Fkp<PHE, PAPXFKP>(this, fkp);

    U32 startFc;
    U32 endFc;
    U8 rgb;
    PAP pap;
    PHE layout;
    PAPXFKP style;

    papx.startIteration();
    while (papx.getNext(&startFc, &endFc, &rgb, &layout, &style))
    {
        QString text;

        read(m_mainStream + startFc, &text, endFc - startFc, unicode);
        paragraphStyleCreate(&pap);
        paragraphStyleModify(&pap, style);
        paragraphStyleModify(&pap, layout);

        // What kind of paragraph was this?

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
                paragraphStyleModify(&pap, ptr3, level.cbGrpprlPapx);

                // Apply the startAt.

                pap.anld.iStartAt = level.iStartAt;
                kdDebug(s_area) << "got startAt " << pap.anld.iStartAt << " from LVLF" << endl;
            }
            kdDebug(s_area) << "list: startAt: " << pap.anld.iStartAt <<
                "nfc: " << pap.anld.nfc << "jc: " << pap.anld.jc << endl;
if (pap.anld.nfc > 5)
pap.anld.nfc=0;
        }
        kdDebug(s_area) << "MsWord::gotParagraph: style: " << pap.istd << endl;
        decodeParagraph(text, pap);
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

    // Fetch the STSHI.

    ptr += MsWordGenerated::read(ptr, &cbStshi);
    if (cbStshi > sizeof(stshi))
    {
        kdError(s_area) << "MsWord::getStyles: unsupported STSHI size " << cbStshi << endl;
        return;
    }

    // We know that older/smaller STSHIs can simply be zero extended into our STSHI.
    // So, we overwrite anything thatis not valid with zeros.

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
            read(ptr, &std);
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

void MsWord::gotTableEnd()
{
}

void MsWord::gotTableParagraph(const QString &text, PAP &style)
{
    kdDebug(s_area) << "MsWord::gotParagraph: table: " << style.ilvl << endl;
}

void MsWord::gotTableStart()
{
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

    // Store away the streams for future use. Note that we do not
    // copy the contents of the streams, and that we rely on the storage
    // being present until we are destroyed.

    m_mainStream = mainStream;
    m_tableStream = m_fib.fWhichTblStm ? table1Stream : table0Stream;
    m_dataStream = dataStream;
    if (!m_tableStream)
    {
        constructionError(__LINE__, "the tableStream is missing");
        return;
    }

    // Start with the grpprl and PCD.

    typedef enum
    {
        clxtGrpprl = 1,
        clxtPlcfpcd = 2
    };

    const U8 *ptr;
    const U8 *end;
    U8 clxt = 0;
    U16 cb;
    U32 lcb;

    ptr = m_tableStream + m_fib.fcClx;
    end = ptr + m_fib.lcbClx;
    m_grpprls.byteCountOffset = (ptr + 1) - m_tableStream;
    m_grpprls.count = 0;
    while (ptr < end)
    {
        ptr += MsWordGenerated::read(ptr, &clxt);
        if (clxt != clxtGrpprl)
        {
            ptr--;
            break;
        }
        m_grpprls.count++;
        ptr += MsWordGenerated::read(ptr, &cb);
        ptr += cb;
    }

    // For the text plex, we store the start and size of the plex in the table

    struct
    {
        const U8 *ptr;
        U32 byteCount;
    } m_textPlex;
    unsigned count = 0;

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
        m_textPlex.byteCount = lcb;
        m_textPlex.ptr = ptr;
        ptr += lcb;
    }
    if ((clxt != clxtPlcfpcd) ||
        (count != 1))
    {
        constructionError(__LINE__, "cannot locate the piece table");
        return;
    };
    m_pcd = new Plex<PCD>(this, m_textPlex.ptr, m_textPlex.byteCount);
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

void MsWord::paragraphStyleModify(PAP *pap, const U8 *grpprl, unsigned count)
{
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
        bytes += MsWordGenerated::read(in + bytes, &opcode.value);
        operandSize = operandSizes[opcode.bits.spra];
        if (!operandSize)
        {
            U8 t8;
            U16 t16;

            // Get length of variable size operand.

            switch (opcode.value)
            {
            case 0xc615:
                bytes += MsWordGenerated::read(in + bytes, &t8);
                operandSize = t8;
                if (operandSize == 255)
                    kdError(s_area) << "MsWord::paragraphStyleModify: cannot parse sprmPChgTabs" << endl;
                break;
            case 0xd606:
            case 0xd608:
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
        case 0x2403:
            MsWordGenerated::read(in + bytes, &pap->jc);
            break;
        case 0x2404:
            MsWordGenerated::read(in + bytes, &tmp);
            pap->fSideBySide = tmp;
            break;
        case 0x2405:
            MsWordGenerated::read(in + bytes, &tmp);
            pap->fKeep = tmp;
            break;
        case 0x2406:
            MsWordGenerated::read(in + bytes, &tmp);
            pap->fKeepFollow = tmp;
            break;
        case 0x2416:
            MsWordGenerated::read(in + bytes, &tmp);
            pap->fInTable = tmp;
            break;
        case 0x2417:
            MsWordGenerated::read(in + bytes, &tmp);
            pap->fTtp = tmp;
            break;
        case 0x260a:
            MsWordGenerated::read(in + bytes, &pap->ilvl);
            break;
        case 0x2640:
            MsWordGenerated::read(in + bytes, &pap->lvl);
            break;
        case 0x460b:
            MsWordGenerated::read(in + bytes, &pap->ilfo);
            break;
        case 0x840e:
            MsWordGenerated::read(in + bytes, &pap->dxaRight);
            break;
        case 0x840f:
            MsWordGenerated::read(in + bytes, &pap->dxaLeft);
            break;
        case 0x8411:
            MsWordGenerated::read(in + bytes, &pap->dxaLeft1);
            break;
        case 0xa413:
            MsWordGenerated::read(in + bytes, &pap->dyaBefore);
            break;
        case 0xa414:
            MsWordGenerated::read(in + bytes, &pap->dyaAfter);
            break;
        default:
            kdWarning(s_area) << "MsWord::paragraphStyleModify: unsupported opcode:" << opcode.value << endl;
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

                paragraphStyleModify(pap, ptr2, level.cbGrpprlPapx);
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
    paragraphStyleModify(pap, style.grpprl, style.grpprlBytes);
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
    paragraphStyleModify(pap, grpprl, cbUpx - 2);
}

void MsWord::parse()
{
    if (m_constructionError.length())
    {
       gotError(m_constructionError);
       return;
    }

    // Fill the style cache.

    getStyles();
    getListStyles();
    m_wasInTable = false;

    // Note that we test for the presence of complex structure, rather than
    // m_fib.fComplex. This allows us to treat newer files which always seem
    // to have piece tables in a consistent manner.
    //
    // There is also the implication that without the complex structures, the
    // text cannot be in unicode form.

    if (m_fib.lcbClx)
    {
        U32 startFc;
        U32 endFc;
        PCD data;
        const U32 codepage1252mask = 0x40000000;
        bool unicode;

        m_pcd->startIteration();
        while (m_pcd->getNext(&startFc, &endFc, &data))
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

template <class T>
MsWord::Plex<T>::Plex(MsWord *client, const U8 *plex, const U32 byteCount) :
    m_client(client),
    m_plex(plex),
    m_byteCount(byteCount)
{
    U32 startFc;

    // Calculate the number of entries in the plex.

    m_crun = (m_byteCount - sizeof(startFc)) / (sizeof(T) + sizeof(startFc));
    kdDebug(s_area) << "MsWord::Plex::Plex" << m_crun << endl;
};

template <class T>
bool MsWord::Plex<T>::getNext(U32 *startFc, U32 *endFc, T *data)
{
    // Sanity check accesses beyond end of Plex.

    if (m_i >= m_crun)
    {
        return false;
    }
    m_fcNext += MsWordGenerated::read(m_fcNext, startFc);
    MsWordGenerated::read(m_fcNext, endFc);
    m_dataNext += MsWordGenerated::read(m_dataNext, data);
    m_i++;
    return true;
}

template <class T>
void MsWord::Plex<T>::startIteration()
{
    U32 startFc;

    m_fcNext = m_plex;
    m_dataNext = m_plex + ((m_crun + 1) * sizeof(startFc));
    m_i = 0;
}

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
    //kdDebug(s_area) << "MsWord::read: " << *out << endl;
    return bytes;
}

//
// Read a PAPX as stored in an FKP.
//
unsigned MsWord::read(const U8 *in, PAPXFKP *out, unsigned count)
{
    unsigned bytes = 0;
    U8 cw;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += MsWordGenerated::read(in + bytes, &cw);
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
        out++;
    }
    return bytes;
}

unsigned MsWord::read(const U8 *in, STD *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        U16 nameLength;
        U16 terminator;
        U8 offset;

        offset = 0;
        offset += MsWordGenerated::read(in + offset, (U16 *)(ptr + bytes), 5);
        offset += MsWordGenerated::read(in + offset, &nameLength);
        offset += read(in + offset, &out->xstzName, nameLength, true);
        offset += MsWordGenerated::read(in + offset, &terminator);
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
        // Word 6 writes files with nFib = 101-104.
        // Winword 97 and later products write files with nFib > 105.

        // Bytes 0 to 31 are common.

        bytes += MsWordGenerated::read(in + bytes, (U16 *)(ptr + bytes), 7);
        bytes += MsWordGenerated::read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += MsWordGenerated::read(in + bytes, (U8 *)(ptr + bytes), 2);
        bytes += MsWordGenerated::read(in + bytes, (U16 *)(ptr + bytes), 2);
        bytes += MsWordGenerated::read(in + bytes, (U32 *)(ptr + bytes), 2);
        if (out->nFib > 105)
        {
            bytes += MsWordGenerated::read(in + bytes, (U16 *)(ptr + bytes), 16);
            bytes += MsWordGenerated::read(in + bytes, (U32 *)(ptr + bytes), 22);
            bytes += MsWordGenerated::read(in + bytes, (U16 *)(ptr + bytes), 1);
            bytes += MsWordGenerated::read(in + bytes, (U32 *)(ptr + bytes), 186);
        }
        else
        if (out->nFib > 100)
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





