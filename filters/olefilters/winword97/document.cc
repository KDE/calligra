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
    QString &text,
    const PAP &baseStyle,
    const CHPXarray &originalChpxs,
    Attributes &attributes)
{
    typedef enum
    {
        SPECIAL_CURRENT_PAGE_NUMBER,
        SPECIAL_PICTURE,
        SPECIAL_AUTONUMBERED_FOOTNOTE_REFERENCE,
        SPECIAL_FOOTNOTE_SEPARATOR_CHARACTER,
        SPECIAL_FOOTNOTE_CONTINUATION_CHARACTER,
        SPECIAL_ANNOTATION_REFERENCE,
        SPECIAL_LINE_NUMBER,
        SPECIAL_HAND_ANNOTATION_PICTURE,
        SPECIAL_DRAWN_OBJECT,
        SPECIAL_ABBREVIATED_DATE = 10,
        SPECIAL_TIME,
        SPECIAL_CURRENT_SECTION_NUMBER,
        SPECIAL_PARAGRAPH_END,
        SPECIAL_ABBREVIATED_DAY_OF_WEEK,
        SPECIAL_DAY_OF_WEEK,
        SPECIAL_DAY_SHORT,
        SPECIAL_FIELD_BEGIN_MARK = 19,
        SPECIAL_FIELD_SEPARATOR,
        SPECIAL_FIELD_END_MARK,
        SPECIAL_HOUR_OF_CURRENT_TIME_WITH_NO_LEADING_ZERO,
        SPECIAL_HOUR_OF_CURRENT_TIME,
        SPECIAL_MINUTE_OF_CURRENT_TIME_WITH_NO_LEADING_ZERO,
        SPECIAL_MINUTE_OF_CURRENT_TIME,
        SPECIAL_SECONDS_OF_CURRENT_TIME,
        SPECIAL_AM_PM_FOR_CURRENT_TIME,
        SPECIAL_CURRENT_TIME_IN_HOURS_MINUTES_SECONDS,
        SPECIAL_DATE_M,
        SPECIAL_SHORT_DATE,
        SPECIAL_SHORT_MONTH = 33,
        SPECIAL_LONG_YEAR,
        SPECIAL_SHORT_YEAR,
        SPECIAL_ABBREVIATED_MONTH,
        SPECIAL_LONG_MONTH,
        SPECIAL_CURRENT_TIME_IN_HOURS_MINUTES,
        SPECIAL_LONG_DATE,
        SPECIAL_PRINT_MERGE_HELPER_FIELD = 41
    } specialChars;
    CHPXarray chpxs = originalChpxs;
    unsigned runs;

    runs = chpxs.size();
    attributes.baseStyle = baseStyle;
    attributes.runs.setAutoDelete(true);
    for (unsigned i = 0; i < runs; i++)
    {
        Properties exceptionStyle = Properties(*this);
        const CHP *chp;
        Run *run;

        // Initialise the entry with the base style, then apply the deltas.

        exceptionStyle.apply(baseStyle);
        exceptionStyle.apply(chpxs[i].data.ptr, chpxs[i].data.count);
        chp = exceptionStyle.getChp();

        // Check the type of data. If it is a field, then we collect the
        // information needed to rewrite the text stream appropriately.

        run = 0L;
        if (!chp->fSpec)
        {
        }
        else
        if (chp->fObj)
        {
            QString mimeType;

            // OLE2 and embedded objects.

            m_field.separator = i; //chpxs[i].startFc;
            if (chp->fOle2)
            {
                kdDebug(s_area) << "Document::createAttributes: OLE2 object" << endl;
                Object *object = new Object;

                // All we know about the object is its storage id...

                object->start = chpxs[i].startFc;
                object->end = chpxs[i].endFc;
                object->id = chp->fcPic_fcObj_lTagObj;
                run = object;
            }
            else
            {
                kdDebug(s_area) << "Document::createAttributes: embedded object" << endl;

                // TBD: verify object support!

                //MsWord::getObject(
                    //chp->fcPic_fcObj_lTagObj,
                    //mimeType);

                Object *object = new Object;

                object->start = chpxs[i].startFc;
                object->end = chpxs[i].endFc;
                object->id = chp->fcPic_fcObj_lTagObj;
                run = object;
            }
        }
        else
        {
            U32 pictureId;
            QString pictureType;
            U32 pictureLength;
            const U8 *pictureData;
            U8 fieldType;

            // This is either a picture or an Office art object or...

            switch (text[chpxs[i].startFc].unicode())
            {
            case SPECIAL_PICTURE:
            case SPECIAL_DRAWN_OBJECT:

                // If this picture follows an embedded object, we treat it as
                // equivalent to the end of a field.

                if (m_field.fieldType == FIELD_TYPE_EMBEDDED_OBJECT)
                {
                    m_field.end = i;
                    kdDebug(s_area) << "Document::createAttributes: field: " <<
                        m_field.fieldType << ": " <<
                        chpxs[m_field.start].startFc << "." <<
                        chpxs[m_field.separator].startFc << "." <<
                        chpxs[m_field.end].startFc << ": " << endl;
                    rewriteField(text, chpxs);
                }
                else
                {
                    pictureId = 0;
                    pictureLength = 0;
                    if (text[chpxs[i].startFc].unicode() == SPECIAL_PICTURE)
                    {
                        // A picture.

                        pictureId = chp->fcPic_fcObj_lTagObj;
                        MsWord::getPicture(
                            pictureId,
                            pictureType,
                            &pictureLength,
                            &pictureData);
                    }
                    else
                    {
                        // A drawn object, a.k.a. "Office Art".

                        MsWord::getOfficeArt(
                            m_characterPosition + chpxs[i].startFc,
                            &pictureId,
                            pictureType,
                            &pictureLength,
                            &pictureData);
                    }
                    if (pictureLength)
                    {
                        Image *image = new Image;

                        kdDebug(s_area) << "Document::createAttributes: picture type: " << pictureType << endl;
                        image->start = chpxs[i].startFc;
                        image->end = chpxs[i].endFc;
                        m_imageNumber++;
                        image->id = (unsigned)pictureId;
                        image->type = pictureType;
                        image->length = (unsigned)pictureLength;
                        image->data = (const char *)pictureData;
    // TBD: Kword pictures are broken!
                        run = image;
                    }
                    else
                    {
                        kdError(s_area) << "Document::createAttributes: cannot find picture:" <<
                            pictureId << endl;
                    }
                }
                break;
            case SPECIAL_FIELD_BEGIN_MARK:
                MsWord::getField(
                    m_characterPosition + chpxs[i].startFc,
                    &fieldType);
                m_field.fieldType = static_cast<fieldTypes>(fieldType);
                m_field.start = i;
                kdDebug(s_area) << "Document::createAttributes: set field type: " <<
                    m_field.fieldType << endl;
                break;
            case SPECIAL_FIELD_SEPARATOR:
                m_field.separator = i;
                break;
            case SPECIAL_FIELD_END_MARK:
                m_field.end = i;
                kdDebug(s_area) << "Document::createAttributes: field: " <<
                    m_field.fieldType << ": " <<
                    chpxs[m_field.start].startFc << "." <<
                    chpxs[m_field.separator].startFc << "." <<
                    chpxs[m_field.end].startFc << ": " << endl;
                rewriteField(text, chpxs);
                break;
            default:
                kdError(s_area) << "Document::createAttributes: unsupported object type:" <<
                    text[chpxs[i].startFc].unicode() << endl;
                break;
            }
        }

        if (!run)
        {
            // This is normal text, or we had an error with a special
            // character.

            Format *format = new Format;

            format->start = chpxs[i].startFc;
            format->end = chpxs[i].endFc;
            format->values = new Properties(exceptionStyle);
            run = format;
        }
        attributes.runs.append(run);
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
    QString cleantext = text;

    createAttributes(cleantext, pap, chpxs, attributes);
    gotParagraph(cleantext, attributes);
    m_characterPosition += cleantext.length();
}

void Document::gotHeadingParagraph(
    const QString &text,
    const PAP &pap,
    const CHPXarray &chpxs)
{
    Attributes attributes;
    QString cleantext = text;

    createAttributes(cleantext, pap, chpxs, attributes);
    gotHeadingParagraph(cleantext, attributes);
    m_characterPosition += cleantext.length();
}

void Document::gotListParagraph(
    const QString &text,
    const PAP &pap,
    const CHPXarray &chpxs)
{
    Attributes attributes;
    QString cleantext = text;

    createAttributes(cleantext, pap, chpxs, attributes);
    gotListParagraph(cleantext, attributes);
    m_characterPosition += cleantext.length();
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

void Document::rewriteField(
    QString &text,
    CHPXarray &chpxs)
{
    unsigned lhsLength = chpxs[m_field.separator].startFc - chpxs[m_field.start].startFc;
    unsigned rhsLength = chpxs[m_field.end].startFc - chpxs[m_field.separator].startFc;
    unsigned run;
    int length;
    QString newLhs;
    QString newRhs;

    switch (m_field.fieldType)
    {
    case FIELD_TYPE_EMBEDDED_OBJECT:
        break;
    case 0: // TBD: some (second?, mailto:?) HYPERLINKs look like this!
    case FIELD_TYPE_HYPERLINK:
        newRhs = text.mid(chpxs[m_field.separator].startFc, rhsLength);
        break;
    default:
        kdError(s_area) << "Document::rewriteField: unsupported field: " <<
            m_field.fieldType << endl;
        return;
    }

    // Adjust LHS if required.

    {
        run = m_field.start;
        text.replace(chpxs[run].startFc, lhsLength, newLhs);
        length = lhsLength - newLhs.length();
        chpxs[run].endFc -= length;
        run++;
        while (run < chpxs.size())
        {
            chpxs[run].startFc -= length;
            chpxs[run].endFc -= length;
            run++;
        }
    }

    // Adjust RHS if required.

    {
        run = m_field.separator;
        text.replace(chpxs[run].startFc, rhsLength, newRhs);
        length = rhsLength - newRhs.length();
        chpxs[run].endFc -= length;
        run++;
        while (run < chpxs.size())
        {
            chpxs[run].startFc -= length;
            chpxs[run].endFc -= length;
            run++;
        }
    }
}

