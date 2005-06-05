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
    but some of the algorithms to workaround broken specifications etc. come
    from the pioneering work on the "wvWare" library by Caolan McNamara (see
    http://www.wvWare.com).
*/

#include <kdebug.h>
#include <msword.h>
#include <myfile.h>
#include <properties.h>
#include <qtextcodec.h>
#include <string.h>

//#define CHAR_DEBUG

const int MsWord::s_area = 30513;
const unsigned MsWord::s_minWordVersion = 100;
const unsigned MsWord::s_maxWord6Version = 105;
const unsigned MsWord::s_maxWord7Version = 193;

// We currently only take note of the document's main non-Far Eastern
// language, and ignore character properties. TBD: remove these restrictions!

QString MsWord::char2unicode(unsigned lid, char c)
{
    static QTextCodec *codec = (QTextCodec *)0;
    static unsigned lastLid = (unsigned)-1;

    // Is the language changing?
    if (lastLid != lid)
    {
        const char *codepage;

        // Unconditionally set the new language - this will prevent
        // an error message for each character if things go wrong!
        lastLid = lid;

        // Find the name of the new code page and open the codec.
        codepage = lid2codepage(lastLid);
        codec = QTextCodec::codecForName(codepage);

        if (codec)
        {
            kdDebug(s_area) <<
                "converting " << codepage <<
                " to Unicode" << endl;
        }
        else
        {
            kdError(s_area) <<
                "cannot convert " << codepage <<
                " to Unicode" << endl;
        }
    }

    // Do the conversion!

    QString result;

    if (codec)
        result = codec->toUnicode(&c, 1);
    else
        result = '?';

    // KWord (?) can't handle the " <- if it's like this, up, it just
    // always draws the bottom-" , the real " is interpreted as shit
    // in some documents (mostly german ones)
    // so let's force the bottom-" for the moment (Niko)

    // UNICODE WORKAROUNDS
    // If you see a wrong character, get the unicode specs
    // convert the containing hex values to dec values
    // and use CHAR_DEBUG to find out the unicode value of
    // the wrong char and replace them (Niko)
    if (result[0].unicode() == 8222 || result[0].unicode() == 8221)
        result[0] = QChar(8220);

    if (result[0].unicode() == 8217)
        result[0] = QChar(39);

#ifdef CHAR_DEBUG
    kdDebug() << "text: " << result << endl;
    kdDebug() << "unicode value: " << result[0].unicode() << endl;
#endif

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
    case 0x0c0a:    /*Traditional Spanish*/         return cp1252; // TBD: Undocumented!
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
    case 0x0418:    /*Romanian*/                    return cp1252;
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

void MsWord::decodeParagraph(
    const QString &text,
    MsWord::PHE &layout,
    MsWord::PAPXFKP &style,
    CHPXarray &chpxs)
{
    Properties properties = Properties(*this);

    // Work out the paragraph details.

    properties.apply(style);
    properties.apply(layout);

    // We treat table paragraphs somewhat differently...so deal with
    // them first.

    if (properties.m_pap.fInTable)
    {
        if (!m_wasInTable)
        {
            gotTableBegin();
            m_tableColumn = 0;
        }
        m_wasInTable = true;

        // When we get to the end of the row, output the whole lot.

        if (properties.m_pap.fTtp)
        {
            gotTableRow(m_tableText, m_tableStyle, m_tableRuns, properties.m_tap);
            m_tableColumn = 0;
        }
        else
        {
            m_tableText[m_tableColumn] = text;
            m_tableStyle[m_tableColumn] = properties.m_pap;
            m_tableRuns[m_tableColumn] = chpxs;
            m_tableColumn++;
        }
        return;
    }
    if (m_wasInTable)
        gotTableEnd();
    m_wasInTable = false;

    // What kind of paragraph was this?
    if ((properties.m_pap.istd >= 1) && (properties.m_pap.istd <= 9))
    {
        gotHeadingParagraph(text, properties.m_pap, chpxs);
    }
    else
    if (properties.m_pap.ilfo)
    {
        const U8 *ptr = m_tableStream + m_fib.fcPlfLfo; //lcbPlfLfo.
        const U8 *ptr2;
        const U8 *ptr3 = 0L;
        S32 lfoCount;
        int i;

        // Find the number of LFOs.

        ptr += MsWordGenerated::read(ptr, &lfoCount);
        ptr2 = ptr + lfoCount * MsWordGenerated::sizeof_LFO;
        if (lfoCount < properties.m_pap.ilfo)
            kdError(s_area) << "MsWord::decodeParagraph: error finding LFO[" <<
                properties.m_pap.ilfo << "]" << endl;

        // Skip all the LFOs before our one, so that we can traverse the
        // variable length LFOLVL arrays.

        for (i = 1; i < properties.m_pap.ilfo; i++)
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
                    ptr2 += read(m_fib.lid, ptr2, &numberText, numberTextLength, true, m_fib.nFib);
                }
            }
        }

        // We have found the LFO from its 1-based array. Check to see if there
        // are any overrides for this particular level.

        LFO data;

        // Read our LFO, apply the LSTF and then search any LFOLVLs for a
        // matching level.

        ptr += MsWordGenerated::read(ptr, &data);
        properties.apply(data);
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
                ptr2 += read(m_fib.lid, ptr2, &numberText, numberTextLength, true, m_fib.nFib);
            }

            // If this LFOLVL is ours, we are done!

            if (properties.m_pap.ilvl == levelOverride.ilvl)
            {
                // If the LFOLVL was not a complete override, resort to the LVLF
                // for whatever is missing.

                if (levelOverride.fFormatting)
                {
                    // Apply the grpprl.

                    kdDebug(s_area) << "getting formatting from LVLF" << endl;
                    properties.apply(ptr3, level.cbGrpprlPapx);

                    // Apply the startAt.

                    properties.m_pap.anld.iStartAt = level.iStartAt;
                    kdDebug(MsWord::s_area) << "startAt from LVLF:" <<
                        properties.m_pap.anld.iStartAt << endl;
                }
                else
                if (levelOverride.fStartAt)
                {
                    // Apply the startAt.

                    properties.m_pap.anld.iStartAt = levelOverride.iStartAt;
                    kdDebug(s_area) << "startAt from LFOLVL:" <<
                        properties.m_pap.anld.iStartAt << endl;
                }
                break;
            };
        }

        // TBD: We often seem to get invalid nfc's. Map them to a safe value.
        if (properties.m_pap.anld.nfc > 5)
            properties.m_pap.anld.nfc = 5;
        gotListParagraph(text, properties.m_pap, chpxs);
    }
    else
    {
        gotParagraph(text, properties.m_pap, chpxs);
    }
}

