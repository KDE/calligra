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

#ifndef _IMPORT_FORMATTING_H
#define _IMPORT_FORMATTING_H

#include <qptrstack.h>
#include <qstring.h>
#include <qcolor.h>
#include <qxml.h>
#include <qdom.h>

#include "ImportHelpers.h"

// Tags in lower case (e.g. <c>) are AbiWord's ones.
// Tags in upper case (e.g. <TEXT>) are KWord's ones.

// Note: as we are not validating anything, we are quite tolerant about the file
//   that we will read. So some element are not checked:
//   <styles>
enum StackItemElementType{
    ElementTypeUnknown  = 0,
    ElementTypeBottom,      // Bottom of the stack
    ElementTypeIgnore,      // Element is known but ignored
    ElementTypeEmpty,       // Element is empty ( <pagesize>, <s>)
    ElementTypeAbiWord,     // <abiword>
    ElementTypeSection,     // <section>
    ElementTypeParagraph,   // <p>
    ElementTypeContent,     // <c>
    ElementTypeField        // <field>
};

class StackItem
{
public:
    StackItem();
    ~StackItem();
public:
    QString itemName;   // Name of the tag (only for error purposes)
    StackItemElementType elementType;
    QDomElement stackElementParagraph; // <PARAGRAPH>
    QDomElement stackElementText; // <TEXT>
    QDomElement stackElementFormatsPlural; // <FORMATS>
    QString     fontName;
    int         fontSize;
    int         pos; //Position
    bool        italic;
    bool        bold;
    bool        underline;
    bool        strikeout;
    QColor      fgColor;
    QColor      bgColor;
    int         textPosition; //Normal (0), subscript(1), superscript (2)
};

class StackItemStack : public QPtrStack<StackItem>
{
public:
        StackItemStack(void) { }
        virtual ~StackItemStack(void) { }
};

void PopulateProperties(StackItem* stackItem, const QString& strStyleProps,
    const QXmlAttributes& attributes, AbiPropsMap& abiPropsMap,
    const bool allowInit);
void AddFormat(QDomElement& formatElementOut, StackItem* stackItem,
    QDomDocument& mainDocument);


#endif // _IMPORT_FORMATTING_H
