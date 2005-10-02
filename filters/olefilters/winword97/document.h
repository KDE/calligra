#ifndef DOCUMENT_H
#define DOCUMENT_H

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

#include <kdebug.h>
#include <ksharedptr.h>
#include <msword.h>
#include <properties.h>
#include <qvaluelist.h>

class myFile;

class Document: protected MsWord
{
protected:

    // Construction. Invoke with the OLE streams that comprise the Word document.

    Document(
        const myFile &mainStream,
        const myFile &table0Stream,
        const myFile &table1Stream,
        const myFile &dataStream);
    virtual ~Document();

    // Call the parse() function to process the document. The callbacks return
    // the text along with any relevant attributes.

public:
    void parse();

    // We describe the data we return to a client in terms of a "run" of
    // character data along with some properties. The properties may be
    // formatting information or, in the case of embedded objects, data
    // associated with the object. Thus, we have a base class called Run
    // and a set of derivations for each specialisation...
    //
    // Where a run of text is associated with an object, the character
    // concerned is set to a known s_anchor (whose value is chosen so
    // as not to upset KSpell).
    //
    // We derive from KSharedPtr since some overrides of our callback
    // functions may choose to store the data for later processing.

    static const QString s_anchor;

    class Run: public KShared
    {
    public:
        unsigned start;
        unsigned end;

        // We need at least one virtual function to enable RTTI!

        virtual ~Run() {};
    };

    // Specialisation for text formatting.

    class Format: public Run
    {
    public:
        Properties *values;
    };

    // Specialisations for embedded images and drawings.

    class Image: public Run
    {
    public:
        unsigned id;
        QString type;
        unsigned length;
        const char *data;
    };

    class VectorGraphic: public Image
    {
    public:
        const char *delay;
    };

    // Specialisation for embedded objects etc.

    class Object: public Run
    {
    public:
        unsigned id;
    };

    // Attributes describe the detailed styling
    // of an extent of text.

    class Attributes
    {
    public:
        Attributes(
            Document *doc,
            const PAP &baseStyle);
        ~Attributes();

        // The starting style for the whole extent.
        const Properties &baseStyle() const;

        // Get the styles of the individual runs within the extent.
        const QValueList<KSharedPtr<Run> > &runs() const;

        // Set the styles of the individual runs within the extent.
        // The text passed in may be modified to eliminate Microsoftisms.
        void setRuns(
            QString &text,
            const CHPXarray &originalChpxs);

    private:
        // Field code handling.

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

        Document *m_doc;
        Properties m_baseStyle;
        QValueList<KSharedPtr<Run> > m_runs;
        struct
        {
            fieldTypes fieldType;
            unsigned start;
            unsigned separator;
            unsigned end;
        } m_field;
        void rewriteField(
            QString &text,
            CHPXarray &chpxs);
    };
    friend class Attributes;

protected:
    virtual void gotError(
        const QString &text) = 0;
    virtual void gotParagraph(
        const QString &text,
        Attributes &style) = 0;
    virtual void gotHeadingParagraph(
        const QString &text,
        Attributes &style) = 0;
    virtual void gotListParagraph(
        const QString &text,
        Attributes &style) = 0;
    virtual void gotTableBegin(
        unsigned tableNumber) = 0;
    virtual void gotTableEnd(
        unsigned tableNumber) = 0;
    virtual void gotTableRow(
        unsigned tableNumber,
        const QString texts[],
        const QValueList<Attributes *> styles,
        MsWordGenerated::TAP &row) = 0;

    // Return the name of a font.

    QString getFont(unsigned fc);

private:
    Document(const Document &);
    const Document &operator=(const Document &);

    // Parse context.

    unsigned m_tableNumber;
    unsigned m_characterPosition;
    unsigned m_imageNumber;

    // Override the base class functions.
    void gotParagraph(
        const QString &text,
        const PAP &pap,
        const CHPXarray &chpxs);
    void gotHeadingParagraph(
        const QString &text,
        const PAP &pap,
        const CHPXarray &chpxs);
    void gotListParagraph(
        const QString &text,
        const PAP &pap,
        const CHPXarray &chpxs);
    void gotTableBegin();
    void gotTableEnd();
    void gotTableRow(
        const QString texts[],
        const PAP styles[],
        const CHPXarray chpxs[],
        TAP &row);
};
#endif