template <class T1, class T2>
MsWord::Fkp<T1, T2>::Fkp(MsWord *document) :
    m_document(document)
{
}

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
        m_dataNext += m_document->read(m_dataNext, data1);

    if (!(*rgb))
    {
        // If the word offset is zero, then the second piece of data is
        // not explicitly stored.
    }
    else
    {
        // Get the second piece of data.
        m_document->read(m_fkp + (2 * (*rgb)), data2);
    }
    return (m_i++ < m_crun);
}

template <class T1, class T2>
void MsWord::Fkp<T1, T2>::startIteration(const U8 *fkp)
{
    // Get the number of entries in the FKP.

    m_fkp = fkp;
    MsWordGenerated::read(m_fkp + 511, &m_crun);
    m_fcNext = m_fkp;
    m_dataNext = m_fkp + ((m_crun + 1) * MsWordGenerated::sizeof_U32);
    m_i = 0;
}

const MsWordGenerated::FIB &MsWord::fib() const
{
    return m_fib;
}

// Get the character property exceptions for a range of file positions by
// walking the BTEs. The result is an array of CHPXs which start and end at the
// given range.
void MsWord::getChpxs(U32 startFc, U32 endFc, CHPXarray &result)
{
    // A bin table is a plex of BTEs.

    Plex<BTE, 2, MsWordGenerated::sizeof_BTE> btes = Plex<BTE, 2, MsWordGenerated::sizeof_BTE>(this);
    U32 actualStartFc;
    U32 actualEndFc;
    BTE data;

    // Walk the BTEs.

    btes.startIteration(m_tableStream + m_fib.fcPlcfbteChpx, m_fib.lcbPlcfbteChpx);
    while (btes.getNext(&actualStartFc, &actualEndFc, &data))
    {
        getChpxs(m_mainStream + (data.pn * 512), startFc, endFc, result);
    }

    // Tailor the result array to the caller's request.

    unsigned index = result.size();

    if (index == 0)
    {
        kdError(s_area) << "MsWord::getChpxs: cannot find entries for " << startFc << ".." << endFc << endl;

        // Recover by making up a dummy entry.

        CHPX style;

        style.startFc = startFc;
        style.endFc = endFc;
        style.data.count = 0;
        style.data.ptr = (U8 *)0;
        result.resize(1);
        result[1] = style;
    }
    else
    {
        //kdDebug(s_area) << "using chp from: " << result[0].startFc << ".." << result[index - 1].endFc << endl;
        result[0].startFc = startFc;
        result[index - 1].endFc = endFc;
    }
}

// Get the character property exceptions for a range of file positions in an FKP.
// The result is an array of CHPXs which start and end at the given range.
void MsWord::getChpxs(const U8 *fkp, U32 startFc, U32 endFc, CHPXarray &result)
{
    // A CHPX FKP contains no extra data, specify a dummy PHE for the template.

    Fkp<PHE, CHPXFKP> chpx = Fkp<PHE, CHPXFKP>(this);

    U8 rgb;
    CHPX style;

    chpx.startIteration(fkp);
    while (chpx.getNext(&style.startFc, &style.endFc, &rgb, NULL, &style.data))
    {
        if (style.endFc <= startFc)
        {
            // This one ends before the region of interest.

            continue;
        }
        else
        if (endFc <= style.startFc)
        {
            // This one starts after the area of interest...we are done!

            break;
        }

        // Add this to the result array.

        //kdDebug(s_area) << "found chp from: " << style.startFc << ".." << style.endFc << ": rgb: " << rgb << endl;
        if (!rgb)
        {
            style.data.count = 0;
            style.data.ptr = (U8 *)0;
        }

        unsigned index = result.size();

        result.resize(index + 1);
        result[index] = style;
    }
}

// Get a FLD.

void MsWord::getField(
    U32 anchorCp,
    U8 *fieldType)
{
    // A field table is a plex of FLDs.

    Plex<FLD, MsWordGenerated::sizeof_FLD, MsWordGenerated::sizeof_FLD> flds = Plex<FLD, MsWordGenerated::sizeof_FLD, MsWordGenerated::sizeof_FLD>(this);
    U32 actualStartCp;
    U32 actualEndCp;
    FLD data;

    // Walk the FLDs.

    *fieldType = 0;
    flds.startIteration(m_tableStream + m_fib.fcPlcffldMom, m_fib.lcbPlcffldMom);
    while (flds.getNext(&actualStartCp, &actualEndCp, &data))
    {
        if (actualStartCp == anchorCp)
        {
            *fieldType = data.flt;
            break;
        }
    }
}

// Get a font.

const MsWord::FFN &MsWord::getFont(unsigned fc)
{
    if (fc >= m_fonts.count)
    {
        kdError(s_area) << "MsWord::getFont: invalid font code: " << fc << endl;
        fc = 0;
    }
    return m_fonts.data[fc];
}

// Embedded object access.

void MsWord::getObject(
    U32 fc,
    QString &/*mimeType*/)
{
    const U8 *in = m_dataStream + fc;
    OBJHEADER data;

    // Get the OBJHEADER.

    MsWordGenerated::read(in, &data);

    // Skip the complete OBJHEADER, even if it is longer than we expect.

    in += data.cbHeader;
}

// Get a piece of Office art by walking the FSPAs.

bool MsWord::getOfficeArt(
    U32 anchorCp,
    FSPA &result,
    unsigned *pictureLength,
    const U8 **pictureData,
    const U8 **delayData)
{
    // A spa table is a plex of FSPAs.

    Plex<FSPA, MsWordGenerated::sizeof_FSPA, MsWordGenerated::sizeof_FSPA> fspas = Plex<FSPA, MsWordGenerated::sizeof_FSPA, MsWordGenerated::sizeof_FSPA>(this);
    U32 actualStartCp;
    U32 actualEndCp;

    // Walk the FSPAs.

    *pictureData = 0L;
    *pictureLength = 0;
    *delayData = 0L;
    fspas.startIteration(m_tableStream + m_fib.fcPlcspaMom, m_fib.lcbPlcspaMom);
    while (fspas.getNext(&actualStartCp, &actualEndCp, &result))
    {
        if (actualStartCp == anchorCp)
        {
            *pictureLength = (unsigned)m_fib.lcbDggInfo;
            *pictureData = m_tableStream + m_fib.fcDggInfo;

            // TBD: somewhat surprisingly, the m_mainStream contains these!
            *delayData = m_mainStream;
            return true;
        }
    }
    return false;
}

