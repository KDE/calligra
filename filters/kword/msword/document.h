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

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <handlers.h>
#include <qstring.h>
#include <qcolor.h>
#include <qdom.h>

#include <string>

namespace wvWare {
    class Style;
    class Parser;
}

class KWordCharacterHandler : public wvWare::SpecialCharHandler
{
public:
    virtual wvWare::U8 hardLineBreak();
    virtual wvWare::U8 nonBreakingHyphen();
    virtual wvWare::U8 nonRequiredHyphen();
};

class Document : public wvWare::BodyTextHandler
{
public:
    Document( const std::string& fileName, QDomDocument& mainDocument, QDomElement& mainFramesetElement );
    virtual ~Document();

    bool parse();

    virtual void sectionStart();
    virtual void sectionEnd();

    virtual void paragraphStart( wvWare::SharedPtr<const wvWare::Word97::PAP> pap );
    virtual void paragraphEnd();
    virtual void runOfText( const wvWare::UString& text, wvWare::SharedPtr<const wvWare::Word97::CHP> chp );

    virtual void pageBreak();

private:
    QString getFont(unsigned fc) const;
    void prepareDocument();
    void processStyles();
    void writeOutParagraph( const QString& styleName, const QString& text );
    // Write a <FORMAT> tag from the given CHP
    void writeFormat( QDomElement& parentElement, const wvWare::Word97::CHP* chp, const wvWare::Word97::CHP* refChp, int pos, int len );
    // Write the _contents_ (children) of a <LAYOUT> or <STYLE> tag, from the given PAP
    void writeLayout( QDomElement& parentElement, const wvWare::Word97::PAP* pap );

    QString m_paragraph;
    QDomDocument& m_mainDocument;
    QDomElement& m_mainFramesetElement;
    QDomElement m_formats;
    QDomElement m_oldLayout;
    int m_index;
    int m_sectionNumber;
    const wvWare::Style* m_paragStyle;
    wvWare::SharedPtr<const wvWare::Word97::PAP> m_pap;
    KWordCharacterHandler* m_charHandler;
    wvWare::SharedPtr<wvWare::Parser> m_parser;
};

#endif // DOCUMENT_H
