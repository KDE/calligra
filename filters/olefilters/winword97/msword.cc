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

#include <errno.h>
#ifdef __linux__
#include <iconv.h>
#endif
#include <kdebug.h>
#include <msword.h>
#include <paragraph.h>

// We currently only take note of the document's main non-Far Eastern
// language, and ignore character properties. TBD: remove these restrictions!
unsigned short MsWord::char2unicode(unsigned lid, unsigned char c)
{
    static iconv_t lastIconv = (iconv_t)-1;
    static unsigned lastLid = (unsigned)-1;

    // Is the language changing?
    if (lastLid != lid)
    {
        const char *codepage;

        // Unconditionally set the new language - this will prevent
        // an error message for each character if things go wrong!
        lastLid = lid;

        // Find the name of the new code page.
        codepage = lid2codepage(lastLid);

        // Close any existing context before opening the new one.
        if (lastIconv != (iconv_t)-1)
        {
            iconv_close(lastIconv);
        }
        lastIconv = iconv_open("UCS-2", codepage);
        if (lastIconv != (iconv_t)-1)
        {
            kdDebug(s_area) <<
                "converting " << codepage <<
                " to UCS-2" << endl;
        }
        else
        {
            kdError(s_area) <<
                "cannot convert " << codepage <<
                " to UCS-2: " << strerror(errno) << endl;
        }
    }

    // Do the conversion!

    unsigned short result = '?';

    if (lastIconv != (iconv_t)-1)
    {
        unsigned char input[1];
        unsigned char output[2];
        size_t ibuflen = 1;
        size_t obuflen = 2;
        char *ibuf;
        char *obuf;

        input[0] = c;
        ibuf = (char *)&input[0];
        obuf = (char *)&output[0];
        if ((size_t)-1 != iconv(lastIconv, &ibuf, &ibuflen, &obuf, &obuflen))
        {
            result = (output[0] << 8) + output[1];
        }
        else
        {
            kdError(s_area) << "cannot convert " << c << ": " <<
                strerror(errno) << endl;
        }
    }
    return result;
}

// Map a language identifier to a code page. I have not been able to find
// an authoritative mapping between the LID and code page, so this
// one is drawn from wv/text.c. Corrections are welcome!