// Walk a FKP of BTEs outputting text.
void MsWord::getParagraphsFromBtes(U32 startFc, U32 endFc, bool unicode)
{
    // A bin table is a plex of BTEs.

    Plex<BTE, 2, MsWordGenerated::sizeof_BTE> btes = Plex<BTE, 2, MsWordGenerated::sizeof_BTE>(this);
    U32 actualStartFc;
    U32 actualEndFc;
    BTE data;

    // Walk the BTEs.

    btes.startIteration(m_tableStream + m_fib.fcPlcfbtePapx, m_fib.lcbPlcfbtePapx);
    while (btes.getNext(&actualStartFc, &actualEndFc, &data))
    {
        if (actualEndFc <= startFc)
        {
            // This one ends before the region of interest.

            continue;
        }
        else
        if (endFc <= actualStartFc)
        {
            // This one starts after the area of interest...we are done!

            break;
        }

        // Tailor the result array to the caller's request.

        //kdDebug(s_area) << "bte for FCs: " << startFc << ".." << endFc <<
            //" actual FCs: " << actualStartFc << ".." << actualEndFc << endl;
        if (actualStartFc < startFc)
            actualStartFc = startFc;
        if (actualEndFc > endFc)
            actualEndFc = endFc;
        getParagraphsFromPapxs(
            m_mainStream + (data.pn * 512),
            actualStartFc,
            actualEndFc,
            unicode);
    }
}

// Walk a FKP of PAPXs outputting text.
void MsWord::getParagraphsFromPapxs(
    const U8 *fkp,
    U32 startFc,
    U32 endFc,
    bool unicode)
{
    // A PAPX FKP contains PHEs.

    Fkp<PHE, PAPXFKP> papx = Fkp<PHE, PAPXFKP>(this);

    U32 actualStartFc;
    U32 actualEndFc;
    U8 rgb;
    PHE layout;
    PAPXFKP style;

    // Decode main body text.

    {
        papx.startIteration(fkp);
        while (papx.getNext(&actualStartFc, &actualEndFc, &rgb, &layout, &style))
        {
            QString text;
            CHPXarray chpxs;
            bool discardedEnd;

            //kdDebug(s_area) << "pap from: " << actualStartFc << ".." << actualEndFc << ": rgb: " << rgb << endl;
            if (actualEndFc <= startFc)
            {
                // This one ends before the region of interest.

                continue;
            }
            else
            if (endFc <= actualStartFc)
            {
                // This one starts after the area of interest...we are done!

                break;
            }

            // Tailor the result array to the caller's request.

            //kdDebug(s_area) << "text for FCs: " << startFc << ".." << endFc <<
                //" actual FCs: " << actualStartFc << ".." << actualEndFc << endl;
            if (actualStartFc < startFc)
                actualStartFc = startFc;
            discardedEnd = false;
            if (actualEndFc > endFc)
            {
                actualEndFc = endFc;

                // Note that we will not use the end of the paragraph.

                discardedEnd = true;
            }

            // Read the text we are after, and get the CHPXs that apply to
            // the range of characters.
            read(
                m_fib.lid,
                m_mainStream + actualStartFc,
                &text,
                (actualEndFc - actualStartFc) / (unicode ? 2 : 1),
                unicode, m_fib.nFib);
            getChpxs(actualStartFc, actualEndFc, chpxs);

            // Adjust the end position to be in character count terms,
            // independent of the original encoding.

            unsigned i;
            unsigned length;

            for (i = 0; i < chpxs.size(); i++)
            {
                length = chpxs[i].endFc - chpxs[i].startFc;
                length /= (unicode ? 2 : 1);
                chpxs[i].endFc = chpxs[i].startFc + length;
            }

            // If we got to the end of the properties, output it. Otherwise, we
            // save it and its CHPXs away for next time around. TBD: Make sure
            // we output any trailing partial paragraph.

            unsigned extra;

            m_partialParagraph.text += text;
            length = m_partialParagraph.chpxs.size();
            extra = chpxs.size();
            m_partialParagraph.chpxs.resize(length + extra);
            for (i = 0; i < extra; i++)
                m_partialParagraph.chpxs[length + i] = chpxs[i];
            if (discardedEnd)
            {
                continue;
            }
            text = m_partialParagraph.text;
            chpxs = m_partialParagraph.chpxs;
            m_partialParagraph.text = "";
            m_partialParagraph.chpxs.resize(0);

            // Bias all the start and end positions to be in a monotonic
            // sequence starting from zero within the paragraph.

            for (i = 0; i < chpxs.size(); i++)
            {
                chpxs[i].endFc -= chpxs[i].startFc;
                chpxs[i].startFc = 0;
                if (i > 0)
                {
                    chpxs[i].endFc += chpxs[i - 1].endFc;
                    chpxs[i].startFc += chpxs[i - 1].endFc;
                }
            }

            // TBD: We only support main body text.

            //if (m_characterPosition < m_fib.ccpText)
            decodeParagraph(text, layout, style, chpxs);

            // Track what kind of text  comes next.

            m_characterPosition += text.length();
            if (m_characterPosition == m_fib.ccpText)
            {
                if (m_fib.ccpFtn)
                    kdDebug(s_area) << "MsWord::MsWord: start footnotes" << endl;
            }
            if (m_characterPosition == m_fib.ccpText + m_fib.ccpFtn)
            {
                if (m_fib.ccpHdd)
                    kdDebug(s_area) << "MsWord::MsWord: start headers" << endl;
                // TBD: find headers
                //const U8 *ptr = m_tableStream + m_fib.fcPlcfhdd;
                //for (unsigned i = 0; i < m_fib.lcbPlcfhdd / MsWordGenerated::sizeof_U32; i++)
                //{
                //    U32 cp;
                //
                //    ptr += MsWordGenerated::read(ptr, &cp);
                //    kdDebug(s_area) << "MsWord::MsWord: header:" << i << " " << cp << endl;
                //}
            }
            if (m_characterPosition == m_fib.ccpText + m_fib.ccpFtn + m_fib.ccpHdd)
            {
                if (m_fib.ccpAtn)
                    kdDebug(s_area) << "MsWord::MsWord: start annotations" << endl;
            }
            if (m_characterPosition == m_fib.ccpText + m_fib.ccpFtn + m_fib.ccpHdd +
                                        m_fib.ccpAtn)
            {
                if (m_fib.ccpEdn)
                    kdDebug(s_area) << "MsWord::MsWord: start endnotes" << endl;
            }
            if (m_characterPosition == m_fib.ccpText + m_fib.ccpFtn + m_fib.ccpHdd +
                                        m_fib.ccpAtn + m_fib.ccpEdn)
            {
                if (m_fib.ccpTxbx)
                    kdDebug(s_area) << "MsWord::MsWord: start text boxes" << endl;
            }
            if (m_characterPosition == m_fib.ccpText + m_fib.ccpFtn + m_fib.ccpHdd +
                                        m_fib.ccpAtn + m_fib.ccpEdn + m_fib.ccpTxbx)
            {
                if (m_fib.ccpHdrTxbx)
                    kdDebug(s_area) << "MsWord::MsWord: start header textboxes" << endl;
            }
            if (m_characterPosition == m_fib.ccpText + m_fib.ccpFtn + m_fib.ccpHdd +
                                        m_fib.ccpAtn + m_fib.ccpEdn + m_fib.ccpTxbx +
                                        m_fib.ccpHdrTxbx)
            {
            }
        }
    }
}

