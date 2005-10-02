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
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.

DESCRIPTION

    This file implements a simplified abstraction of Microsoft Word documents.
*/

//#define FONT_DEBUG

#include <kdebug.h>
#include <qfontdatabase.h>
#include <qfontinfo.h>
#include <document.h>
#include <properties.h>
#include <typeinfo>

const QString Document::s_anchor = "#";

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

// Return the name of a font. We have to convert the Microsoft font names to something that
// might just be present under X11.
QString Document::getFont(unsigned fc)
{
    QString font = MsWord::getFont(fc).xszFfn;

#ifdef FONT_DEBUG
    kdDebug() << "MS-FONT: " << font << endl;
#endif

    static const unsigned ENTRIES = 6;
    static QString fuzzyLookup[ENTRIES][2] =
    {
        // MS contains      X11 font family
        // substring.       non-AA name.
        { "times",          "times" },
        { "courier",        "courier" },
        { "andale",         "monotype" },
        { "monotype.com",   "monotype" },
        { "georgia",        "times" },
        { "helvetica",      "helvetica" }
    };

    // In an anti-aliased X environment, Qt will do a good job of looking up our local
    // equivalent of the MS font. But, we want to work even on non-AA. So, first, we do
    // a fuzzy match of some common MS font names.
    unsigned i;

    font = font.lower();
    for (i = 0; i < ENTRIES; i++)
    {
        // The loop will leave unchanged any MS font name not fuzzy-matched.
        if (font.find(fuzzyLookup[i][0], 0, FALSE) != -1)
        {
            font = fuzzyLookup[i][1];
            break;
        }
    }

#ifdef FONT_DEBUG
    kdDebug() << "FUZZY-FONT: " << font << endl;
#endif

    // Use Qt to look up our canonical equivalent of the font name.
    QFont xFont( font );
    QFontInfo info( xFont );

#ifdef FONT_DEBUG
    kdDebug() << "QT-FONT: " << info.family() << endl;
#endif

    return info.family();
}

void Document::gotParagraph(
    const QString &text,
    const PAP &pap,
    const CHPXarray &chpxs)
{
    Attributes attributes(this, pap);
    QString cleantext = text;

    attributes.setRuns(cleantext, chpxs);
    gotParagraph(cleantext, attributes);
    m_characterPosition += cleantext.length();
}

void Document::gotHeadingParagraph(
    const QString &text,
    const PAP &pap,
    const CHPXarray &chpxs)
{
    Attributes attributes(this, pap);
    QString cleantext = text;

    attributes.setRuns(cleantext, chpxs);
    gotHeadingParagraph(cleantext, attributes);
    m_characterPosition += cleantext.length();
}

void Document::gotListParagraph(
    const QString &text,
    const PAP &pap,
    const CHPXarray &chpxs)
{
    Attributes attributes(this, pap);
    QString cleantext = text;

    attributes.setRuns(cleantext, chpxs);
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
    QValueList<Attributes *> outStyles;
    int i;

    for (i = 0; i < row.itcMac; i++)
    {
        Attributes *attributes = new Attributes(this, styles[i]);
        QString cleantext = texts[i];

        attributes->setRuns(cleantext, chpxs[i]);
        outStyles.append(attributes);
        outTexts[i] = cleantext;
        m_characterPosition += cleantext.length();
    }
    gotTableRow(m_tableNumber, outTexts, outStyles, row);
    delete [] outTexts;
}

void Document::parse()
{
    m_tableNumber = 0;
    m_characterPosition = 0;
    m_imageNumber = 0;
    MsWord::parse();
}

Document::Attributes::Attributes(
    Document *doc,
    const PAP &baseStyle) :
    m_baseStyle( *doc )
{
    m_doc = doc;
    m_baseStyle.apply(baseStyle);
    m_field.fieldType = FIELD_TYPE_UNKNOWN_KEYWORD;
    m_field.start = 0;
    m_field.separator = 0;
    m_field.end = 0;
}

Document::Attributes::~Attributes()
{
    m_runs.clear();
}

const Properties &Document::Attributes::baseStyle() const
{
    return m_baseStyle;
}