const char *MsWord::lid2codepage(U16 lid)
{
    static const char *cp874 = "CP874";
    static const char *cp932 = "CP932";
    static const char *cp936 = "CP936";
    static const char *cp949 = "CP949";
    static const char *cp950 = "CP950";
    static const char *cp1250 = "CP1250";
    static const char *cp1251 = "CP1251";
    static const char *cp1252 = "CP1252";
    static const char *cp1253 = "CP1253";
    static const char *cp1254 = "CP1254";
    static const char *cp1255 = "CP1255";
    static const char *cp1256 = "CP1256";
    static const char *cp1257 = "CP1257";
    static const char *unknown = "not known";

    switch (lid)
    {
    case 0x0401:    /*Arabic*/                      return cp1256;
    case 0x0402:    /*Bulgarian*/                   return cp1251;
    case 0x0403:    /*Catalan*/                     return cp1252;
    case 0x0404:    /*Traditional Chinese*/         return cp950;
    case 0x0804:    /*Simplified Chinese*/          return cp936;
    case 0x0405:    /*Czech*/                       return cp1250;
    case 0x0406:    /*Danish*/                      return cp1252;
    case 0x0407:    /*German*/                      return cp1252;
    case 0x0807:    /*Swiss German*/                return cp1252;
    case 0x0408:    /*Greek*/                       return cp1253;
    case 0x0409:    /*U.S. English*/                return cp1252;
    case 0x0809:    /*U.K. English*/                return cp1252;
    case 0x0c09:    /*Australian English*/          return cp1252;
    case 0x040a:    /*Castilian Spanish*/           return cp1252;
    case 0x080a:    /*Mexican Spanish*/             return cp1252;
    case 0x040b:    /*Finnish*/                     return cp1252;
    case 0x040c:    /*French*/                      return cp1252;
    case 0x080c:    /*Belgian French*/              return cp1252;
    case 0x0c0c:    /*Canadian French*/             return cp1252;
    case 0x100c:    /*Swiss French*/                return cp1252;
    case 0x040d:    /*Hebrew*/                      return cp1255;
    case 0x040e:    /*Hungarian*/                   return cp1250;
    case 0x040f:    /*Icelandic*/                   return cp1252;
    case 0x0410:    /*Italian*/                     return cp1252;
    case 0x0810:    /*Swiss Italian*/               return cp1252;
    case 0x0411:    /*Japanese*/                    return cp932;
    case 0x0412:    /*Korean*/                      return cp949;
    case 0x0413:    /*Dutch*/                       return cp1252;
    case 0x0813:    /*Belgian Dutch*/               return cp1252;
    case 0x0414:    /*Norwegian - Bokmal*/          return cp1252;
    case 0x0814:    /*Norwegian - Nynorsk*/         return cp1252;
    case 0x0415:    /*Polish*/                      return cp1250;
    case 0x0416:    /*Brazilian Portuguese*/        return cp1252;
    case 0x0816:    /*Portuguese*/                  return cp1252;
    case 0x0417:    /*Rhaeto-Romanic*/              return cp1252;
    case 0x0418:    /*Romanian*/                    return cp1250;
    case 0x0419:    /*Russian*/                     return cp1251;
    case 0x041a:    /*Croato-Serbian (Latin)*/      return cp1250;
    case 0x081a:    /*Serbo-Croatian (Cyrillic) */  return cp1252;
    case 0x041b:    /*Slovak*/                      return cp1250;
    case 0x041c:    /*Albanian*/                    return cp1251;
    case 0x041d:    /*Swedish*/                     return cp1250;
    case 0x041e:    /*Thai*/                        return cp874;
    case 0x041f:    /*Turkish*/                     return cp1254;
    case 0x0420:    /*Urdu*/                        return cp1256;
    case 0x0421:    /*Bahasa*/                      return cp1256;
    case 0x0422:    /*Ukrainian*/                   return cp1251;
    case 0x0423:    /*Byelorussian*/                return cp1251;
    case 0x0424:    /*Slovenian*/                   return cp1250;
    case 0x0425:    /*Estonian*/                    return cp1257;
    case 0x0426:    /*Latvian*/                     return cp1257;
    case 0x0427:    /*Lithuanian*/                  return cp1257;
    case 0x0429:    /*Farsi*/                       return cp1256;
    case 0x042D:    /*Basque*/                      return cp1252;
    case 0x042F:    /*Macedonian*/                  return cp1251;
    case 0x0436:    /*Afrikaans*/                   return cp1252;
    case 0x043E:    /*Malaysian*/                   return cp1251;
    default: return unknown;
    }
}

void MsWord::constructionError(unsigned line, const char *reason)
{
    m_constructionError="[" + QString(__FILE__ ) + ":" + QString::number(line) + "]" + reason;
    kdError(s_area) << m_constructionError << endl;
}