// Picture access.

bool MsWord::getPicture(
    U32 fc,
    QString &pictureType,
    U32 *pictureLength,
    const U8 **pictureData)
{
    const U8 *in = m_dataStream + fc;
    PICF data;
    unsigned bytes;
    QString tiffFilename;

    // sentinel check
    if( fc > m_dataStreamLength ) return false;

    // Get the PICF.

    pictureType = "";
    *pictureLength = 0;
    *pictureData = 0L;
    MsWordGenerated::read(in, &data);

    // Skip the complete PICF, even if it is longer than we exepct.

    in += data.cbHeader;
    *pictureLength = data.lcb - data.cbHeader;
    *pictureData = in;
    switch (data.mfp.mm)
    {
    case 98:
        pictureType = "tiff";
        bytes = MsWord::read(m_fib.lid, in, &tiffFilename, true, m_fib.nFib);
        in += bytes;
        *pictureLength -= bytes;
        *pictureData += bytes;
        break;
    case 99:
        pictureType = "bmp";
        break;
    default:
        kdDebug(s_area) << "MsWord::getPicture: mm: " << data.mfp.mm << endl;
        pictureType = "wmf";
        break;
    };
    //kdDebug(s_area) << "MsWord::getPicture: cbHeader: " << data.cbHeader << endl;
    //kdDebug(s_area) << "MsWord::getPicture: dxaGoal: " << (double)data.dxaGoal/1440 << endl;
    //kdDebug(s_area) << "MsWord::getPicture: dyaGoal: " << (double)data.dyaGoal/1440 << endl;
    //kdDebug(s_area) << "MsWord::getPicture: dxaOrigin: " << data.dxaOrigin << endl;
    //kdDebug(s_area) << "MsWord::getPicture: dyaOrigin: " << data.dyaOrigin << endl;
    //kdDebug(s_area) << "MsWord::getPicture: mm.x: " << data.mfp.xExt << endl;
    //kdDebug(s_area) << "MsWord::getPicture: mm.y: " << data.mfp.yExt << endl;
    //U8 *t =m_dataStream + fc;
    //if (0)
    //for (int i = 0; i < 67; i += 1)
    //{
    //    kdDebug() << i <<" "<< QString::number(t[i], 16) << endl;
    //}
    return (*pictureLength > 0);
}

// Walk the list of styles, outputting each in turn.

void MsWord::getStyles()
{
    for (unsigned i = 0; i < m_styles.count; i++)
    {
        gotStyle(m_styles.names[i], *m_styles.data[i]);
    }
}

MsWord::MsWord(
        const myFile &mainStream,
        const myFile &table0Stream,
        const myFile &table1Stream,
        const myFile &dataStream) :
    mainStream(mainStream),
    table0Stream(table0Stream),
    table1Stream(table1Stream),
    dataStream(dataStream)
{
    m_constructionError = QString("");
    m_fib.nFib = s_minWordVersion;
    read(mainStream.data, &m_fib);
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
    kdDebug(s_area) << "MsWord::MsWord: main text: " << m_fib.ccpText << endl;
    kdDebug(s_area) << "MsWord::MsWord: footnote text: " << m_fib.ccpFtn << endl;
    kdDebug(s_area) << "MsWord::MsWord: header text: " << m_fib.ccpHdd << endl;
    kdDebug(s_area) << "MsWord::MsWord: annotation text: " << m_fib.ccpAtn << endl;
    kdDebug(s_area) << "MsWord::MsWord: endnote text: " << m_fib.ccpEdn << endl;
    kdDebug(s_area) << "MsWord::MsWord: textbox text: " << m_fib.ccpTxbx << endl;
    kdDebug(s_area) << "MsWord::MsWord: header textbox text: " << m_fib.ccpHdrTxbx << endl;

    // Store away the streams for future use. Note that we do not
    // copy the contents of the streams, and that we rely on the storage
    // being present until we are destroyed.

    m_mainStream = mainStream.data;
    if (table0Stream.data && table1Stream.data)
    {
        // If and only if both table streams are present, the FIB tells us which
        // we should use.

        m_tableStream = m_fib.fWhichTblStm ? table1Stream.data : table0Stream.data;
    }
    else
    if (table0Stream.data)
    {
        m_tableStream = table0Stream.data;
    }
    else
    {
        m_tableStream = table1Stream.data;
    }
    m_dataStream = dataStream.data;
    m_dataStreamLength = dataStream.length;
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
        m_dataStreamLength = mainStream.length;
    }

    // We must call readStyles() first, as we use the STSHI size to detect
    // Word 2000.

    readStyles();
    readFonts();
    readListStyles();
}

MsWord::~MsWord()
{
}

void MsWord::parse()
{
    if (m_constructionError.length())
    {
       kdError(s_area) << m_constructionError << endl;
       return;
    }

    // Initialise the parse state.

    m_wasInTable = false;
    m_partialParagraph.text = "";
    m_partialParagraph.chpxs.resize(0);
    m_characterPosition = 0;

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
        // byte count preceding the first entry, and the number of entries.
        //
        // For the text plex, we store the start and size of the plex in the table

        typedef enum
        {
            clxtGrpprl = 1,
            clxtPlcfpcd = 2
        } clxtENUM;

        QMemArray<unsigned> grpprlCounts;
        QMemArray<const U8 *> grpprlPtrs;
        unsigned pieceCount = 0;
        const U8 *piecePtr = 0L;
        unsigned count = 0;
        const U8 *ptr;
        const U8 *end;
        U8 clxt = 0;

        // First skip the grpprls.

        ptr = m_tableStream + m_fib.fcClx;
        end = ptr + m_fib.lcbClx;
        while (ptr < end)
        {
            U16 cb;

            ptr += MsWordGenerated::read(ptr, &clxt);
            if (clxt != clxtGrpprl)
            {
                ptr--;
                break;
            }
            ptr += MsWordGenerated::read(ptr, &cb);

            unsigned index = grpprlCounts.size();

            grpprlCounts.resize(index + 1);
            grpprlPtrs.resize(index + 1);
            grpprlCounts[index] = cb;
            grpprlPtrs[index] = ptr;
            ptr += cb;
        }

        // Now locate the piece table.

        while (ptr < end)
        {
            U32 cb;

            ptr += MsWordGenerated::read(ptr, &clxt);
            if (clxt != clxtPlcfpcd)
            {
                ptr--;
                break;
            }
            count++;
            ptr += MsWordGenerated::read(ptr, &cb);
            pieceCount = cb;
            piecePtr = ptr;
            ptr += cb;
        }
        if ((clxt != clxtPlcfpcd) ||
            (count != 1))
        {
            constructionError(__LINE__, "cannot locate the piece table");
            return;
        };

        // Locate the piece table in a complex document.

        Plex<PCD, 8, MsWordGenerated::sizeof_PCD> *pieceTable = new Plex<PCD, 8, MsWordGenerated::sizeof_PCD>(this);

        U32 actualStartCp;
        U32 actualEndCp;
        PCD data;
        const U32 codepage1252mask = 0x40000000;
        bool unicode;

        kdDebug(s_area) << "text stream: FCs: " << m_fib.fcMin << ".." << m_fib.fcMac << endl;
        pieceTable->startIteration(piecePtr, pieceCount);
        while (pieceTable->getNext(&actualStartCp, &actualEndCp, &data))
        {
            unsigned prmCount;
            const U8 *prmPtr;
            U8 sprm[3];

            if (m_fib.nFib > s_maxWord6Version)
            {
                unicode = ((data.fc & codepage1252mask) != codepage1252mask);
                //unicode = unicode || m_fib.fExtChar;
                if (!unicode)
                {
                    data.fc &= ~codepage1252mask;
                    data.fc /= 2;
                }
            }
            else
            {
                unicode = false;
            }

            // Get the relevant property modifier(s).

            if (data.prm.fComplex)
            {
                unsigned igrpprl = (data.prm.val << 7) + data.prm.isprm;

                prmCount = grpprlCounts[igrpprl];
                prmPtr = grpprlPtrs[igrpprl];
            }
            else
            {
                U16 opcode = Properties::getRealOpcode(data.prm.isprm, m_fib);

                sprm[0] = opcode;
                sprm[1] = opcode >> 8;
                sprm[2] = data.prm.val;
                prmCount = 3;
                prmPtr = &sprm[0];
            }

            Properties properties = Properties(*this);

            properties.apply(prmPtr, prmCount);
            getParagraphsFromBtes(
                data.fc,
                data.fc + ((actualEndCp - actualStartCp) * (unicode ? 2 : 1)),
                unicode);
        }
    }
    else
    {
        getParagraphsFromBtes(
            //m_mainStream + m_fib.fcMin,
            m_fib.fcMin,
            m_fib.fcMac,
            false);
    }

    // Now deliver the document Metadata.
    //readAssociatedStrings();
}

