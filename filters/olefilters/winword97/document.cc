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

    This file implements a simplified abstraction of Microsoft Word documents.
*/

#include <kdebug.h>
#include <document.h>
#include <properties.h>

void Document::createAttributes(
    const PAP &baseStyle,
    const CHPXarray &chpxs,
    Attributes &attributes)
{
    unsigned runs;

    runs = chpxs.size();
    attributes.baseStyle = baseStyle;
    attributes.runs.resize(runs);

    // Initialise each array entry with the base style, then apply the deltas.

    for (unsigned i = 0; i < runs; i++)
    {
        attributes.runs[i].start = chpxs[i].startFc;
        attributes.runs[i].end = chpxs[i].endFc;
        attributes.runs[i].values = new Properties(*this);
        attributes.runs[i].values->apply(baseStyle);
        attributes.runs[i].values->apply(chpxs[i].data.ptr, chpxs[i].data.count);
    }
}

Document::Document(
    const U8 *mainStream,
    const U8 *table0Stream,
    const U8 *table1Stream,
    const U8 *dataStream) :
        MsWord(
            mainStream,
            table0Stream,
            table1Stream,
            dataStream)
{
}

Document::~Document()
{
}

void Document::gotParagraph(
    const QString &text,
    const PAP &pap,
    const CHPXarray &chpxs)
{
    Attributes attributes;

    createAttributes(pap, chpxs, attributes);
    gotParagraph(text, attributes);
}

void Document::gotHeadingParagraph(
    const QString &text,
    const PAP &pap,
    const CHPXarray &chpxs)
{
    Attributes attributes;

    createAttributes(pap, chpxs, attributes);
    gotHeadingParagraph(text, attributes);
}

void Document::gotListParagraph(
    const QString &text,
    const PAP &pap,
    const CHPXarray &chpxs)
{
    Attributes attributes;

    createAttributes(pap, chpxs, attributes);
    gotListParagraph(text, attributes);
}

void Document::gotTableBegin()
{
    m_tableNumber++;
    m_tableRowNumber = 0;
    gotTableBegin(m_tableNumber);
}

void Document::gotTableEnd()
{
    gotTableEnd(m_tableNumber);
}

void Document::gotTableRow(const QString texts[], const PAP styles[], TAP &row)
{
    gotTableRow(m_tableNumber, m_tableRowNumber, texts, styles, row);
    m_tableRowNumber++;
}

void Document::parse()
{
    m_tableNumber = 0;
    MsWord::parse();
}
