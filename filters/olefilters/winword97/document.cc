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
#include <qfontdatabase.h>
#include <qfontinfo.h>
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

kdDebug(s_area) << "Document::createAttributes: hps 1: " <<exceptionStyle.getChp()->hps<< endl;
        exceptionStyle.apply(baseStyle);
kdDebug(s_area) << "Document::createAttributes: hps 2: " <<exceptionStyle.getChp()->hps<< endl;
        exceptionStyle.apply(chpxs[i].data.ptr, chpxs[i].data.count);
kdDebug(s_area) << "Document::createAttributes: hps 3: " <<exceptionStyle.getChp()->hps<< endl;
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
                    U32 drawingId;
                    QString drawingType;
                    U32 drawingLength;
                    const U8 *drawingData;
                    const U8 *delayData;
                    bool found;

                    if (text[chpxs[i].startFc].unicode() == SPECIAL_PICTURE)
                    {
                        // A drawing.

                        drawingId = chp->fcPic_fcObj_lTagObj;
                        found = MsWord::getPicture(
                                    drawingId,
                                    drawingType,
                                    &drawingLength,
                                    &drawingData);
                        delayData = 0L;
                    }
                    else
                    {
                        // A drawn object, a.k.a. "Office Art".

    // the the length will be set to zero. The returned drawingId is
    // guaranteed to be the same for drawings which appear multiple
    // times in the document.

                        FSPA shape;

                        found = MsWord::getOfficeArt(
                                    m_characterPosition + chpxs[i].startFc,
                                    shape,
                                    &drawingLength,
                                    &drawingData,
                                    &delayData);
                        drawingId = shape.spid;
                        drawingType = "msod";
                    }
                    if (found)
                    {
                        kdDebug(s_area) << "Document::createAttributes: drawing type: " <<
                            drawingType << endl;
                        if ((drawingType == "wmf") ||
                            (drawingType == "msod"))
                        {
                            VectorGraphic *vectorGraphic = new VectorGraphic;

                            vectorGraphic->start = chpxs[i].startFc;
                            vectorGraphic->end = chpxs[i].endFc;
                            vectorGraphic->id = drawingId;
                            vectorGraphic->type = drawingType;
                            vectorGraphic->length = drawingLength;
                            vectorGraphic->data = (const char *)drawingData;
                            vectorGraphic->delay = (const char *)delayData;
                            run = vectorGraphic;
                        }
                        else
                        {
                            Image *image = new Image;

                            image->start = chpxs[i].startFc;
                            image->end = chpxs[i].endFc;
                            m_imageNumber++;
                            image->id = drawingId;
                            image->type = drawingType;
                            image->length = drawingLength;
                            image->data = (const char *)drawingData;
                            run = image;
                        }
                    }
                    else
                    {
                        kdError(s_area) << "Document::createAttributes: cannot find drawing:" <<
                            (m_characterPosition + chpxs[i].startFc) << endl;
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
    const myFile &mainStream,
    const myFile &table0Stream,
    const myFile &table1Stream,
    const myFile &dataStream) :
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

// Paragrpahs end with a control character that we want to suppress.
QString Document::cleanText(
    const QString &text)
{
    QString cleantext = text;
    unsigned length = cleantext.length() - 1;
    QChar last = cleantext[length];

    if ((last == QChar('\r')) ||
        (last == QChar('\a')))
        cleantext.truncate(length);
    return cleantext;
}

// Return the name of a font. We have to convert the Microsoft font names to something that
// might just be present under X11.

QString Document::getFont(unsigned fc)
{
    QString msFont = MsWord::getFont(fc).xstzName;

    // Use Qt to look up our local equivalent of the MS font.
    QFont xFont(msFont);
    QFontInfo info(xFont);
    kdDebug() << "FONT: Original: " << msFont << endl;
    kdDebug() << "FONT: Requested: " << xFont.family() << endl;
    kdDebug() << "FONT: Got: " << info.family() << endl;
    return info.family();
}

void Document::gotParagraph(
    const QString &text,
    const PAP &pap,
    const CHPXarray &chpxs)
{
    Attributes attributes;
    QString cleantext = cleanText(text);

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
    QString cleantext = cleanText(text);

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
    QString cleantext = cleanText(text);

    createAttributes(cleantext, pap, chpxs, attributes);
    gotListParagraph(cleantext, attributes);
    m_characterPosition += cleantext.length();
}

void Document::gotTableBegin()
{
    m_tableNumber++;
    gotTableBegin(m_tableNumber);
}

void Document::gotTableEnd()
{
    gotTableEnd(m_tableNumber);
    m_characterPosition++;
}

void Document::gotTableRow(
    const QString texts[],
    const PAP styles[],
    const CHPXarray chpxs[],
    TAP &row)
{
    QString *outTexts = new QString[row.itcMac];
    Attributes *outStyles = new Attributes[row.itcMac];
    unsigned i;

    for (i = 0; i < row.itcMac; i++)
    {
        Attributes attributes;
        QString cleantext = cleanText(texts[i]);
        createAttributes(cleantext, styles[i], chpxs[i], attributes);
        outStyles[i] = attributes;
        outTexts[i] = cleantext;
        m_characterPosition += cleantext.length();
    }
    gotTableRow(m_tableNumber, outTexts, outStyles, row);
    delete [] outTexts;
    delete [] outStyles;
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