//
// Get various strings which are associated with the document.
//
void MsWord::readAssociatedStrings()
{
    typedef enum
    {
        ibstAssocFileNext,      // unused.
        ibstAssocDot,           // filename of associated template.
        ibstAssocTitle,         // title of document.
        ibstAssocSubject,       // subject of document.
        ibstAssocKeyWords,      // keywords of document.
        ibstAssocComments,      // comments of document.
        ibstAssocAuthor,        // author of document.
        ibstAssocLastRevBy,     // name of person who last revised the
                                // document.
        ibstAssocDataDoc,       // filename of data document.
        ibstAssocHeaderDoc,     // filename of header document.
        ibstAssocCriteria1,     // packed strings used by print merge record
        ibstAssocCriteria2,     // selection.
        ibstAssocCriteria3,
        ibstAssocCriteria4,
        ibstAssocCriteria5,
        ibstAssocCriteria6,
        ibstAssocCriteria7
    } ibst;
    QString title;
    QString subject;
    QString author;
    QString lastRevisedBy;

    const U8 *ptr = m_tableStream + m_fib.fcSttbfAssoc; //lcbSttbfAssoc.

    // Failsafe for simple documents.
    if (!m_fib.lcbSttbfAssoc)
    {
        kdDebug(s_area) << "MsWord::getAssociatedStrings: no data " << endl;
        return;
    }

    STTBF data;
    ptr += read(ptr, &data);

    if (data.stringCount < ibstAssocCriteria1)
    {
        kdError(s_area) << "MsWord::getAssociatedStrings: insufficient data " << endl;
        return;
    }
    title = data.strings[ibstAssocTitle];
    subject = data.strings[ibstAssocSubject];
    author = data.strings[ibstAssocAuthor];
    lastRevisedBy = data.strings[ibstAssocLastRevBy];

    gotDocumentInformation(title, subject, author, lastRevisedBy);
}

// Create a cache of information about fonts. The information is indexed by font code.

void MsWord::readFonts()
{
    const U8 *ptr = m_tableStream + m_fib.fcSttbfffn; //lcbSttbfffn.
    const U8 *ptr2 = ptr + m_fib.lcbSttbfffn;

    // Failsafe for simple documents.

    m_fonts.count = 0;
    m_fonts.data = NULL;
    if (!m_fib.lcbSttbfffn)
    {
        kdDebug(s_area) << "MsWord::readFonts: no data " << endl;
        return;
    }

    if (m_fib.nFib > s_maxWord6Version)
    {
        // Find the number of fonts from the STTBF header.

        ptr += MsWordGenerated::read(ptr, &m_fonts.count);
        ptr += MsWordGenerated::sizeof_U16;
    }
    else
    {
        // Word6 does not record the number of fonts explicitly :-(.

        ptr += MsWordGenerated::sizeof_U16;
        while (ptr < ptr2)
        {
            FFN data;

	    ptr += read(ptr, &data);
            m_fonts.count++;
        }
        ptr = m_tableStream + m_fib.fcSttbfffn;
    }

    // Construct the array of fonts, and then walk the array reading in the font
    // definitions.

    m_fonts.data = new FFN [m_fonts.count];
    unsigned i = 0;
    while (ptr < ptr2)
    {
	ptr += read(ptr, &m_fonts.data[i]);
        i++;
    }
}

// Create a cache of information about lists.
//
//    m_listStyles: an array of arrays of pointers to LVLFs for each list style in the
//    LST array. The entries must be looked up using the list id and list level.

void MsWord::readListStyles()
{
    const U8 *ptr = m_tableStream + m_fib.fcPlcfLst; //lcbPlcfLst.
    const U8 *ptr2;
    U16 lstfCount;

    // Failsafe for simple documents.

    m_listStyles = NULL;
    if (!m_fib.lcbPlcfLst)
    {
        kdDebug(s_area) << "MsWord::readListStyles: no data " << endl;
        return;
    }

    // Find the number of LSTFs.

    ptr += MsWordGenerated::read(ptr, &lstfCount);
    ptr2 = ptr + lstfCount * MsWordGenerated::sizeof_LSTF;

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
            ptr2 += read(m_fib.lid, ptr2, &numberText, numberTextLength, true, m_fib.nFib);
        }
    }
}

// Create a cache of information about built-in styles.
//
// The cache consists of:
//
//    m_styles: an array of fully-decoded PAPs for each built in style
//    indexed by istd.

