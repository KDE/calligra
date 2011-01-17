/* This file is part of the KOffice project
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
#include "versionmagic.h"
#include "paragraph.h"

#include <wv2/src/handlers.h>
#include <wv2/src/functordata.h>
#include <wv2/src/lists.h>
#include <QString>
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
class PAP;
}
}

class KWordReplacementHandler : public wvWare::InlineReplacementHandler
{
public:
    virtual wvWare::U8 hardLineBreak();
    virtual wvWare::U8 nonBreakingHyphen();
    virtual wvWare::U8 nonRequiredHyphen();
};


class KWordTextHandler : public QObject, public wvWare::TextHandler
{
    Q_OBJECT
public:
    KWordTextHandler(wvWare::SharedPtr<wvWare::Parser> parser, KoXmlWriter* bodyWriter, KoGenStyles* mainStyles);
    ~KWordTextHandler() { Q_ASSERT (m_fldStart == m_fldEnd); }

    //////// TextHandler interface

    virtual void sectionStart(wvWare::SharedPtr<const wvWare::Word97::SEP> sep);
    virtual void sectionEnd();
    virtual void pageBreak();
    virtual void headersFound(const wvWare::HeaderFunctor& parseHeaders);
    virtual void footnoteFound(wvWare::FootnoteData::Type type, wvWare::UString characters,
                               wvWare::SharedPtr<const wvWare::Word97::CHP> chp,
                               const wvWare::FootnoteFunctor& parseFootnote);
    virtual void annotationFound(wvWare::UString characters,
                                 wvWare::SharedPtr<const wvWare::Word97::CHP> chp,
                                 const wvWare::AnnotationFunctor& parseAnnotation);

    virtual void paragraphStart(wvWare::SharedPtr<const wvWare::ParagraphProperties> paragraphProperties);
    virtual void paragraphEnd();
    virtual void fieldStart(const wvWare::FLD* fld, wvWare::SharedPtr<const wvWare::Word97::CHP> chp);
    virtual void fieldSeparator(const wvWare::FLD* fld, wvWare::SharedPtr<const wvWare::Word97::CHP> chp);
    virtual void fieldEnd(const wvWare::FLD* fld, wvWare::SharedPtr<const wvWare::Word97::CHP> chp);
    virtual void runOfText(const wvWare::UString& text, wvWare::SharedPtr<const wvWare::Word97::CHP> chp);

    virtual void tableRowFound(const wvWare::TableRowFunctor& functor, wvWare::SharedPtr<const wvWare::Word97::TAP> tap);
    virtual void tableEndFound();
    virtual void bookmarkStart( const wvWare::BookmarkData& data );
    virtual void bookmarkEnd( const wvWare::BookmarkData& data );

#ifdef IMAGE_IMPORT
    virtual void inlineObjectFound(const wvWare::PictureData& data);
    virtual void floatingObjectFound(unsigned int globalCP );
#endif // IMAGE_IMPORT

    ///////// Our own interface

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

    // Provide access to private attributes for our handlers
    Document* document() const { return m_document; }
    void setDocument(Document * document) { m_document = document; }
    void set_breakBeforePage(bool val) { m_breakBeforePage = val; }
    bool breakBeforePage(void) const { return m_breakBeforePage; }
    int sectionNumber(void) const { return m_sectionNumber; }

    //TODO: let's try to solve all list related stuff locally in TextHandler
    bool listIsOpen(); //tell us whether a list is open
    void closeList();

    // Communication with Document, without having to know about Document
signals:
    void sectionFound(wvWare::SharedPtr<const wvWare::Word97::SEP>);
    void sectionEnd(wvWare::SharedPtr<const wvWare::Word97::SEP>);
    void subDocFound(const wvWare::FunctorBase* parsingFunctor, int data);
    void footnoteFound(const wvWare::FunctorBase* parsingFunctor, int data);
    void annotationFound(const wvWare::FunctorBase* parsingFunctor, int data);
    void headersFound(const wvWare::FunctorBase* parsingFunctor, int data);
    void tableFound(KWord::Table* table);
    void inlineObjectFound(const wvWare::PictureData& data, KoXmlWriter* writer);
    void floatingObjectFound(unsigned int globalCP, KoXmlWriter* writer);
/*     void updateListDepth(int); */

