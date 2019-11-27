/* This file is part of the Calligra project
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2002 David Faure <faure@kde.org>
   Copyright (C) 2008 Benjamin Cail <cricketc@gmail.com>
   Copyright (C) 2009 Inge Wallin   <inge@lysator.liu.se>
   Copyright (C) 2010, 2011 Matus Uzak <matus.uzak@ixonos.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the Library GNU General Public
   version 2 of the License, or (at your option) version 3 or,
   at the discretion of KDE e.V (which shall act as a proxy as in
   section 14 of the GPLv3), any later version..

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef TEXTHANDLER_H
#define TEXTHANDLER_H

#include "tablehandler.h"
//#include "versionmagic.h"
#include "paragraph.h"
#include "exceptions.h"

#include <wv2/src/handlers.h>
#include <wv2/src/functordata.h>
#include <wv2/src/lists.h>
#include <QString>
#include <QChar>
#include <QObject>
#include <QDomElement>
#include <QBuffer>
#include <QStack>

#include <KoXmlWriter.h>
#include <KoGenStyles.h>

#include <string>
#include <vector>
#include <stack>


class Document;

namespace wvWare
{
class Style;
class Parser;
class FunctorBase;
namespace Word97 {
struct PAP;
}
}

class WordsReplacementHandler : public wvWare::InlineReplacementHandler
{
public:
    wvWare::U8 hardLineBreak() override;
    wvWare::U8 nonBreakingHyphen() override;
    wvWare::U8 nonRequiredHyphen() override;
};


class WordsTextHandler : public QObject, public wvWare::TextHandler
{
    Q_OBJECT
public:
    WordsTextHandler(wvWare::SharedPtr<wvWare::Parser> parser, KoXmlWriter* bodyWriter, KoGenStyles* mainStyles);
    ~WordsTextHandler() override;

    //////// TextHandler interface

    void sectionStart(wvWare::SharedPtr<const wvWare::Word97::SEP> sep) override;
    void sectionEnd() override;
    void pageBreak() override;
    void headersFound(const wvWare::HeaderFunctor& parseHeaders) override;
    void footnoteFound(wvWare::FootnoteData data, wvWare::UString characters,
                               wvWare::SharedPtr<const wvWare::Word97::SEP> sep,
                               wvWare::SharedPtr<const wvWare::Word97::CHP> chp,
                               const wvWare::FootnoteFunctor& parseFootnote) override;
    void annotationFound(wvWare::UString characters,
                                 wvWare::SharedPtr<const wvWare::Word97::CHP> chp,
                                 const wvWare::AnnotationFunctor& parseAnnotation) override;

    void paragraphStart(wvWare::SharedPtr<const wvWare::ParagraphProperties> paragraphProperties, wvWare::SharedPtr<const wvWare::Word97::CHP> chp) override;
    void paragraphEnd() override;
    void fieldStart(const wvWare::FLD* fld, wvWare::SharedPtr<const wvWare::Word97::CHP> chp) override;
    void fieldSeparator(const wvWare::FLD* fld, wvWare::SharedPtr<const wvWare::Word97::CHP> chp) override;
    void fieldEnd(const wvWare::FLD* fld, wvWare::SharedPtr<const wvWare::Word97::CHP> chp) override;
    void runOfText(const wvWare::UString& text, wvWare::SharedPtr<const wvWare::Word97::CHP> chp) override;

    void tableRowFound(const wvWare::TableRowFunctor& functor, wvWare::SharedPtr<const wvWare::Word97::TAP> tap) override;
    void tableEndFound() override;
    void bookmarkStart( const wvWare::BookmarkData& data ) override;
    void bookmarkEnd( const wvWare::BookmarkData& data ) override;

    void msodrawObjectFound(const unsigned int globalCP, const wvWare::PictureData* data) override;

    ///////// Our own interface

    /**
     * Check the current texthandler state.  At the moment only the number of
     * opened and closed fields if checked.
     * @return 0 - Not Ok, 1 - Ok
     */
    bool stateOk() const;

    /**
     * Paragraph can be present in {header, footer, footnote, endnote,
     * annotation, body}.  @return the actual writer.
     */
    KoXmlWriter* currentWriter() const;

    /**
     * @ftc index into the font table provided by Character properties (CHP)
     * @return font name
     */
    QString getFont(unsigned ftc) const;

    /**
     * A special purpose function which provides the first font color not set
     * to cvAuto from the styles hierarchy of the lately processed paragraph.
     *
     * @return font color in the format "#RRGGBB" or an empty string.
     */
    QString paragraphBaseFontColorBkp() const { return m_paragraphBaseFontColorBkp; }

    /**
     * Provides access to the background color of the lately processed
     * paragraph to other handlers.  Notice that text-properties of any of the
     * text styles might modify the background color.
     *
     * @return background color in the format "#RRGGBB" or an empty string.
     */
    QString paragraphBgColor() const { return m_paragraph ? m_paragraph->currentBgColor() : QString(); }

    /**
     * TODO:
     */
    bool writeListInfo(KoXmlWriter* writer, const wvWare::Word97::PAP& pap, const wvWare::ListInfo* listInfo);

    /**
     * TODO:
     */
    void defineListStyle(KoGenStyle &style);

    /**
     *
     */
    bool listIsOpen(); //tell us whether a list is open

    /**
     *
     */
    void closeList();

    // Provide access to private attributes for our handlers
    Document* document() const { return m_document; }
    void setDocument(Document * document) { m_document = document; }
    void set_breakBeforePage(bool val) { m_breakBeforePage = val; }
    bool breakBeforePage(void) const { return m_breakBeforePage; }
    int sectionNumber(void) const { return m_sectionNumber; }

    // Communication with Document, without having to know about Document
