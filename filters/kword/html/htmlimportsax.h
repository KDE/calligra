// $Header$

/* This file is part of the KDE project
   Copyright (C) 2001 Nicolas GOUTTE <nicog@snafu.de>

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

#ifndef HTMLIMPORTSAX_H
#define HTMLIMPORTSAX_H

#include<qdom.h>
class CSS2Styles
{
public:
    CSS2Styles() {};
    CSS2Styles(QString newValue) : m_value(newValue) {};
    virtual ~CSS2Styles() {};
    QString name;
public:
    inline QString getValue(void) const { return m_value; }
private:
    QString m_value;
};

class CSS2StylesMap : public QMap<QString,CSS2Styles>
{
public:
    CSS2StylesMap() {};
    virtual ~CSS2StylesMap() {};
public:
    bool setProperty(QString newName, QString newValue);
};

void TreatCSS2Styles(QString strProps,QValueList<CSS2Styles> &css2StylesList);

enum StackItemElementType{
    ElementTypeUnknown  = 0,
    ElementTypeBottom,      // Bottom of the stack
    ElementTypeEmpty,       // An empty elemnt
    ElementTypeHtml,        // <html>
    ElementTypeBody,        // <body>
    ElementTypeParagraph,   // <p>
    ElementTypeSpan,        // <span>
    ElementTypeDisplayNone  // Do not display, nor its children!
};

class StackItem
{
public:
    StackItem()
    {
        elementName="-none-";
        fontSize=0; //No explicit font size
        italic=false;
        bold=false;
        underline=false;
        strikeout=false;
        red=0;
        green=0;
        blue=0;
        textPosition=0;
    }
    ~StackItem()
    {
    }
public:
    StackItemElementType elementType;
    QDomElement stackElementParagraph; // <PARAGRAPH>
    QDomElement stackElementText; // <TEXT>
    QDomElement stackElementFormatsPlural; // <FORMATS>
    QString     elementName; // Name of the element

    QString     fontName;
    int         fontSize;
    int         pos; //Position
    bool        italic;
    bool        bold;
    bool        underline;
    bool        strikeout;
    int         red;
    int         green;
    int         blue;
    int         textPosition; //Normal (0), subscript(1), superscript (2)
};

bool TransformCSS2ToStackItem(StackItem* stackItem, StackItem* stackCurrent, QString strStyle);
bool StartElementSpan(StackItem* stackItem, StackItem* stackCurrent,
    const QString& strStyleLocal, const QString& strStyleAttribute);
bool charactersElementSpan (StackItem* stackItem, QDomDocument& mainDocument, const QString & ch);
bool EndElementSpan (StackItem* stackItem, StackItem* stackCurrent);
bool StartElementP(StackItem* stackItem, StackItem* stackCurrent,
    QDomDocument& mainDocument, QDomElement& mainFramesetElement,
    const QString& strStyleLocal, const QString& strStyleAttribute,
    const QString& strAlign);
bool charactersElementP (StackItem* stackItem, QDomDocument& mainDocument, const QString & ch);
bool EndElementP (StackItem* stackItem);
bool StartElementBR(StackItem* stackItem, StackItem* stackCurrent,
    QDomDocument& mainDocument,QDomElement& mainFramesetElement);

#endif // HTMLIMPORTSAX_H
