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

    This file is a description of the structures used in the on-disk format
    of Microsoft Word documents.
*/

#include <kdebug.h>
#include <msword.h>
#include <mswordgenerated.h>
static const short CP2UNI[] = { 0x20ac, 0x0000, 0x201a, 0x0192,
                                0x201e, 0x2026, 0x2020, 0x2021,
                                0x02c6, 0x2030, 0x0160, 0x2039,
                                0x0152, 0x0000, 0x017d, 0x0000,
                                0x0000, 0x2018, 0x2019, 0x201c,
                                0x201d, 0x2022, 0x2013, 0x2014,
                                0x02dc, 0x2122, 0x0161, 0x203a,
                                0x0153, 0x0000, 0x017e, 0x0178 };
const short char2unicode(const unsigned char &c) {
    if(c<=0x7f || c>=0xa0)
        return static_cast<short>(c);
    else
        return CP2UNI[c-0x80];
}

MsWord::MsWord(
        const U8 *mainStream,
        const U8 *table0Stream,
        const U8 *table1Stream,
        const U8 *dataStream)
{
    MsWordGenerated::read(mainStream, &m_fib);
    if(m_fib.fEncrypted)
    {
        kDebugError(31000, "MsWord::MsWord(): Can't locate the piece table");
	return;
    };

    m_pcd = new Plex<PCD>(this, m_tableStream + m_plcfpcds.byteCountOffset);
}

MsWord::~MsWord()
{
}

void MsWord::getPAPX()
{
    // A bin table is a plex of BTEs.

    Plex<PAPXBTE> btes = Plex<PAPXBTE>(this, m_tableStream + m_plcfpcds.byteCountOffset);

    btes.iterate(0, m_fib.fcMac - m_fib.fcMin);
}

void MsWord::getText()
{
    // Note that we test for the presence of complex structure, rather than
    // m_fib.fComplex. This allows us to treat newer files which always seem
    // to have piece tables in a consistent manner.
    //
    // There is also the implication that without the complex structures, the
    // text cannot be in unicode form.

    if (m_fib.lcbClx)
    {
	m_pcd->iterate(0, m_fib.fcMac - m_fib.fcMin);
    }
    else
    {
        QString text;

        read(m_mainStream + m_fib.fcMin, &text, m_fib.fcMac - m_fib.fcMin, false);
        gotText(text);
    }
}

template <class T, class secondaryT>
void MsWord::FKP<T, secondaryT>::iterate(U32 requestedStart, U32 requestedEnd)
{
    const U8 *countPtr;
    const U8 *dataPtr;
    U8 crun;
    U32 start;
    U32 end;
    U8 rgb;
    secondaryT secondaryData;
    T mainData;

    // Get the number of entries in the FKP.

    countPtr = reinterpret_cast<U8 *>(m_pn * 512);
    MsWordGenerated::read(countPtr + 511, &crun); 
    dataPtr = countPtr + ((crun + 1) * sizeof(start));

    // Traverse the FKP until we run out of data, or the caller tells
    // us not to continue.

    for (unsigned i = 0; i < crun; i++)
    {
	countPtr += MsWordGenerated::read(countPtr, &start);
	MsWordGenerated::read(countPtr, &end);
	dataPtr += MsWordGenerated::read(dataPtr, &rgb);
        if (!rgb)
        {
            kDebugError(31000, "MsWord::iterate: %u:%u: no PAPX/CHPX", start, end);
            continue;
        }
	dataPtr += MsWordGenerated::read(dataPtr, &secondaryData);
	dataPtr += MsWord::read(dataPtr, &mainData);

        // Ignore data that preceeds the first location of interest or
        // follows the last location of interest.

        if (end < requestedStart)
            continue;
        if (start > requestedEnd)
            return;

        // Trim the entry as required if we are near the very start or very
        // end of the requested range.

        if (start < requestedStart)
        {
            unsigned diff = requestedStart - start;
            start += diff;
        }
        if (end > requestedEnd)
        {
            unsigned diff = end - requestedEnd;
            end -= diff;
        }

	if (!m_client->fkpCallback(start, end, mainData))
            break;
    }
}

template <class T>
void MsWord::Plex<T>::iterate(U32 requestedStart, U32 requestedEnd)
{
    const U8 *countPtr = m_plexPtr;
    const U8 *dataPtr;
    U32 byteCount;
    unsigned crun;
    U32 start;
    U32 end;
    T data;

    // Get the length of the plex and calculate the number of entries.

    countPtr += MsWordGenerated::read(countPtr, &byteCount);
    crun = (byteCount - sizeof(start)) / (sizeof(T) + sizeof(start));
    dataPtr = countPtr + ((crun + 1) * sizeof(start));

    // Traverse the plex until we run out of data, or the caller tells
    // us not to continue.

    for (unsigned i = 0; i < crun; i++)
    {
	countPtr += MsWordGenerated::read(countPtr, &start);
	MsWordGenerated::read(countPtr, &end);
	dataPtr += MsWordGenerated::read(dataPtr, &data);

        // Ignore data that preceeds the first location of interest or
        // follows the last location of interest.

        if (end < requestedStart)
            continue;
        if (start > requestedEnd)
            return;

        // Trim the entry as required if we are near the very start or very
        // end of the requested range.

        if (start < requestedStart)
        {
            unsigned diff = requestedStart - start;
            start += diff;
        }
        if (end > requestedEnd)
        {
            unsigned diff = end - requestedEnd;
            end -= diff;
        }

	if (!m_client->plexCallback(start, end, data))
            break;
    }
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
    kDebugError(31000, QString("MsWord::read: ") + *out);
    return bytes;
}

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
    kDebugError(31000, "MsWord::read: PAPXFKP grpprl bytes %u", out->grpprl);
    return bytes;
}

bool MsWord::fkpCallback(unsigned start, unsigned end, const PAPXFKP& data)
{
/*
    const U32 codepage1252mask = 0x40000000;
    U32 fc = data.fc;
    bool unicode;
    QString text;

    unicode = ((fc & codepage1252mask) != codepage1252mask);
    if (!unicode)
    {
	fc &= ~ codepage1252mask;
	fc /= 2;
    }
    read(m_mainStream + fc, &text, end - start, unicode);
    gotText(text);
*/
    return true;
};

bool MsWord::plexCallback(unsigned start, unsigned end, const PCD& data)
{
    const U32 codepage1252mask = 0x40000000;
    U32 fc = data.fc;
    bool unicode;
    QString text;

    unicode = ((fc & codepage1252mask) != codepage1252mask);
    if (!unicode)
    {
	fc &= ~ codepage1252mask;
	fc /= 2;
    }
    read(m_mainStream + fc, &text, end - start, unicode);
    gotText(text);
    return true;
};

bool MsWord::plexCallback(unsigned start, unsigned end, const PAPXBTE& data)
{
    FKP<PAPXFKP, PHE> papxFKP = FKP<PAPXFKP, PHE>(this, data.pn);

    papxFKP.iterate(start, end);
    return true;
};