void MsWord::readStyles()
{
    const U8 *ptr = m_tableStream + m_fib.fcStshf;
    U16 cbStshi;
    STSHI stshi;

    // Failsafe for simple documents.

    m_styles.data = 0L;
    m_styles.names = 0L;
    if (!m_fib.lcbStshf)
    {
        return;
    }

    // Fetch the STSHI.

    ptr += MsWordGenerated::read(ptr, &cbStshi);
    if (cbStshi > MsWordGenerated::sizeof_STSHI)
    {
        // We simply discard parts of the STSHI we do not understand.

        if (cbStshi >= 20)
        {
            kdWarning(s_area) << "MsWord::readStyles: assuming Word 2000" <<
                endl;

            // Flip ourselves into unknown territory!

            m_fib.nFib = s_maxWord7Version + 1;
        }
        MsWordGenerated::read(ptr, &stshi);
        ptr += cbStshi;
    }
    else
    {
        // We know that older/smaller STSHIs can simply be zero extended into
        // out STSHI. So, we overwrite anything that is not valid with zeros.

        ptr += MsWordGenerated::read(ptr, &stshi);
        memset(((char *)&stshi) + cbStshi, 0, MsWordGenerated::sizeof_STSHI - cbStshi);
        ptr -= MsWordGenerated::sizeof_STSHI - cbStshi;
    }

    // Construct the array of styles, and then walk the array reading in the
    // style definitions.

    m_styles.count = stshi.cstd;
    m_styles.data = new Properties *[m_styles.count];
    m_styles.names = new QString[m_styles.count];
    for (unsigned i = 0; i < m_styles.count; i++)
    {
        U16 cbStd;
        STD std;

        ptr += MsWordGenerated::read(ptr, &cbStd);
        if (cbStd)
        {
            read(ptr, stshi.cbSTDBaseInFile, &std);

            // Fill the paragraph with its characteristics.

            m_styles.data[i] = new Properties(*this);
            m_styles.data[i]->apply(std);
            m_styles.names[i] = std.xstzName;
        }
        else
        {
            // Set the style to be the same as stiNormal. This is a purely
            // defensive thing...and relies on a viable 0th entry.

            m_styles.data[i] = m_styles.data[0];
            m_styles.names[i] = m_styles.names[0];
        }
        ptr += cbStd;
    }
}

template <class T, int word6Size, int word8Size>
MsWord::Plex<T, word6Size, word8Size>::Plex(MsWord *document) :
    m_document(document)
{
}

template <class T, int word6Size, int word8Size>
bool MsWord::Plex<T, word6Size, word8Size>::getNext(U32 *startFc, U32 *endFc, T *data)
{
    // Sanity check accesses beyond end of Plex.

    if (m_i >= m_crun)
    {
        return false;
    }
    m_fcNext += MsWordGenerated::read(m_fcNext, startFc);
    MsWordGenerated::read(m_fcNext, endFc);
    m_dataNext += m_document->read(m_dataNext, data);
    m_i++;
    return true;
}

template <class T, int word6Size, int word8Size>
void MsWord::Plex<T, word6Size, word8Size>::startIteration(const U8 *plex, const U32 byteCount)
{
    m_plex = plex;
    m_byteCount = byteCount;

    // Calculate the number of entries in the plex.

    if (m_byteCount > MsWordGenerated::sizeof_U32)
    {
        if (m_document->fib().nFib > s_maxWord6Version)
            m_crun = (m_byteCount - MsWordGenerated::sizeof_U32) / (word8Size + MsWordGenerated::sizeof_U32);
        else
            m_crun = (m_byteCount - MsWordGenerated::sizeof_U32) / (word6Size + MsWordGenerated::sizeof_U32);
    }
    else
    {
        m_crun = 0;
    }
    //kdDebug(s_area) << "MsWord::Plex::startIteration: " << m_crun << endl;
    m_fcNext = m_plex;
    m_dataNext = m_plex + ((m_crun + 1) * MsWordGenerated::sizeof_U32);
    m_i = 0;
}

// Read a string, converting to unicode if needed.
unsigned MsWord::read(U16 lid, const U8 *in, QString *out, unsigned count, bool unicode, U16 nFib)
{
    U16 char16;
    U8 char8;
    unsigned bytes = 0;

    *out = QString("");

    // Word6 always uses codepages rather than unicode.
    if (nFib <= s_maxWord6Version)
        unicode = false;
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
            *out += char2unicode(lid, char8);
        }
    }
//kdDebug(s_area) << "unicode: " << unicode << " " << *out << endl;
    return bytes;
}

// Read a Pascal string, converting to unicode if needed.
unsigned MsWord::read(U16 lid, const U8 *in, QString *out, bool unicode, U16 nFib)
{
    unsigned bytes = 0;

    *out = QString("");

    // Word6 always uses codepages rather than unicode.
    if (nFib <= s_maxWord6Version)
        unicode = false;
    if (unicode)
    {
        U16 length;
        U16 terminator;

        bytes += MsWordGenerated::read(in + bytes, &length);
        bytes += read(lid, in + bytes, out, length, true, nFib);
        bytes += MsWordGenerated::read(in + bytes, &terminator);
    }
    else
    {
        U8 length;
        U8 terminator;

        bytes += MsWordGenerated::read(in + bytes, &length);
        bytes += read(lid, in + bytes, out, length, false, nFib);
        bytes += MsWordGenerated::read(in + bytes, &terminator);
    }
    return bytes;
}

//
// Read a CHPX as stored in a FKP.
//
unsigned MsWord::read(const U8 *in, CHPXFKP *out)
{
    unsigned bytes = 0;

    bytes += MsWordGenerated::read(in + bytes, &out->count);
    out->ptr = (U8 *)(in + bytes);
    bytes += out->count;
    return bytes;
}

//
// Read a PAPX as stored in a FKP.
//
unsigned MsWord::read(const U8 *in, PAPXFKP *out)
{
    unsigned bytes = 0;
    U8 cw;

    bytes += MsWordGenerated::read(in + bytes, &cw);
    if (m_fib.nFib > s_maxWord6Version)
    {
        if (!cw)
        {
            bytes += MsWordGenerated::read(in + bytes, &cw);
            out->count = 2 * (cw - 1);
        }
        else
        {
            out->count = 2 * (cw - 1) - 1;
        }
        bytes += MsWordGenerated::read(in + bytes, &out->istd);
        out->ptr = (U8 *)(in + bytes);
        bytes += out->count;
    }
    else
    {
        out->count = 2 * (cw - 1);
        // The spec says that the Word6 istd is a byte, but that seems to be wrong.
        bytes += MsWordGenerated::read(in + bytes, &out->istd);
        out->ptr = (U8 *)(in + bytes);
        bytes += out->count;
    }
    return bytes;
}