void MsWord::decodeParagraph(const QString &text, MsWord::PHE &layout, MsWord::PAPXFKP &style)
{
    Paragraph paragraph = Paragraph(*this);

    // Work out the paragraph details.

    paragraph.apply(style);
    paragraph.apply(layout);

    // We treat table paragraphs somewhat differently...so deal with
    // them first.

    if (paragraph.m_pap.fInTable)
    {
        if (!m_wasInTable)
        {
            gotTableBegin();
            m_tableColumn = 0;
        }
        m_wasInTable = true;

        // When we get to the end of the row, output the whole lot.

        if (paragraph.m_pap.fTtp)
        {
            MsWord::TAP tap;

            // A TAP describes the row.

            memset(&tap, 0, sizeof(tap));
            paragraph.apply(style.grpprl, style.grpprlBytes, &tap);
            gotTableRow(m_tableText, m_tableStyle, tap);
            m_tableColumn = 0;
        }
        else
        {
            m_tableText[m_tableColumn] = text;
            m_tableStyle[m_tableColumn] = paragraph.m_pap;
            m_tableColumn++;
        }
        return;
    }
    if (m_wasInTable)
        gotTableEnd();
    m_wasInTable = false;

    // What kind of paragraph was this?

    if ((paragraph.m_pap.istd >= 1) && (paragraph.m_pap.istd <= 9))
    {
        gotHeadingParagraph(text, paragraph.m_pap);
    }
    else
    if (paragraph.m_pap.ilfo)
    {
        const U8 *ptr = m_tableStream + m_fib.fcPlfLfo; //lcbPlfLfo.
        const U8 *ptr2;
        const U8 *ptr3;
        U32 lfoCount;
        int i;

        // Find the number of LFOs.

        ptr += MsWordGenerated::read(ptr, &lfoCount);
        ptr2 = ptr + lfoCount * sizeof(LFO);
        if (lfoCount < paragraph.m_pap.ilfo)
            kdError(s_area) << "MsWord::decodeParagraph: error finding LFO[" <<
                paragraph.m_pap.ilfo << "]" << endl;

        // Skip all the LFOs before our one, so that we can traverse the variable
        // length LFOLVL arrays.

        for (i = 1; i < paragraph.m_pap.ilfo; i++)
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
                    ptr2 += read(m_fib.lid, ptr2, &numberText, numberTextLength, true);
                }
            }
        }

        // We have found the LFO from its 1-based array. Check to see if there are any
        // overrides for this particular level.

        LFO data;

        // Read our LFO, apply the LSTF and then search any LFOLVLs for a matching level.

        ptr += MsWordGenerated::read(ptr, &data);
        paragraph.apply(data);
        for (i = 0; i < data.clfolvl; i++)
        {
            LFOLVL levelOverride;
            LVLF level;
            U16 numberTextLength;
            QString numberText;

            ptr2 += MsWordGenerated::read(ptr2, &levelOverride);
            if (levelOverride.fFormatting)
            {
                ptr2 += MsWordGenerated::read(ptr2, &level);
                ptr3 = ptr2;
                ptr2 += level.cbGrpprlPapx;
                ptr2 += level.cbGrpprlChpx;
                ptr2 += MsWordGenerated::read(ptr2, &numberTextLength);
                ptr2 += read(m_fib.lid, ptr2, &numberText, numberTextLength, true);
            }

            // If this LFOLVL is ours, we are done!

            if (paragraph.m_pap.ilvl == levelOverride.ilvl)
            {
                // If the LFOLVL was not a complete override, resort to the LVLF
                // for whatever is missing.

                if (levelOverride.fFormatting)
                {
                    // Apply the grpprl.

                    kdDebug(s_area) << "getting formatting from LVLF" << endl;
                    paragraph.apply(ptr3, level.cbGrpprlPapx);

                    // Apply the startAt.

                    paragraph.m_pap.anld.iStartAt = level.iStartAt;
                    kdDebug(s_area) << "got startAt " << paragraph.m_pap.anld.iStartAt <<
                        " from LVLF" << endl;
                }
                else
                if (levelOverride.fStartAt)
                {
                    // Apply the startAt.

                    paragraph.m_pap.anld.iStartAt = levelOverride.iStartAt;
                    kdDebug(s_area) << "got startAt " << paragraph.m_pap.anld.iStartAt <<
                        " from LFOLVL" << endl;
                }
                break;
            };
        }

        // TBD: We often seem to get invalid nfc's. Map them to a safe value.
        if (paragraph.m_pap.anld.nfc > 5)
            paragraph.m_pap.anld.nfc = 5;
        gotListParagraph(text, paragraph.m_pap);
    }
    else
    {
        gotParagraph(text, paragraph.m_pap);
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

    // Get word offset to the second piece of data, and the first piece of data
    // if required.

    m_dataNext += MsWordGenerated::read(m_dataNext, rgb);
    if (data1)
        m_dataNext += read(m_fib.nFib, m_dataNext, data1);

    if (!(*rgb))
    {
        // If the word offset is zero, then the second piece of data is
        // not explicitly stored.
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

void MsWord::getCHPXFKP()
{
    // A bin table is a plex of BTEs.

    Plex<BTE, 2> btes = Plex<BTE, 2>(m_fib);
    U32 startFc;
    U32 endFc;
    BTE data;

    // Walk the BTEs.

    btes.startIteration(m_tableStream + m_fib.fcPlcfbteChpx, m_fib.lcbPlcfbteChpx);
    while (btes.getNext(&startFc, &endFc, &data))
    {
        getCHPX(m_mainStream + (data.pn * 512));
    }
}

void MsWord::getCHPX(const U8 *fkp)
{
    // A CHPX FKP contains no extra data, specify a dummy PHE for the template.

    Fkp<PHE, CHPXFKP> chpx = Fkp<PHE, CHPXFKP>(m_fib);

    U32 startFc;
    U32 endFc;
    U8 rgb;
    CHPXFKP style;

    chpx.startIteration(fkp);
    while (chpx.getNext(&startFc, &endFc, &rgb, NULL, &style))
    {
        //kdDebug(s_area) << "chp from: " << startFc << ".." << endFc << ": rgb: " << rgb << endl;
    }
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

        //kdDebug(s_area) << "pap from: " << startFc << ".." << endFc << ": rgb: " << rgb << endl;
        read(m_fib.lid, m_mainStream + startFc, &text, endFc - startFc, unicode);
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

    kdDebug(s_area) << "MsWord::getListStyles" << endl;

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
            ptr2 += read(m_fib.lid, ptr2, &numberText, numberTextLength, true);
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

    kdDebug(s_area) << "MsWord::getStyles" << endl;

    // Failsafe for simple documents.

    m_styles = NULL;
    if (!m_fib.lcbStshf)
    {
        kdError(s_area) << "MsWord::getStyles: no data " << endl;
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

    m_styles = new Paragraph *[stshi.cstd];
    for (unsigned i = 0; i < stshi.cstd; i++)
    {
        U16 cbStd;
        STD std;

        ptr += MsWordGenerated::read(ptr, &cbStd);
        if (cbStd)
        {
            read(m_fib.lid, ptr, stshi.cbSTDBaseInFile, &std);
            kdDebug(s_area) << "MsWord::getStyles: style: " << std.xstzName <<
                ", types: " << std.cupx <<
                endl;

            // If this is a paragraph style, fill it.

            if (std.sgc == 1)
            {
                m_styles[i] = new Paragraph(*this);
                m_styles[i]->apply(std);
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
    kdDebug(s_area) << "MsWord::MsWord: lid: " << m_fib.lid << " lidFE: " << m_fib.lidFE << endl;
    kdDebug(s_area) << "MsWord::MsWord: fExtChar: " << m_fib.fExtChar << endl;

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
    // TBD: implement char properties: getCHPXFKP();
}

MsWord::~MsWord()
{
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
            kdDebug(s_area) << "piece table from: " << startFc << ".." << endFc << endl;
            unicode = ((data.fc & codepage1252mask) != codepage1252mask);
            //unicode = unicode || m_fib.fExtChar;
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
unsigned MsWord::read(U16 lid, const U8 *in, QString *out, unsigned count, bool unicode)
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
            *out += QChar(char2unicode(lid, char8));
        }
    }

    // If the string ends in a CR, strip it off.

    if (!unicode && (char8 == '\r'))
        out->truncate(count - 1);
    return bytes;
}

//
// Read a CHPX as stored in a FKP.
//
unsigned MsWord::read(unsigned nFib, const U8 *in, CHPXFKP *out)
{
    unsigned bytes = 0;

    bytes += MsWordGenerated::read(in + bytes, &out->grpprlBytes);
    out->grpprl = (U8 *)(in + bytes);
    bytes += out->grpprlBytes;
    return bytes;
}

//
// Read a PAPX as stored in a FKP.
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
        // TBD: The spec says that the Word6 istd is a byte, but that seems to be wrong.
        bytes += MsWordGenerated::read(in + bytes, &out->istd);
        out->grpprl = (U8 *)(in + bytes);
        bytes += out->grpprlBytes;
    }
    return bytes;
}

unsigned MsWord::read(U16 lid, const U8 *in, unsigned baseInFile, STD *out, unsigned count)
{
    unsigned long shiftRegister;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        U8 offset;

        offset = 0;
        offset += MsWordGenerated::read(in + offset, (U16 *)&shiftRegister);
        out->sti = shiftRegister;
        shiftRegister >>= 12;
        out->fScratch = shiftRegister;
        shiftRegister >>= 1;
        out->fInvalHeight = shiftRegister;
        shiftRegister >>= 1;
        out->fHasUpe = shiftRegister;
        shiftRegister >>= 1;
        out->fMassCopy = shiftRegister;
        shiftRegister >>= 1;
        offset += MsWordGenerated::read(in + offset, (U16 *)&shiftRegister);
        out->sgc = shiftRegister;
        shiftRegister >>= 4;
        out->istdBase = shiftRegister;
        shiftRegister >>= 12;
        offset += MsWordGenerated::read(in + offset, (U16 *)&shiftRegister);
        out->cupx = shiftRegister;
        shiftRegister >>= 4;
        out->istdNext = shiftRegister;
        shiftRegister >>= 12;
        offset += MsWordGenerated::read(in + offset, &out->bchUpe); // U16
        offset += MsWordGenerated::read(in + offset, (U16 *)&shiftRegister);
        out->fAutoRedef = shiftRegister;
        shiftRegister >>= 1;
        out->fHidden = shiftRegister;
        shiftRegister >>= 1;
        out->unused8_3 = shiftRegister;
        shiftRegister >>= 14;
        memset(out + bytes + baseInFile, 0, 10 - baseInFile);
        offset -= 10 - baseInFile;

        // If the baseInFile is less than 10, then the style name is not stored in unicode!

        if (baseInFile < 10)
        {
            U8 nameLength;
            U8 terminator;

            offset += MsWordGenerated::read(in + offset, &nameLength);
            offset += read(lid, in + offset, &out->xstzName, nameLength, false);
            offset += MsWordGenerated::read(in + offset, &terminator);
        }
        else
        {
            U16 nameLength;
            U16 terminator;

            offset += MsWordGenerated::read(in + offset, &nameLength);
            offset += read(lid, in + offset, &out->xstzName, nameLength, true);
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
    unsigned long shiftRegister;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        // What version of Word are we dealing with?
        // Word 6 for the PC writes files with nFib = 101-102.
        // Word 6 for the Mac writes files with nFib = 103-104.
        // Word 8 (a.k.a. Winword 97) and later products write files with nFib > 105.

        MsWordGenerated::read(in + 2, &out->nFib);
        if (out->nFib > s_maxWord6Version)
        {
            bytes += MsWordGenerated::read(in + bytes, out);
        }
        else
        if (out->nFib > s_minWordVersion)
        {
            // We will convert the FIB into the same form as for Winword 7.

            memset(out, sizeof(*out), 0);
            bytes += MsWordGenerated::read(in + bytes, &out->wIdent); // U16
            bytes += MsWordGenerated::read(in + bytes, &out->nFib); // U16
            bytes += MsWordGenerated::read(in + bytes, &out->nProduct); // U16
            bytes += MsWordGenerated::read(in + bytes, &out->lid); // U16
            bytes += MsWordGenerated::read(in + bytes, &out->pnNext); // U16
            bytes += MsWordGenerated::read(in + bytes, (U16 *)&shiftRegister);
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
            bytes += MsWordGenerated::read(in + bytes, &out->nFibBack); // U16
            bytes += MsWordGenerated::read(in + bytes, &out->lKey); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->envr); // U8
            bytes += MsWordGenerated::read(in + bytes, (U8 *)&shiftRegister);
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
            bytes += MsWordGenerated::read(in + bytes, &out->chs); // U16
            bytes += MsWordGenerated::read(in + bytes, &out->chsTables); // U16
            bytes += MsWordGenerated::read(in + bytes, &out->fcMin); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcMac); // U32
            out->csw = 14;
            out->lidFE = out->lid;
            out->clw = 22;
            bytes += MsWordGenerated::read(in + bytes, &out->cbMac);
            bytes += 16;

            // ccpText through ccpHdrTxbx.

            bytes += MsWordGenerated::read(in + bytes, &out->ccpText); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->ccpFtn); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->ccpHdd); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->ccpMcr); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->ccpAtn); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->ccpEdn); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->ccpTxbx); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->ccpHdrTxbx); // U32

            // ccpSpare2.

            bytes += 4;
            out->cfclcb = 93;

            // fcStshfOrig through lcbSttbfAtnbkmk.

            bytes += MsWordGenerated::read(in + bytes, &out->fcStshfOrig); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbStshfOrig); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcStshf); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbStshf); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcffndRef); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcffndRef); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcffndTxt); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcffndTxt); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfandRef); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfandRef); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfandTxt); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfandTxt); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfsed); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfsed); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcpad); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcpad); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfphe); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfphe); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcSttbfglsy); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbSttbfglsy); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfglsy); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfglsy); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfhdd); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfhdd); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfbteChpx); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfbteChpx); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfbtePapx); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfbtePapx); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfsea); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfsea); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcSttbfffn); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbSttbfffn); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcffldMom); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcffldMom); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcffldHdr); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcffldHdr); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcffldFtn); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcffldFtn); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcffldAtn); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcffldAtn); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcffldMcr); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcffldMcr); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcSttbfbkmk); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbSttbfbkmk); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfbkf); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfbkf); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfbkl); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfbkl); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcCmds); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbCmds); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcmcr); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcmcr); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcSttbfmcr); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbSttbfmcr); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPrDrvr); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPrDrvr); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPrEnvPort); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPrEnvPort); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPrEnvLand); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPrEnvLand); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcWss); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbWss); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcDop); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbDop); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcSttbfAssoc); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbSttbfAssoc); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcClx); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbClx); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfpgdFtn); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfpgdFtn); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcAutosaveSource); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbAutosaveSource); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcGrpXstAtnOwners); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbGrpXstAtnOwners); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcSttbfAtnbkmk); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbSttbfAtnbkmk); // U32

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

            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcdoaMom); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcdoaMom); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcdoaHdr); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcdoaHdr); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcspaMom); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcspaMom); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcspaHdr); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcspaHdr); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfAtnbkf); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfAtnbkf); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfAtnbkl); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfAtnbkl); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPms); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPms); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcFormFldSttbs); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbFormFldSttbs); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfendRef); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfendRef); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfendTxt); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfendTxt); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcffldEdn); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcffldEdn); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfpgdEdn); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfpgdEdn); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcDggInfo); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbDggInfo); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcSttbfRMark); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbSttbfRMark); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcSttbCaption); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbSttbCaption); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcSttbAutoCaption); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbSttbAutoCaption); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfwkb); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfwkb); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfspl); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfspl); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcftxbxTxt); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcftxbxTxt); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcffldTxbx); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcffldTxbx); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfhdrtxbxTxt); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfhdrtxbxTxt); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPlcffldHdrTxbx); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcffldHdrTxbx); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcStwUser); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbStwUser); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcSttbttmbd); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->cbSttbttmbd); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcUnused); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbUnused); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPgdMother); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPgdMother); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcBkdMother); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbBkdMother); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPgdFtn); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPgdFtn); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcBkdFtn); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbBkdFtn); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcPgdEdn); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbPgdEdn); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcBkdEdn); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbBkdEdn); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcSttbfIntlFld); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbSttbfIntlFld); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcRouteSlip); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbRouteSlip); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcSttbSavedBy); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbSttbSavedBy); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->fcSttbFnm); // U32
            bytes += MsWordGenerated::read(in + bytes, &out->lcbSttbFnm); // U32
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
    unsigned bytes = 0;
    U16 tmp;

    if (nFib > s_maxWord6Version)
    {
        bytes += MsWordGenerated::read(in + bytes, out);
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
    unsigned bytes = 0;

    bytes += MsWordGenerated::read(in + bytes, out);
    return bytes;
} // PCD

unsigned MsWord::read(unsigned nFib, const U8 *in, PHE *out)
{
    unsigned bytes = 0;
    unsigned long shiftRegister;
    U16 tmp;

    if (nFib > s_maxWord6Version)
    {
        bytes += MsWordGenerated::read(in + bytes, out);
    }
    else
    {
        bytes += MsWordGenerated::read(in + bytes, (U16 *)&shiftRegister);
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
        bytes += MsWordGenerated::read(in + bytes, &tmp);
        out->dxaCol = tmp;
        bytes += MsWordGenerated::read(in + bytes, &tmp);
        out->dym = tmp;
    }
    return bytes;
} // PHE

