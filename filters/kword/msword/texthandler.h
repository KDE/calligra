/* This file is part of the KOffice project
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2002 David Faure <faure@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License version 2 as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef TEXTHANDLER_H
#define TEXTHANDLER_H

#include <handlers.h>
#include <qstring.h>
#include <qobject.h>
#include <qdom.h>

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
    KWordTextHandler( wvWare::SharedPtr<wvWare::Parser> parser );

    void setFrameSetElement( const QDomElement& frameset );

    //////// TextHandler interface

    virtual void sectionStart( wvWare::SharedPtr<const wvWare::Word97::SEP> sep );
    virtual void sectionEnd();
    virtual void pageBreak();
    virtual void headersFound( const wvWare::HeaderFunctor& parseHeaders );
    virtual void footnoteFound( wvWare::FootnoteData::Type type, wvWare::UChar character,
                                wvWare::SharedPtr<const wvWare::Word97::CHP> chp, const wvWare::FootnoteFunctor& parseFootnote );

    virtual void paragraphStart( wvWare::SharedPtr<const wvWare::ParagraphProperties> paragraphProperties );
    virtual void paragraphEnd();
    virtual void runOfText( const wvWare::UString& text, wvWare::SharedPtr<const wvWare::Word97::CHP> chp );

    ///////// Our own interface, also used by processStyles

    // Called at the beginning of a parag/style
    void paragLayoutBegin();

    // Write a <FORMAT> tag from the given CHP
    // Returns that element into pChildElement if set (in that case even an empty FORMAT can be appended)
    void writeFormat( QDomElement& parentElement, const wvWare::Word97::CHP* chp, const wvWare::Word97::CHP* refChp, int pos, int len, int formatId, QDomElement* pChildElement );

    // Write the _contents_ (children) of a <LAYOUT> or <STYLE> tag, from the given parag props
    void writeLayout( QDomElement& parentElement, const wvWare::ParagraphProperties& paragraphProperties, const wvWare::Style* style );

    // Communication with Document, without having to know about Document
signals:
    void firstSectionFound( wvWare::SharedPtr<const wvWare::Word97::SEP> );
    void subDocFound( const wvWare::FunctorBase* parsingFunctor );

protected:
    void writeOutParagraph( const QString& styleName, const QString& text );
    void writeCounter( QDomElement& parentElement, const wvWare::ParagraphProperties& paragraphProperties, const wvWare::Style* style );
    QDomElement insertVariable( int type, wvWare::SharedPtr<const wvWare::Word97::CHP> chp, const QString& format );
    QString getFont(unsigned fc) const;
    QDomDocument mainDocument() const;

private:
    wvWare::SharedPtr<wvWare::Parser> m_parser;
    QDomElement m_framesetElement;
    int m_sectionNumber;
    int m_footNoteNumber; // number of footnote _vars_ written out

    // Current paragraph
    QString m_paragraph;
    const wvWare::Style* m_currentStyle;
    wvWare::SharedPtr<const wvWare::ParagraphProperties> m_paragraphProperties;
    enum { NoShadow, Shadow, Imprint } m_shadowTextFound;
    int m_index;
    QDomElement m_formats;
    QDomElement m_oldLayout;
};

#endif // TEXTHANDLER_H
