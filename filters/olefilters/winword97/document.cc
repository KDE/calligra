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
#include <typeinfo>

void Document::createAttributes(
    const QString &text,
    const PAP &baseStyle,
    const CHPXarray &chpxs,
    Attributes &attributes)
{
    unsigned runs;

    runs = chpxs.size();
    attributes.baseStyle = baseStyle;
    attributes.runs.setAutoDelete(true);
    for (unsigned i = 0; i < runs; i++)
    {
        Properties exceptionStyle = Properties(*this);
        const CHP *chp;

        // Initialise the entry with the base style, then apply the deltas.

        exceptionStyle.apply(baseStyle);
        exceptionStyle.apply(chpxs[i].data.ptr, chpxs[i].data.count);
        chp = exceptionStyle.getChp();
        if (!chp->fSpec)
        {
            // This is normal text.

            Format *format = new Format;

            format->start = chpxs[i].startFc;
            format->end = chpxs[i].endFc;
            format->values = new Properties(exceptionStyle);
            attributes.runs.append(format);
        }
        else
        if (chp->fObj)
        {
            // OLE2 and embedded objects.

            if (chp->fOle2)
            {
                kdDebug(s_area) << "Document::createAttributes: OLE2 object" << endl;
            }
            else
            {
                kdDebug(s_area) << "Document::createAttributes: embedded object" << endl;
            }

            // TBD: replace with object support!

            Format *format = new Format;

            format->start = chpxs[i].startFc;
            format->end = chpxs[i].endFc;
            format->values = new Properties(exceptionStyle);
            attributes.runs.append(format);
        }
        else
        {
            U32 cp = 0;
            U32 pictureId;
            QString pictureType;
            U32 pictureLength = 0;
            const U8 *pictureData;

            // This is either a picture or an Office art object or...

            switch (text[chpxs[i].startFc])
            {
            case 8:
                cp = m_characterPosition + chpxs[i].startFc;
                MsWord::getOfficeArt(
                    cp,
                    &pictureId,
                    pictureType,
                    &pictureLength,
                    &pictureData);
                break;
            default:
                kdError(s_area) << "Document::createAttributes: unsupported object type:" <<
                    text[chpxs[i].startFc].unicode() << endl;
                break;
            }

            if (pictureLength)
            {
                Image *image = new Image;

                image->start = chpxs[i].startFc;
                image->end = chpxs[i].endFc;
                m_imageNumber++;
                image->id = (unsigned)pictureId;
                image->type = pictureType;
                image->length = (unsigned)pictureLength;
                image->data = (const char *)pictureData;
                attributes.runs.append(image);
            }
            else
            {
                kdError(s_area) << "Document::createAttributes: cannot find picture:" <<
                    cp << endl;

                Format *format = new Format;

                format->start = chpxs[i].startFc;
                format->end = chpxs[i].endFc;
                format->values = new Properties(exceptionStyle);
                attributes.runs.append(format);
            }
        }
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

    createAttributes(text, pap, chpxs, attributes);
    gotParagraph(text, attributes);
    m_characterPosition += text.length();
}

void Document::gotHeadingParagraph(
    const QString &text,
    const PAP &pap,
    const CHPXarray &chpxs)
{
    Attributes attributes;

    createAttributes(text, pap, chpxs, attributes);
    gotHeadingParagraph(text, attributes);
    m_characterPosition += text.length();
}

void Document::gotListParagraph(
    const QString &text,
    const PAP &pap,
    const CHPXarray &chpxs)
{
    Attributes attributes;

    createAttributes(text, pap, chpxs, attributes);
    gotListParagraph(text, attributes);
    m_characterPosition += text.length();
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
    m_characterPosition++;
}

void Document::gotTableRow(const QString texts[], const PAP styles[], TAP &row)
{
    gotTableRow(m_tableNumber, m_tableRowNumber, texts, styles, row);
    for (unsigned i = 0; i < row.itcMac; i++)
    {
        m_characterPosition += texts[i].length();
    }
    m_tableRowNumber++;
}

void Document::parse()
{
    m_tableNumber = 0;
    m_characterPosition = 0;
    m_imageNumber = 0;
    MsWord::parse();
}
