/* This file is part of the KDE project
   Copyright (C) 2003 Percy Leonhardt

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
*/


#ifndef STYLEFACTORY_H
#define STYLEFACTORY_H

#include <qptrlist.h>
#include <qstring.h>

#include <qdom.h>

class QDomDocument;
class QDomElement;

class PageStyle
{
public:
    PageStyle( QDomElement & page, const uint index );
    ~PageStyle() {};

    void toXML( QDomDocument & doc, QDomElement & e ) const;
    bool operator==( const PageStyle & pageStyle ) const;
    QString name() const { return m_name; };
    QString style() const { return m_style; };

private:
    PageStyle() {};

    QString m_name, m_page_width, m_page_height, m_orientation, m_style;
    QString m_margin_top, m_margin_bottom, m_margin_left, m_margin_right;
};

class TextStyle
{
public:
    TextStyle() {};
    ~TextStyle() {};

private:
    QString name;
};

class GraphicStyle
{
public:
    GraphicStyle() {};
    ~GraphicStyle() {};

private:
    QString name;
};

class ParagraphStyle
{
public:
    ParagraphStyle() {};
    ~ParagraphStyle() {};

private:
    QString name;
};

class PresentationStyle
{
public:
    PresentationStyle() {};
    ~PresentationStyle() {};

private:
    QString name;
};

class StyleFactory
{
public:
    StyleFactory();
    ~StyleFactory();

    void addOfficeStyles( QDomDocument & doc, QDomElement & styles );
    void addOfficeMaster( QDomDocument & doc, QDomElement & master );
    void addOfficeAutomatic( QDomDocument & doc, QDomElement & automatic );
    void addAutomaticStyles( QDomDocument & doc, QDomElement & autoStyles );

    QString createPageStyle( QDomElement & e );
    QString createTextStyle( QDomElement & e );
    QString createGraphicStyle( QDomElement & e );
    QString createParagraphStyle( QDomElement & e );
    QString createPresentationStyle( QDomElement & e );

    static QString toCM( const QString & point );

private:
    void addTextStyles( QDomDocument & doc, QDomElement & autoStyles );
    void addGraphicStyles( QDomDocument & doc, QDomElement & autoStyles );
    void addParagraphStyles( QDomDocument & doc, QDomElement & autoStyles );
    void addPresentationStyles( QDomDocument & doc, QDomElement & autoStyles );

    QPtrList<PageStyle>         m_pageStyles;
    QPtrList<TextStyle>         m_textStyles;
    QPtrList<GraphicStyle>      m_graphicStyles;
    QPtrList<ParagraphStyle>    m_paragraphStyles;
    QPtrList<PresentationStyle> m_presentationStyles;
};

#endif