void Document::Attributes::rewriteField(
    QString &text,
    CHPXarray &chpxs)
{
    kdDebug(s_area) << "Document::rewriteField: field: " <<
        m_field.fieldType << ": " <<
        chpxs[m_field.start].startFc << "." <<
        chpxs[m_field.separator].startFc << "." <<
        chpxs[m_field.end].startFc << ": " <<
        text.mid(chpxs[m_field.start].startFc, chpxs[m_field.end].startFc - chpxs[m_field.start].startFc + 1) << endl;
    if (m_field.separator < m_field.start)
    {
        kdDebug(s_area) << "Document::rewriteField: missing separator" << endl;
        m_field.separator = m_field.end;
    }
    //kdDebug(s_area) << "Document::rewriteField: before: " << text << endl;
    unsigned lhsLength = chpxs[m_field.separator].startFc - chpxs[m_field.start].startFc + 1;
    unsigned rhsLength = chpxs[m_field.end].startFc - chpxs[m_field.separator].startFc;
    unsigned run;
    unsigned int length;
    int adjusted;
    int adjustment;
    QString newLhs;
    QString newRhs;

    switch (m_field.fieldType)
    {
    case FIELD_TYPE_EMBEDDED_OBJECT:
        newLhs = "";
        newRhs = s_anchor;
        break;
    case 0: // TBD: some (second?, mailto:?) HYPERLINKs look like this!
    case FIELD_TYPE_HYPERLINK:
        newLhs = "";
        newRhs = text.mid(chpxs[m_field.separator].startFc + 1, rhsLength - 1);
        break;
    default:
        kdError(s_area) << "Document::rewriteField: unsupported field: " <<
            m_field.fieldType << endl;
        return;
    }

    // Adjust LHS and separator as required.

    {
        run = m_field.start;
        text.replace(chpxs[run].startFc, lhsLength, newLhs);
        length = lhsLength - newLhs.length();
        adjustment = QMIN(length, chpxs[run].endFc - chpxs[run].startFc);
        chpxs[run].endFc -= adjustment;
        adjusted = adjustment;
        run++;
        while (run < chpxs.size())
        {
            adjustment = QMIN(length - adjusted, chpxs[run].endFc - chpxs[run].startFc);
            chpxs[run].startFc -= adjusted;
            adjusted += adjustment;
            chpxs[run].endFc -= adjusted;
            run++;
        }
    }

    // Adjust RHS as required.

    {
        run = m_field.separator;
        text.replace(chpxs[run].startFc, rhsLength, newRhs);
        length = rhsLength - newRhs.length();
        adjustment = QMIN(length - adjusted, chpxs[run].endFc - chpxs[run].startFc);
        chpxs[run].endFc -= adjustment;
        adjusted = adjustment;
        run++;
        while (run < chpxs.size())
        {
            adjustment = QMIN(length - adjusted, chpxs[run].endFc - chpxs[run].startFc);
            chpxs[run].startFc -= adjusted;
            adjusted += adjustment;
            chpxs[run].endFc -= adjusted;
            run++;
        }
    }
    //kdDebug(s_area) << "Document::rewriteField: after: " << text << endl;
}

const QValueList<KSharedPtr<Document::Run> > &Document::Attributes::runs() const
{
    return m_runs;
}

void Document::Attributes::setRuns(
    QString &text,
    const CHPXarray &originalChpxs)
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
    QValueList<KSharedPtr<Run> > tmpRuns;
    unsigned i;

    runs = chpxs.size();
    for (i = 0; i < runs; i++)
    {
        Properties exceptionStyle(m_baseStyle);
        const CHP *chp;
        Run *run;

        // Initialise the entry with the base style, then apply the deltas.

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
                kdDebug(s_area) << "Document::Attributes::setRuns: OLE2 object" << endl;
                Object *object = new Object;

                // All we know about the object is its storage id...

                object->start = chpxs[i].startFc;
                object->end = chpxs[i].endFc;
                object->id = chp->fcPic_fcObj_lTagObj;
                run = object;
            }
            else
            {
                kdDebug(s_area) << "Document::Attributes::setRuns: embedded object" << endl;

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
            text[chpxs[i].startFc] = s_anchor[0];
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
			found = m_doc->getPicture(
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

                        found = m_doc->getOfficeArt(
                                    m_doc->m_characterPosition + chpxs[i].startFc,
                                    shape,
                                    &drawingLength,
                                    &drawingData,
                                    &delayData);
                        drawingId = shape.spid;
                        drawingType = "msod";
                    }
                    text[chpxs[i].startFc] = s_anchor[0];
                    if (found)
                    {
                        kdDebug(s_area) << "Document::Attributes::setRuns: drawing type: " <<
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
                            m_doc->m_imageNumber++;
                            image->id = drawingId;
                            image->type = drawingType;
                            image->length = drawingLength;
                            image->data = (const char *)drawingData;
                            run = image;
                        }
                    }
                    else
                    {
                        kdError(s_area) << "Document::Attributes::setRuns: cannot find drawing:" <<
                            (m_doc->m_characterPosition + chpxs[i].startFc) << endl;
                    }
                }
                break;
            case SPECIAL_FIELD_BEGIN_MARK:
                m_doc->getField(
                    m_doc->m_characterPosition + chpxs[i].startFc,
                    &fieldType);
                m_field.fieldType = static_cast<fieldTypes>(fieldType);
                m_field.start = i;
                kdDebug(s_area) << "Document::Attributes::setRuns: set field type: " <<
                    m_field.fieldType << endl;
                break;
            case SPECIAL_FIELD_SEPARATOR:
                m_field.separator = i;
                break;
            case SPECIAL_FIELD_END_MARK:
                m_field.end = i;
                rewriteField(text, chpxs);
                break;
            default:
                kdError(s_area) << "Document::Attributes::setRuns: unsupported object type:" <<
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
        tmpRuns.append( KSharedPtr<Run>( run ) );
    }

    // Deal with the effects of rewriteField() in case it modified a run (or
    // even set it to zero length!) after it was created.

    for (i = 0; i < runs; i++)
    {
        if (chpxs[i].startFc != chpxs[i].endFc)
        {
            tmpRuns[i]->start = chpxs[i].startFc;
            tmpRuns[i]->end = chpxs[i].endFc;
            m_runs.append(tmpRuns[i]);
        }
    }

    // Paragraphs end with a control character that we want to suppress.

    unsigned length = text.length() - 1;
    QChar last = text[length];

    runs = m_runs.count();
    if ((last == QChar('\r')) ||
        (last == QChar('\a')))
    {
        text.truncate(length);
        m_runs[runs - 1]->end--;

        // Trim the final run if required.

        if (m_runs[runs - 1]->start == m_runs[runs - 1]->end)
        {
            m_runs.remove(m_runs.last());
        }
    }
}
