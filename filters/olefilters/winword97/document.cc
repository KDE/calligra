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
    typedef enum
    {
        FIELD_TYPE_UNKNOWN_KEYWORD = 1,
        FIELD_TYPE_POSSIBLE_BOOKMARK,
        FIELD_TYPE_BOOKMARK_REFERENCE,
        FIELD_TYPE_INDEX_ENTRY,
        FIELD_TYPE_FOOTNOTE_REFERENCE,
        FIELD_TYPE_SET_COMMAND_FOR_PRINT_MERGE,
        FIELD_TYPE_IF_COMMAND_FOR_PRINT_MERGE,
        FIELD_TYPE_CREATE_INDEX,
        FIELD_TYPE_TABLE_OF_CONTENTS_ENTRY,
        FIELD_TYPE_STYLE_REFERENCE,
        FIELD_TYPE_DOCUMENT_REFERENCE,
        FIELD_TYPE_SEQUENCE_MARK,
        FIELD_TYPE_CREATE_TABLE_OF_CONTENTS,
        FIELD_TYPE_QUOTE_INFO_VARIABLE,
        FIELD_TYPE_QUOTE_TITLE_VARIABLE,
        FIELD_TYPE_QUOTE_SUBJECT_VARIABLE,
        FIELD_TYPE_QUOTE_AUTHOR_VARIABLE,
        FIELD_TYPE_QUOTE_KEYWORDS_VARIABLE,
        FIELD_TYPE_QUOTE_COMMENTS_VARIABLE,
        FIELD_TYPE_QUOTE_LAST_REVISED_BY_VARIABLE,
        FIELD_TYPE_QUOTE_CREATION_DATE_VARIABLE,
        FIELD_TYPE_QUOTE_REVISION_DATE_VARIABLE,
        FIELD_TYPE_QUOTE_PRINT_DATE_VARIABLE,
        FIELD_TYPE_QUOTE_REVISION_NUMBER_VARIABLE,
        FIELD_TYPE_QUOTE_EDIT_TIME_VARIABLE,
        FIELD_TYPE_QUOTE_NUMBER_OF_PAGES_VARIABLE,
        FIELD_TYPE_QUOTE_NUMBER_OF_WORDS_VARIABLE,
        FIELD_TYPE_QUOTE_NUMBER_OF_CHARACTERS_VARIABLE,
        FIELD_TYPE_QUOTE_FILE_NAME_VARIABLE,
        FIELD_TYPE_QUOTE_DOCUMENT_TEMPLATE_NAME_VARIABLE,
        FIELD_TYPE_QUOTE_CURRENT_DATE_VARIABLE,
        FIELD_TYPE_QUOTE_CURRENT_TIME_VARIABLE,
        FIELD_TYPE_QUOTE_CURRENT_PAGE_VARIABLE,
        FIELD_TYPE_EVALUATE_EXPRESSION,
        FIELD_TYPE_INSERT_LITERAL_TEXT,
        FIELD_TYPE_INCLUDE_COMMAND_PRINT_MERGE,
        FIELD_TYPE_PAGE_REFERENCE,
        FIELD_TYPE_ASK_COMMAND_PRINT_MERGE,
        FIELD_TYPE_FILL_IN_COMMAND_TO_DISPLAY_PROMPT_PRINT_MERGE,
        FIELD_TYPE_DATA_COMMAND_PRINT_MERGE,
        FIELD_TYPE_NEXT_COMMAND_PRINT_MERGE,
        FIELD_TYPE_NEXTIF_COMMAND_PRINT_MERGE,
        FIELD_TYPE_SKIPIF_PRINT_MERGE,
        FIELD_TYPE_INSERTS_NUMBER_OF_CURRENT_PRINT_MERGE_RECORD,
        FIELD_TYPE_DDE_REFERENCE,
        FIELD_TYPE_DDE_AUTOMATIC_REFERENCE,
        FIELD_TYPE_INSERTS_GLOSSARY_ENTRY,
        FIELD_TYPE_SENDS_CHARACTERS_TO_PRINTER_WITHOUT_TRANSLATION,
        FIELD_TYPE_FORMULA_DEFINITION,
        FIELD_TYPE_GOTO_BUTTON,
        FIELD_TYPE_MACRO_BUTTON,
        FIELD_TYPE_INSERT_AUTO_NUMBERING_FIELD_IN_OUTLINE_FORMAT,
        FIELD_TYPE_INSERT_AUTO_NUMBERING_FIELD_IN_LEGAL_FORMAT,
        FIELD_TYPE_INSERT_AUTO_NUMBERING_FIELD_IN_ARABIC_NUMBER_FORMAT,
        FIELD_TYPE_READS_A_TIFF_FILE,
        FIELD_TYPE_LINK,
        FIELD_TYPE_SYMBOL,
        FIELD_TYPE_EMBEDDED_OBJECT,
        FIELD_TYPE_MERGE_FIELDS,
        FIELD_TYPE_USER_NAME,
        FIELD_TYPE_USER_INITIAL,
        FIELD_TYPE_USER_ADDRESS,
        FIELD_TYPE_BAR_CODE,
        FIELD_TYPE_DOCUMENT_VARIABLE,
        FIELD_TYPE_SECTION,
        FIELD_TYPE_SECTION_PAGES,
        FIELD_TYPE_INCLUDE_PICTURE,
        FIELD_TYPE_INCLUDE_TEXT,
        FIELD_TYPE_FILE_SIZE,
        FIELD_TYPE_FORM_TEXT_BOX,
        FIELD_TYPE_FORM_CHECK_BOX,
        FIELD_TYPE_NOTE_REFERENCE,
        FIELD_TYPE_CREATE_TABLE_OF_AUTHORITIES,
        FIELD_TYPE_MARK_TABLE_OF_AUTHORITIES_ENTRY,
        FIELD_TYPE_MERGE_RECORD_SEQUENCE_NUMBER,
        FIELD_TYPE_MACRO,
        FIELD_TYPE_PRIVATE,
        FIELD_TYPE_INSERT_DATABASE,
        FIELD_TYPE_AUTOTEXT,
        FIELD_TYPE_COMPARE_TWO_VALUES,
        FIELD_TYPE_PLUG_IN_MODULE_PRIVATE,
        FIELD_TYPE_SUBSCRIBER,
        FIELD_TYPE_FORM_LIST_BOX,
        FIELD_TYPE_ADVANCE,
        FIELD_TYPE_DOCUMENT_PROPERTY,
        FIELD_TYPE_UNUSED_86,
        FIELD_TYPE_OCX,
        FIELD_TYPE_HYPERLINK,
        FIELD_TYPE_AUTOTEXTLIST,
        FIELD_TYPE_LIST_ELEMENT,
        FIELD_TYPE_HTML_CONTROL
    } fieldTypes;

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

        // Check the type of data.

        run = 0L;
        if (!chp->fSpec)
        {
        }
        else
        if (chp->fObj)
        {
            QString mimeType;

            // OLE2 and embedded objects.

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
/*
                // TBD: verify object support!

                MsWord::getObject(
                    chp->fcPic_fcObj_lTagObj,
                    mimeType);
*/

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

            switch (text[chpxs[i].startFc])
            {
            case SPECIAL_PICTURE:
            case SPECIAL_DRAWN_OBJECT:
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

                    image->start = chpxs[i].startFc;
                    image->end = chpxs[i].endFc;
                    m_imageNumber++;
                    image->id = (unsigned)pictureId;
                    image->type = pictureType;
                    image->length = (unsigned)pictureLength;
                    image->data = (const char *)pictureData;
// TBD: Kword pictures are broken!
//                    run = image;
                }
                else
                {
                    kdError(s_area) << "Document::createAttributes: cannot find picture:" <<
                        pictureId << endl;
                }
                break;
            case SPECIAL_FIELD_BEGIN_MARK:
                kdDebug(s_area) << "Document::createAttributes: field begin" << endl;
                MsWord::getField(
                    m_characterPosition + chpxs[i].startFc,
                    &fieldType);
                switch (fieldType)
                {
                case FIELD_TYPE_EMBEDDED_OBJECT:
                    kdDebug(s_area) << "Document::createAttributes: field: embedded object" << endl;
                    break;
                default:
                    kdError(s_area) << "Document::createAttributes: unsupported field type:" <<
                        fieldType << endl;
                    break;
                }
                break;
            case SPECIAL_FIELD_SEPARATOR:
                kdError(s_area) << "Document::createAttributes: field separator" << endl;
                break;
            case SPECIAL_FIELD_END_MARK:
                kdError(s_area) << "Document::createAttributes: field end" << endl;
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