private:
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

    KWord::Table* m_currentTable;
    Paragraph *m_paragraph; //pointer to paragraph object
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
    //  State
    // ************************************************

    //save/restore for processing footnotes (very similar to the wv2 method)
    struct State {
        State(KWord::Table* curTab, Paragraph* para, QString lStyleName,
              int curListDepth, int curListID, const QMap<int, QString> &preLists) :
                currentTable(curTab), paragraph(para), listStyleName(lStyleName),
                currentListDepth(curListDepth), currentListID(curListID),
                previousLists(preLists) {}

        KWord::Table* currentTable;
        Paragraph* paragraph;
        QString listStyleName;
        int currentListDepth; //tells us which list level we're on (-1 if not in a list)
        int currentListID; //tracks the id of the current list - 0 if no list
        QMap<int, QString> previousLists; //remember previous lists, to continue numbering
    };

    std::stack<State> m_oldStates;
    void saveState();
    void restoreState();

    // ************************************************
    //  List related
    // ************************************************

    bool writeListInfo(KoXmlWriter* writer, const wvWare::Word97::PAP& pap, const wvWare::ListInfo* listInfo);
    QString createBulletStyle(const QString& textStyleName) const;
    void updateListStyle(const QString& textStyleName);

    QString m_listSuffixes[9]; // The suffix for every list level seen so far
    QString m_listStyleName; //track the name of the list style
    bool m_listLevelStyleRequired; //track if a list-level-style is required for current paragraph
    int m_currentListDepth; //tells us which list level we're on (-1 if not in a list)
    int m_currentListID; //tracks the id of the current list - 0 if no list
    wvWare::SharedPtr<const wvWare::ParagraphProperties> m_currentPPs; //paragraph properties

    //int m_previousListID; //track previous list, in case we need to continue the numbering
    QMap<int, QString> m_previousLists; //remember previous lists, to continue numbering
    QStack <KoXmlWriter*> m_usedListWriters;

    // ************************************************
    //  Field related
    // ************************************************

    //field type enumeration as defined in [MS-DOC.pdf] page 356
    enum fldType
    {
        UNSUPPORTED = 0,
        //PARSE_ERROR = 0x01, ///< Specifies that the field was unable to be parsed.
        //REF_WITHOUT_KEYWORD = 0x02, ///< Specifies that the field represents a REF field where the keyword has been omitted.
        //REF = 0x03, ///< Reference
        //FTNREF = 0x05, ///< Identicial to NOTEREF (not a reference)
        //SET = 0x06,
        //IF = 0x07,
        //INDEX = 0x08,
        //STYLEREF = 0x0a,
        //SEQ = 0x0c,
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
        //AUTOTEXTLIST = 0x59,
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
            m_hyperLinkUrl(QString::null),
            m_instructions(QString::null),
            m_result(QString::null),
            m_styleName(QString::null),
            m_tocSeparator(QString::null),
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

        //set to UNSUPPORTED for a field we can't handle, anything else is the field type
        fldType m_type;
        
	//other field related variables
        bool m_insideField;
        bool m_afterSeparator;
        bool m_hyperLinkActive;
	
	//stores the location (bookmark/URL) to jump to
        QString m_hyperLinkUrl;
        
	//stores field instructions
        QString m_instructions;
        
	//stores the field result
        QString m_result;
        
        //KoGenStyle name for the <text:span> element encapsulating content of the
	//processed field (if applicable)
        QString m_styleName;

        //separates an TOC entry and its page number (if applicable)
        QString m_tocSeparator;

	//writer and buffer used to place bookmark elements into the field result,
        //if bookmarks are not to be supported by your field type, use m_result
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