Q_SIGNALS:
    void sectionFound(wvWare::SharedPtr<const wvWare::Word97::SEP>);
    void sectionEnd(wvWare::SharedPtr<const wvWare::Word97::SEP>);
    void subDocFound(const wvWare::FunctorBase* parsingFunctor, int data);
    void footnoteFound(const wvWare::FunctorBase* parsingFunctor, int data);
    void annotationFound(const wvWare::FunctorBase* parsingFunctor, int data);
    void headersFound(const wvWare::FunctorBase* parsingFunctor, int data);
    void tableFound(Words::Table* table);
    void inlineObjectFound(const wvWare::PictureData& data, KoXmlWriter* writer);
    void floatingObjectFound(unsigned int globalCP, KoXmlWriter* writer);
/*     void updateListDepth(int); */

private:

    /**
     * A special purpose function which provides the font color of the current
     * paragraph.  Looking for the first built-in paragraph style in the styles
     * hierarchy providing a font color not set to cvAuto.
     *
     * @return font color in the format "#RRGGBB" or an empty string
     */
    QString paragraphBaseFontColor() const;

    KoGenStyles* m_mainStyles; //this is for collecting most of the styles
    KoXmlWriter* m_bodyWriter; //this writes to content.xml inside <office:body>
    Document* m_document; // The document owning this text handler

    wvWare::SharedPtr<const wvWare::Word97::SEP> m_sep; //store section info for section end
    wvWare::SharedPtr<wvWare::Parser> m_parser;

    int m_sectionNumber;  // number of sections processed
    int m_tocNumber;      // number of TOC fields processed
    int m_footNoteNumber; // number of footnote _vars_ written out
    int m_endNoteNumber;  // number of endnote _vars_ written out

    //int m_textStyleNumber; //number of styles created for text family
    //int m_paragraphStyleNumber; //number of styles created for paragraph family
    //int m_listStyleNumber; //number of styles created for lists

#if 1
    bool       m_hasStoredDropCap; // True if the previous paragraph was a dropcap
    int        m_dcs_fdct;
    int        m_dcs_lines;
    qreal      m_dropCapDistance;
