/* This file is part of the KOffice project
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2002 David Faure <faure@kde.org>
   Copyright (C) 2008 Benjamin Cail <cricketc@gmail.com>
   Copyright (C) 2009 Inge Wallin   <inge@lysator.liu.se>

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

#ifdef IMAGE_IMPORT
    virtual void inlineObjectFound(const wvWare::PictureData& data);
#endif // IMAGE_IMPORT
    virtual void floatingObjectFound(unsigned int globalCP );

    virtual void bookmarkStart( const wvWare::BookmarkData& data );
    virtual void bookmarkEnd( const wvWare::BookmarkData& data );

    ///////// Our own interface, also used by processStyles

    Document* document() const {
        return m_document;
    }
    void setDocument(Document * document) {
        m_document = document;
    }
    bool pageNumberFieldType() const {
        return m_fieldType == 33;
    }

    // Write a <FORMAT> tag from the given CHP
    // Returns that element into pChildElement if set (in that case even an empty FORMAT can be appended)
    //void writeFormattedText(KoGenStyle* textStyle, const wvWare::Word97::CHP* chp, const wvWare::Word97::CHP* refChp, QString text, bool writeText, QString styleName);

    // Write the _contents_ (children) of a <LAYOUT> or <STYLE> tag, from the given parag props
    //void writeLayout(const wvWare::ParagraphProperties& paragraphProperties, KoGenStyle* paragraphStyle, const wvWare::Style* style, bool writeContentTags, QString namedStyle);

    bool listIsOpen(); //tell us whether a list is open
    void closeList();
    QString m_listStyleName; //track the name of the list style
    KoGenStyles* m_mainStyles; //this is for collecting most of the styles
    int m_sectionNumber;
    QString getFont(unsigned fc) const;

    // Provide access to private attributes for our handlers
    void set_breakBeforePage(bool val) { m_breakBeforePage = val; }
    bool breakBeforePage(void) const { return m_breakBeforePage; }

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
    void updateListDepth(int);

protected:
    QDomElement insertVariable(int type, wvWare::SharedPtr<const wvWare::Word97::CHP> chp, const QString& format);
    QDomElement insertAnchor(const QString& fsname);
    KoXmlWriter* m_bodyWriter; //this writes to content.xml inside <office:body>

private:
    // The document owning this text handler.
    Document* m_document;

    wvWare::SharedPtr<wvWare::Parser> m_parser;
    QString m_listSuffixes[9]; // The suffix for every list level seen so far
    QDomElement m_framesetElement;
    int m_footNoteNumber; // number of footnote _vars_ written out
    int m_endNoteNumber; // number of endnote _vars_ written out
    //int m_textStyleNumber; //number of styles created for text family
    //int m_paragraphStyleNumber; //number of styles created for paragraph family
    //int m_listStyleNumber; //number of styles created for lists

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

    QStack <KoXmlWriter*> m_usedListWriters;

    // Current paragraph
    wvWare::SharedPtr<const wvWare::Word97::SEP> m_sep; //store section info for section end
    enum { NoShadow, Shadow, Imprint } m_shadowTextFound;
    int m_index;
    QDomElement m_formats;
    QDomElement m_oldLayout;

    KWord::Table* m_currentTable;

    //pointer to paragraph object
    Paragraph *m_paragraph;
#if 1
    bool       m_hasStoredDropCap; // True if the previous paragraph was a dropcap
    int        m_dcs_fdct;
    int        m_dcs_lines;
    qreal      m_dropCapDistance;
#endif
    QString    m_dropCapString;
    QString    m_dropCapStyleName;

    bool m_insideFootnote;
    KoXmlWriter* m_footnoteWriter; //write the footnote data, then add it to bodyWriter
    QBuffer* m_footnoteBuffer; //buffer for the footnote data

    bool m_insideAnnotation;
    KoXmlWriter* m_annotationWriter; //write the annotation data, then add it to bodyWriter
    QBuffer* m_annotationBuffer; //buffer for the annotation data

    bool m_insideDrawing;
    KoXmlWriter* m_drawingWriter; //write the drawing data, then add it to bodyWriter

    int m_maxColumns; //max number of columns in a table

    bool writeListInfo(KoXmlWriter* writer, const wvWare::Word97::PAP& pap, const wvWare::ListInfo* listInfo);
    int m_currentListDepth; //tells us which list level we're on (-1 if not in a list)
    int m_currentListID; //tracks the id of the current list - 0 if no list
    //int m_previousListID; //track previous list, in case we need to continue the numbering
    //QString m_previousListStyleName;
    QMap<int, QString> m_previousLists; //remember previous lists, to continue numbering

    //if TRUE, the fo:break-before="page" property is required because a manual
    //page break (an end-of-section character not at the end of a section) was
    //found in the main document
    bool m_breakBeforePage;

    // ************************************************
    //  Field related
    // ************************************************

    //save/restore for processing field (very similar to the wv2 method)
    struct fld_State
    {
        fld_State(int type, bool inside, bool afterSep, bool hyperLink, bool bkmkRef,
                  QString inst, QString result, QString stlName, KoXmlWriter* writer, QBuffer* buf) :
        type(type),
        inside(inside),
        afterSeparator(afterSep),
        hyperLinkActive(hyperLink),
        bkmkRefActive(bkmkRef),
        instructions(inst),
        result(result),
        styleName(stlName),
        writer(writer),
        buffer(buf) {}

        int type;
        bool inside;
        bool afterSeparator;
        bool hyperLinkActive;
        bool bkmkRefActive;
        QString instructions;
        QString result;
        QString styleName;
        KoXmlWriter* writer;
        QBuffer* buffer;
    };

    std::stack<fld_State> m_fldStates;
    void fld_saveState();
    void fld_restoreState();

    //storage for XML snippets of already processed fields
    QList<QString> fld_snippets;

    //field type enumeration as defined in MS-DOC page 354/609
    typedef enum
    {
        UNSUPPORTED = 0,
        REF = 3,
        TOC = 13,
        NUMPAGES = 26,
        PAGE = 33,
        PAGEREF = 37,
        EQ = 49,
        HYPERLINK = 88,
    } fldType;

    //set to 0 for a field we can't handle, anything else is the field type
    int m_fieldType;

    //other field related variables
    bool m_insideField;
    bool m_fieldAfterSeparator;
    bool m_hyperLinkActive;
    bool m_bkmkRefActive;

    //writer and buffer used to place bookmark elements into the field result,
    //if bookmarks are not to be supported by your field type, use m_fldResult
    KoXmlWriter* m_fldWriter;
    QBuffer* m_fldBuffer;

    QString m_fldInst;   //stores field instructions
    QString m_fldResult; //stores the field result

    //KoGenStyle name for the <text:span> element encapsulating the field
    //result (if applicable)
    QString m_fldStyleName;

    //counters
    int m_fldStart;
    int m_fldEnd;
};

#endif // TEXTHANDLER_H
