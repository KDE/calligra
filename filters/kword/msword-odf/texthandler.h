/* This file is part of the KOffice project
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2002 David Faure <faure@kde.org>
   Copyright (C) 2008 Benjamin Cail <cricketc@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   version 2 of the License, or (at your option) version 3 or,
   at the discretion of KDE e.V (which shall act as a proxy as in
   section 14 of the GPLv3), any later version..

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef TEXTHANDLER_H
#define TEXTHANDLER_H

#include "tablehandler.h"
#include "versionmagic.h"

#include <wv2/handlers.h>
#include <QString>
#include <QObject>
#include <qdom.h>

#include <KoXmlWriter.h>
#include <KoGenStyles.h>

#include <string>

namespace wvWare {
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
    KWordTextHandler( wvWare::SharedPtr<wvWare::Parser> parser, KoXmlWriter* bodyWriter, KoGenStyles* mainStyles );

    //////// TextHandler interface

    virtual void sectionStart( wvWare::SharedPtr<const wvWare::Word97::SEP> sep );
    virtual void sectionEnd();
    virtual void headersFound( const wvWare::HeaderFunctor& parseHeaders );
    virtual void footnoteFound( wvWare::FootnoteData::Type type, wvWare::UChar character,
                                wvWare::SharedPtr<const wvWare::Word97::CHP> chp, const wvWare::FootnoteFunctor& parseFootnote );

    virtual void paragraphStart( wvWare::SharedPtr<const wvWare::ParagraphProperties> paragraphProperties );
    virtual void paragraphEnd();
    virtual void fieldStart( const wvWare::FLD* fld, wvWare::SharedPtr<const wvWare::Word97::CHP> chp );
    virtual void fieldSeparator( const wvWare::FLD* fld, wvWare::SharedPtr<const wvWare::Word97::CHP> chp );
    virtual void fieldEnd( const wvWare::FLD* fld, wvWare::SharedPtr<const wvWare::Word97::CHP> chp );
    virtual void runOfText( const wvWare::UString& text, wvWare::SharedPtr<const wvWare::Word97::CHP> chp );

    virtual void tableRowFound( const wvWare::TableRowFunctor& functor, wvWare::SharedPtr<const wvWare::Word97::TAP> tap );

#ifdef IMAGE_IMPORT
    virtual void pictureFound( const wvWare::PictureFunctor& picture, wvWare::SharedPtr<const wvWare::Word97::PICF> picf,
                               wvWare::SharedPtr<const wvWare::Word97::CHP> chp );
#endif // IMAGE_IMPORT
    ///////// Our own interface, also used by processStyles

    // Write a <FORMAT> tag from the given CHP
    // Returns that element into pChildElement if set (in that case even an empty FORMAT can be appended)
    void writeFormattedText( QDomElement& parentElement, const wvWare::Word97::CHP* chp, const wvWare::Word97::CHP* refChp,/* int pos,*/ int len, int formatId, QDomElement* pChildElement );

    // Write the _contents_ (children) of a <LAYOUT> or <STYLE> tag, from the given parag props
    void writeLayout( const wvWare::Style* style );

    bool m_writeTextToStylesDotXml; //flag for headers & footers, where we write the actual text to styles.xml
    KoXmlWriter* m_tmpWriter; //for header/footer writing

    // Communication with Document, without having to know about Document
signals:
    void firstSectionFound( wvWare::SharedPtr<const wvWare::Word97::SEP> );
    void subDocFound( const wvWare::FunctorBase* parsingFunctor, int data );
    void tableFound( const KWord::Table& table );
    void pictureFound( const QString& frameName, const QString& pictureName, const wvWare::FunctorBase* pictureFunctor );
    void updateListDepth( int );

protected:
    QDomElement insertVariable( int type, wvWare::SharedPtr<const wvWare::Word97::CHP> chp, const QString& format );
    QDomElement insertAnchor( const QString& fsname );
    QString getFont(unsigned fc) const;
    KoXmlWriter* m_bodyWriter; //this writes to content.xml inside <office:body>
    KoGenStyles* m_mainStyles; //this is for collecting most of the styles

private:
    wvWare::SharedPtr<wvWare::Parser> m_parser;
    QString m_listSuffixes[9]; // The suffix for every list level seen so far
    QDomElement m_framesetElement;
    int m_sectionNumber;
    int m_footNoteNumber; // number of footnote _vars_ written out
    int m_endNoteNumber; // number of endnote _vars_ written out
    int m_currentListDepth; //tells us which list level we're on (-1 if not in a list)
    int m_currentListID; //tracks the id of the current list - 0 if no list
    //int m_textStyleNumber; //number of styles created for text family
    //int m_paragraphStyleNumber; //number of styles created for paragraph family
    //int m_listStyleNumber; //number of styles created for lists

    // Current paragraph
    QString m_runOfText;
    const wvWare::Style* m_currentStyle;
    wvWare::SharedPtr<const wvWare::ParagraphProperties> m_paragraphProperties;
    enum { NoShadow, Shadow, Imprint } m_shadowTextFound;
    int m_index;
    QDomElement m_formats;
    QDomElement m_oldLayout;

    KWord::Table* m_currentTable;
    bool m_bInParagraph;
    //flag that tells us to start a new page
    bool m_bStartNewPage;

    QString m_fieldValue;
    bool m_insideField;
    bool m_fieldAfterSeparator;
    int m_fieldType; // 0 for no field
};

#endif // TEXTHANDLER_H
