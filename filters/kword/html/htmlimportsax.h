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
    CSS2Styles(QString newName, void* newValue) : name(newName), value(newValue) {};
    virtual ~CSS2Styles() {};
    QString name;
    void* value;
};

void TreatCSS2Styles(QString strProps,QValueList<CSS2Styles> &css2StylesList);

enum StackItemElementType{
    ElementTypeUnknown  = 0,
    ElementTypeBottom,      // Bottom of the stack
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
        fontName="times"; //Default font
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
    QDomNode    stackNode,stackNode2;
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
bool StartElementSpan(StackItem* stackItem, StackItem* stackCurrent, const QString& strStyleLocal, const QString& strStyleAttribute);
bool charactersElementSpan (StackItem* stackItem, const QString & ch);
bool EndElementSpan (StackItem* stackItem, StackItem* stackCurrent);
bool StartElementP(StackItem* stackItem, StackItem* stackCurrent, QDomElement& mainFramesetElement,
        const QString& strStyleLocal, const QString& strStyleAttribute, const QString& strAlign);
bool charactersElementP (StackItem* stackItem, const QString & ch);
bool EndElementP (StackItem* stackItem);

#endif // HTMLIMPORTSAX_H