unsigned MsWord::read(const U8 *in, unsigned baseInFile, STD *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += MsWordGenerated::read(in + bytes, &shifterU16);
    out->sti = shifterU16;
    shifterU16 >>= 12;
    out->fScratch = shifterU16;
    shifterU16 >>= 1;
    out->fInvalHeight = shifterU16;
    shifterU16 >>= 1;
    out->fHasUpe = shifterU16;
    shifterU16 >>= 1;
    out->fMassCopy = shifterU16;
    shifterU16 >>= 1;
    bytes += MsWordGenerated::read(in + bytes, &shifterU16);
    out->sgc = shifterU16;
    shifterU16 >>= 4;
    out->istdBase = shifterU16;
    shifterU16 >>= 12;
    bytes += MsWordGenerated::read(in + bytes, &shifterU16);
    out->cupx = shifterU16;
    shifterU16 >>= 4;
    out->istdNext = shifterU16;
    shifterU16 >>= 12;
    bytes += MsWordGenerated::read(in + bytes, &out->bchUpe);
    bytes += MsWordGenerated::read(in + bytes, &shifterU16);
    out->fAutoRedef = shifterU16;
    shifterU16 >>= 1;
    out->fHidden = shifterU16;
    shifterU16 >>= 1;
    out->unused8_3 = shifterU16;
    shifterU16 >>= 14;

    // The grupx reader code has to know about the alignment of the STD. We
    // choose to store this in a convenient field.

    out->fScratch = ((long)in & 1);

    // If the baseInFile is less than 10, then the style name is not stored in unicode!

    S8 offset = 10 - baseInFile;
    in -= offset;
    if (offset > 0)
    {
        bytes += read(m_fib.lid, in + bytes, &out->xstzName, false, m_fib.nFib);
    }
    else
    {
        bytes += read(m_fib.lid, in + bytes, &out->xstzName, true, m_fib.nFib);
    }
    out->grupx = in + bytes;

    // Set the length to the offset of the last stored byte.

    bytes = out->bchUpe;
    return bytes;
} // STD

unsigned MsWord::read(const U8 *in, FIB *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;

    // What version of Word are we dealing with?
    // Word 6 for the PC writes files with nFib = 101-102.
    // Word 6 for the Mac writes files with nFib = 103-104.
    // Word 8 (a.k.a. Winword 97) and later products write files with nFib > 105.

    // Bytes 0 to 31 are common.

    memset(out, 0, MsWordGenerated::sizeof_FIB);
    bytes += MsWordGenerated::read(in + bytes, &out->wIdent);
    bytes += MsWordGenerated::read(in + bytes, &out->nFib);
    bytes += MsWordGenerated::read(in + bytes, &out->nProduct);
    bytes += MsWordGenerated::read(in + bytes, &out->lid);
    bytes += MsWordGenerated::read(in + bytes, &out->pnNext);
    bytes += MsWordGenerated::read(in + bytes, &shifterU16);
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
    bytes += MsWordGenerated::read(in + bytes, &out->nFibBack);
    bytes += MsWordGenerated::read(in + bytes, &out->lKey);
    bytes += MsWordGenerated::read(in + bytes, &out->envr);
    bytes += MsWordGenerated::read(in + bytes, (U8 *)&shifterU8);
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
    bytes += MsWordGenerated::read(in + bytes, &out->chs);
    bytes += MsWordGenerated::read(in + bytes, &out->chsTables);
    bytes += MsWordGenerated::read(in + bytes, &out->fcMin);
    bytes += MsWordGenerated::read(in + bytes, &out->fcMac);
    if (out->nFib > s_maxWord6Version)
    {
        bytes = MsWordGenerated::read(in, out);
    }
    else
    if (out->nFib > s_minWordVersion)
    {
        // We will convert the FIB into the same form as for Winword

        out->csw = 14;
        out->lidFE = out->lid;
        out->clw = 22;
        bytes += MsWordGenerated::read(in + bytes, &out->cbMac);
        bytes += 16;

        // ccpText through ccpHdrTxbx.

        bytes += MsWordGenerated::read(in + bytes, &out->ccpText);
        bytes += MsWordGenerated::read(in + bytes, &out->ccpFtn);
        bytes += MsWordGenerated::read(in + bytes, &out->ccpHdd);
        bytes += MsWordGenerated::read(in + bytes, &out->ccpMcr);
        bytes += MsWordGenerated::read(in + bytes, &out->ccpAtn);
        bytes += MsWordGenerated::read(in + bytes, &out->ccpEdn);
        bytes += MsWordGenerated::read(in + bytes, &out->ccpTxbx);
        bytes += MsWordGenerated::read(in + bytes, &out->ccpHdrTxbx);

        // ccpSpare2.

        bytes += 4;
        out->cfclcb = 93;

        // fcStshfOrig through lcbSttbfAtnbkmk.

        bytes += MsWordGenerated::read(in + bytes, &out->fcStshfOrig);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbStshfOrig);
        bytes += MsWordGenerated::read(in + bytes, &out->fcStshf);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbStshf);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcffndRef);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcffndRef);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcffndTxt);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcffndTxt);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfandRef);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfandRef);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfandTxt);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfandTxt);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfsed);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfsed);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfpad);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfpad);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfphe);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfphe);
        bytes += MsWordGenerated::read(in + bytes, &out->fcSttbfglsy);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbSttbfglsy);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfglsy);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfglsy);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfhdd);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfhdd);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfbteChpx);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfbteChpx);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfbtePapx);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfbtePapx);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfsea);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfsea);
        bytes += MsWordGenerated::read(in + bytes, &out->fcSttbfffn);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbSttbfffn);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcffldMom);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcffldMom);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcffldHdr);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcffldHdr);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcffldFtn);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcffldFtn);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcffldAtn);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcffldAtn);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcffldMcr);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcffldMcr);
        bytes += MsWordGenerated::read(in + bytes, &out->fcSttbfbkmk);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbSttbfbkmk);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfbkf);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfbkf);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfbkl);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfbkl);
        bytes += MsWordGenerated::read(in + bytes, &out->fcCmds);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbCmds);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcmcr);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcmcr);
        bytes += MsWordGenerated::read(in + bytes, &out->fcSttbfmcr);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbSttbfmcr);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPrDrvr);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPrDrvr);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPrEnvPort);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPrEnvPort);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPrEnvLand);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPrEnvLand);
        bytes += MsWordGenerated::read(in + bytes, &out->fcWss);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbWss);
        bytes += MsWordGenerated::read(in + bytes, &out->fcDop);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbDop);
        bytes += MsWordGenerated::read(in + bytes, &out->fcSttbfAssoc);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbSttbfAssoc);
        bytes += MsWordGenerated::read(in + bytes, &out->fcClx);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbClx);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfpgdFtn);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfpgdFtn);
        bytes += MsWordGenerated::read(in + bytes, &out->fcAutosaveSource);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbAutosaveSource);
        bytes += MsWordGenerated::read(in + bytes, &out->fcGrpXstAtnOwners);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbGrpXstAtnOwners);
        bytes += MsWordGenerated::read(in + bytes, &out->fcSttbfAtnbkmk);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbSttbfAtnbkmk);

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

        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcdoaMom);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcdoaMom);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcdoaHdr);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcdoaHdr);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcspaMom);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcspaMom);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcspaHdr);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcspaHdr);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfAtnbkf);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfAtnbkf);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfAtnbkl);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfAtnbkl);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPms);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPms);
        bytes += MsWordGenerated::read(in + bytes, &out->fcFormFldSttbf);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbFormFldSttbf);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfendRef);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfendRef);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfendTxt);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfendTxt);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcffldEdn);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcffldEdn);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfpgdEdn);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfpgdEdn);
        bytes += MsWordGenerated::read(in + bytes, &out->fcDggInfo);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbDggInfo);
        bytes += MsWordGenerated::read(in + bytes, &out->fcSttbfRMark);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbSttbfRMark);
        bytes += MsWordGenerated::read(in + bytes, &out->fcSttbfCaption);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbSttbfCaption);
        bytes += MsWordGenerated::read(in + bytes, &out->fcSttbfAutoCaption);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbSttbfAutoCaption);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfwkb);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfwkb);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfspl);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfspl);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcftxbxTxt);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcftxbxTxt);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcffldTxbx);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcffldTxbx);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcfHdrtxbxTxt);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcfHdrtxbxTxt);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPlcffldHdrTxbx);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPlcffldHdrTxbx);
        bytes += MsWordGenerated::read(in + bytes, &out->fcStwUser);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbStwUser);
        bytes += MsWordGenerated::read(in + bytes, &out->fcSttbttmbd);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbSttbttmbd);
        bytes += MsWordGenerated::read(in + bytes, &out->fcUnused);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbUnused);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPgdMother);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPgdMother);
        bytes += MsWordGenerated::read(in + bytes, &out->fcBkdMother);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbBkdMother);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPgdFtn);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPgdFtn);
        bytes += MsWordGenerated::read(in + bytes, &out->fcBkdFtn);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbBkdFtn);
        bytes += MsWordGenerated::read(in + bytes, &out->fcPgdEdn);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbPgdEdn);
        bytes += MsWordGenerated::read(in + bytes, &out->fcBkdEdn);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbBkdEdn);
        bytes += MsWordGenerated::read(in + bytes, &out->fcSttbfIntlFld);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbSttbfIntlFld);
        bytes += MsWordGenerated::read(in + bytes, &out->fcRouteSlip);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbRouteSlip);
        bytes += MsWordGenerated::read(in + bytes, &out->fcSttbSavedBy);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbSttbSavedBy);
        bytes += MsWordGenerated::read(in + bytes, &out->fcSttbFnm);
        bytes += MsWordGenerated::read(in + bytes, &out->lcbSttbFnm);
    }
    else
    {
        // We don't support this.

        kdError(s_area) << "unsupported version of Word (nFib" << out->nFib << ")";
    }
    return bytes;
} // FIB