#endif
    QString    m_dropCapString;
    QString    m_dropCapStyleName;

    //if TRUE, the fo:break-before="page" property is required because a manual
    //page break (an end-of-section character not at the end of a section) was
    //found in the main document
    bool m_breakBeforePage;

    bool m_insideFootnote;
    KoXmlWriter* m_footnoteWriter; //write the footnote data, then add it to bodyWriter
    QBuffer* m_footnoteBuffer; //buffer for the footnote data

    bool m_insideAnnotation;
    KoXmlWriter* m_annotationWriter; //write the annotation data, then add it to bodyWriter
    QBuffer* m_annotationBuffer; //buffer for the annotation data

    bool m_insideDrawing;
    KoXmlWriter* m_drawingWriter; //write the drawing data, then add it to bodyWriter

    // ************************************************
    //  Paragraph
    // ************************************************
    wvWare::SharedPtr<const wvWare::ParagraphProperties> m_currentPPs; //paragraph properties
    Paragraph *m_paragraph; //pointer to paragraph object

    // The 1st font color not set to cvAuto from the built-in styles hierarchy
    // of the lately processed paragraph.
    QString m_paragraphBaseFontColorBkp;

    // ************************************************
    //  Table
    // ************************************************
    Words::Table* m_currentTable;
    KoXmlWriter* m_tableWriter;
    QBuffer* m_tableBuffer;
    QString m_floatingTable;

    // ************************************************
    //  List
    // ************************************************
    QString m_listSuffixes[9]; // the suffix for every list level seen so far
    int m_currentListLevel; // tells us which list level we're on (-1 if not in a list)
    int m_currentListID;    // tracks the ID of the current list - 0 if not a list

    QStack <KoXmlWriter*> m_usedListWriters;

    // Map of listID keys and listLevel/continue-list pairs
    QMap<int, QPair<quint8, bool> > m_continueListNum;

    // Map of listId.level keys and xml:id values of text:list elements to
    // continue automatic numbering.
    QMap<QString, QString> m_numIdXmlIdMap;

    // ************************************************
    //  State
    // ************************************************

    //save/restore (very similar to the wv2 method)
    struct State {
        State(Words::Table* table, Paragraph* paragraph,
              int listDepth, int listID,
              KoXmlWriter* drawingWriter, bool insideDrawing) :

            table(table),
            paragraph(paragraph),
            listDepth(listDepth),
            listID(listID),
            drawingWriter(drawingWriter),
            insideDrawing(insideDrawing)
        {}
        Words::Table* table;
        Paragraph* paragraph;
        int listDepth; //tells us which list level we're on (-1 if not in a list)
        int listID;    //tracks the id of the current list - 0 if no list
        KoXmlWriter* drawingWriter;
        bool insideDrawing;
    };

    std::stack<State> m_oldStates;
    void saveState();
    void restoreState();

    // ************************************************
    //  Field related
    // ************************************************

    //field type enumeration as defined in [MS-DOC.pdf] page 356
    enum fldType
    {
        UNSUPPORTED = 0,
        //PARSE_ERROR = 0x01, ///< Specifies that the field was unable to be parsed.
        REF_WITHOUT_KEYWORD = 0x02, ///< Specifies that the field represents a REF field where the keyword has been omitted.
        REF = 0x03, ///< Reference
        //FTNREF = 0x05, ///< Identical to NOTEREF (not a reference)
        //SET = 0x06,
        //IF = 0x07,
        //INDEX = 0x08,
        //STYLEREF = 0x0a,
        SEQ = 0x0c,
        TOC = 0x0d,
        //INFO = 0x0e,
        TITLE = 0x0f,
        SUBJECT = 0x10,
        AUTHOR = 0x11,
        //KEYWORDS = 0x12,
        //COMMENTS = 0x13,
        LAST_REVISED_BY = 0x14, ///< LASTSAVEDBY
        CREATEDATE = 0x15,
        SAVEDATE = 0x16,
        //PRINTDATE = 0x17,
        //REVNUM = 0x18,
        EDITTIME = 0x19,
        NUMPAGES = 0x1a,
        //NUMWORDS = 0x1b,
        //NUMCHARS = 0x1c,
        FILENAME = 0x1d,
        //TEMPLATE = 0x1e,
        DATE = 0x1f,
        TIME = 0x20,
        PAGE = 0x21,
        //EQUALS = 0x22, ///< the "=" sign
        //QUOTE = 0x23,
        //INCLUDE = 0x24,
        PAGEREF = 0x25,
        //ASK = 0x26,
        //FILLIN = 0x27,
        //DATA = 0x28,
        //NEXT = 0x29,
        //NEXTIF = 0x2a,
        //SKIPIF = 0x2b,
        //MERGEREC = 0x2c,
        //DDE = 0x2d,
        //DDEAUTO = 0x2e,
        //GLOSSARY = 0x2f,
        //PRINT = 0x30,
        EQ = 0x31,
        //GOTOBUTTON = 0x32,
        MACROBUTTON = 0x33,
        //AUTONUMOUT = 0x34,
        //AUTONUMLGL = 0x35,
        //AUTONUM = 0x36,
        //IMPORT = 0x37,
        //LINK = 0x38,
        SYMBOL = 0x39,
        //EMBED = 0x3a,
        MERGEFIELD = 0x3b,
        //USERNAME = 0x3c,
        //USERINITIALS = 0x3d,
        //USERADDRESS = 0x3e,
        //BARCODE = 0x3f,
        //DOCVARIABLE = 0x40,
        //SECTION = 0x41,
        //SECTIONPAGES = 0x42,
        //INCLUDEPICTURE = 0x43,
        //INCLUDETEXT = 0x44,
        //FILESIZE = 0x45,
        //FORMTEXT = 0x46,
        //FORMCHECKBOX = 0x47,
        //NOTEREF = 0x48,
        //TOA = 0x49,
        //MERGESEQ = 0x4b,
        //AUTOTEXT = 0x4f,
        //COMPARE = 0x50,
        //ADDIN = 0x51,
        //FORMDROPDOWN = 0x53,
        //ADVANCE = 0x54,
        //DOCPROPERTY = 0x55,
        //CONTROL = 0x57,
        HYPERLINK = 0x58,
        AUTOTEXTLIST = 0x59,
        //LISTNUM = 0x5a,
        //HTMLCONTROL = 0x5b,
        //BIDIOUTLINE = 0x5c,
        //ADDRESSBLOCK = 0x5d,
        //GREETINGLINE = 0x5e,
        SHAPE = 0x5f
    };

    //save/restore for processing field (very similar to the wv2 method)
    struct fld_State
    {
        fld_State(fldType type = UNSUPPORTED) :
            m_type(type),
            m_insideField(false),
            m_afterSeparator(false),
            m_hyperLinkActive(false),
            m_tabLeader(QChar::Null),
            m_writer(0),
            m_buffer(0)
        {
            m_buffer = new QBuffer();
            m_buffer->open(QIODevice::WriteOnly);
            m_writer = new KoXmlWriter(m_buffer);
        }

        ~fld_State()
        {
            delete m_writer;
            m_writer = 0;
            delete m_buffer;
            m_buffer = 0;
        }

        // Set to UNSUPPORTED for a field we can't handle.
        fldType m_type;

        bool m_insideField;
        bool m_afterSeparator;

        // Whether to interpret the field content as a hyperlink.
        bool m_hyperLinkActive;

        // Stores the location (bookmark/URL) to jump to.
        QString m_hyperLinkUrl;

        // The text:reference-format value to be used in text:bookmark-ref.
        QString m_refFormat;

        // Name of a KoGenStyle for the <text:span> element encapsulating the
        // XML interpretation of the processed field (if applicable).
        QString m_styleName;

        // The tab leader character for a TOC entry.
        QChar m_tabLeader;

        // Stores field instructions.
        QString m_instructions;

        // Stores the field result. NOTE: Disabled, because we use either
        // m_writer or save the result as vanilla text.
/*         QString m_result; */

        // A writer and buffer used to interpret bookmark elements and tabs in
        // the field result (if applicable to the field type).
        KoXmlWriter* m_writer;
        QBuffer* m_buffer;
    };

    std::stack<fld_State *> m_fldStates;
    void fld_saveState();
    void fld_restoreState();

    //storage for XML snippets of already processed fields
    QList<QString> m_fld_snippets;

    // Current field.
    fld_State *m_fld;

    //counters
    int m_fldStart;
    int m_fldEnd;

    //character properties applied to the bunch of nested fields
    wvWare::SharedPtr<const wvWare::Word97::CHP> m_fldChp;

    int m_textBoxX;
    int m_textBoxY;

    // ************************************************
    //  Obsolete
    // ************************************************

#ifdef TEXTHANDLER_OBSOLETE

public:
    // Write a <FORMAT> tag from the given CHP
    // Returns that element into pChildElement if set (in that case even an empty FORMAT can be appended)
    void writeFormattedText(KoGenStyle* textStyle, const wvWare::Word97::CHP* chp,
                            const wvWare::Word97::CHP* refChp, QString text, bool writeText, QString styleName);

    // Write the _contents_ (children) of a <LAYOUT> or <STYLE> tag, from the given parag props
    void writeLayout(const wvWare::ParagraphProperties& paragraphProperties, KoGenStyle* paragraphStyle,
                     const wvWare::Style* style, bool writeContentTags, QString namedStyle);

protected:
    QDomElement insertVariable(int type, wvWare::SharedPtr<const wvWare::Word97::CHP> chp, const QString& format);
    QDomElement insertAnchor(const QString& fsname);

private:
    enum { NoShadow, Shadow, Imprint } m_shadowTextFound;

    QDomElement m_framesetElement;
    QDomElement m_oldLayout;
    QDomElement m_formats;
    int m_index;
#endif
};

#endif // TEXTHANDLER_H