unsigned MsWord::read(const U8 *in, BTE *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    if (m_fib.nFib > s_maxWord6Version)
    {
        bytes = MsWordGenerated::read(in, out);
    }
    else
    {
        U16 tmp;

        bytes += MsWordGenerated::read(in + bytes, &tmp);
        out->pn = tmp;
    }
    return bytes;
} // BTE

unsigned MsWord::read(const U8 *in, FFN *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    unsigned int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += MsWordGenerated::read(in + bytes, &out->cbFfnM1);
    bytes += MsWordGenerated::read(in + bytes, &shifterU8);
    out->prq = shifterU8;
    shifterU8 >>= 2;
    out->fTrueType = shifterU8;
    shifterU8 >>= 1;
    out->unused1_3 = shifterU8;
    shifterU8 >>= 1;
    out->ff = shifterU8;
    shifterU8 >>= 3;
    out->unused1_7 = shifterU8;
    shifterU8 >>= 1;
    bytes += MsWordGenerated::read(in + bytes, &out->wWeight);
    bytes += MsWordGenerated::read(in + bytes, &out->chs);
    bytes += MsWordGenerated::read(in + bytes, &out->ixchSzAlt);
    unsigned count;
    if (m_fib.nFib > s_maxWord6Version)
    {
        for (i =0; i < sizeof(out->panose); i++)
            bytes += MsWordGenerated::read(in + bytes, &out->panose[i]);
        for (i =0; i < sizeof(out->fs); i++)
            bytes += MsWordGenerated::read(in + bytes, &out->fs[i]);
        count = (out->cbFfnM1 + 1 - bytes) / 2;
    }
    else
    {
        memset(&out->panose[0], 0, sizeof(out->panose));
        memset(&out->fs[0], 0, sizeof(out->fs));
        count = (out->cbFfnM1 + 1 - bytes);
    }
    bytes += read(m_fib.lid, in + bytes, &out->xszFfn, count - 1, true, m_fib.nFib);

    // Set the length to the offset of the last stored byte.

    bytes = out->cbFfnM1 + 1;
    return bytes;
} // FFN

unsigned MsWord::read(const U8 *in, FLD *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes = MsWordGenerated::read(in + bytes, out);
    return bytes;
} // FLD

unsigned MsWord::read(const U8 *in, FSPA *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes = MsWordGenerated::read(in + bytes, out);
    return bytes;
} // FSPA

unsigned MsWord::read(const U8 *in, PCD *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes = MsWordGenerated::read(in + bytes, out);
    return bytes;
} // PCD

unsigned MsWord::read(const U8 *in, PHE *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    if (m_fib.nFib > s_maxWord6Version)
    {
        bytes = MsWordGenerated::read(in, out);
    }
    else
    {
        U16 tmp;

        bytes += MsWordGenerated::read(in + bytes, &shifterU16);
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
        out->unused2 = 0;
        bytes += MsWordGenerated::read(in + bytes, &tmp);
        out->dxaCol = tmp;
        bytes += MsWordGenerated::read(in + bytes, &tmp);
        out->dym = tmp;
    }
    return bytes;
} // PHE

unsigned MsWord::read(const U8 *in, STTBF *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;
    bool unicode = false;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;

    // Get the string count. A value of 0xffff switchesus into unicode mode.

    bytes += MsWordGenerated::read(in + bytes, &out->stringCount);
    if (out->stringCount == 0xffff)
    {
        unicode = true;

        // Get the real string count.

        bytes += MsWordGenerated::read(in + bytes, &out->stringCount);
    }

    // Get the length of extra data.

    bytes += MsWordGenerated::read(in + bytes, &out->extraDataLength);

    // Now read each string and the associated data.

    out->strings = new QString[out->stringCount];
    out->extraData = new const U8 *[out->stringCount];
    for (unsigned i = 0; i < out->stringCount; i++)
    {
        bytes += read(m_fib.lid, in + bytes, &out->strings[i], unicode, m_fib.nFib);
        out->extraData[i] = in + bytes;
        bytes += out->extraDataLength;
    }
    return bytes;
}

MsWord::STTBF::STTBF()
{
    stringCount = 0;
    extraDataLength = 0;
    strings = (QString *)0;
    extraData = (const U8 **)0;
}

MsWord::STTBF::~STTBF()
{
    delete [] extraData;
    delete [] strings;
}
