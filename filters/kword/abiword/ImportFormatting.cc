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

#include <kdebug.h>

#include "ImportFormatting.h"

StackItem::StackItem() : fontSize(0), /* No explicit font size */
    pos(0), italic(false), bold(false), underline(false), strikeout(false),
    textPosition(0)
{
}

StackItem::~StackItem()
{
}

void PopulateProperties(StackItem* stackItem,
    const QXmlAttributes& attributes, AbiPropsMap& abiPropsMap,
    const bool allowInit)
// TODO: find a better name for this function
{
    if (allowInit)
    {
        // Initialize the QStrings with the previous values of the properties
        // TODO: any others needed?
        if (stackItem->italic)
        {
            abiPropsMap.setProperty("font-style","italic");
        }
        if (stackItem->bold)
        {
            abiPropsMap.setProperty("font-weight","bold");
        }

        if (stackItem->underline)
        {
            abiPropsMap.setProperty("text-decoration","underline");
        }
        else if (stackItem->strikeout)
        {
            abiPropsMap.setProperty("text-decoration","line-through");
        }
    }

    kdDebug(30506)<< "========== props=\"" << attributes.value("props") << "\"" << endl;
    // Treat the props attributes in the two available flavors: lower case and upper case.
    abiPropsMap.splitAndAddAbiProps(attributes.value("props"));
    abiPropsMap.splitAndAddAbiProps(attributes.value("PROPS")); // PROPS is deprecated

    stackItem->italic=(abiPropsMap["font-style"].getValue()=="italic");
    stackItem->bold=(abiPropsMap["font-weight"].getValue()=="bold");

    QString strDecoration=abiPropsMap["text-decoration"].getValue();
    stackItem->underline=(strDecoration=="underline");
    stackItem->strikeout=(strDecoration=="line-through");

    QString strTextPosition=abiPropsMap["text-position"].getValue();
    if (strTextPosition=="subscript")
    {
        stackItem->textPosition=1;
    }
    else if (strTextPosition=="superscript")
    {
        stackItem->textPosition=2;
    }
    else if (!strTextPosition.isEmpty())
    {
        // we have any other new value, assume it means normal!
        stackItem->textPosition=0;
    }

    QString strColour=abiPropsMap["color"].getValue();
    if (!strColour.isEmpty())
    {
        // The colour information is *not* lead by a hash (#)
        stackItem->fgColor.setNamedColor("#"+strColour);
   }

    QString strBackgroundTextColor=abiPropsMap["bgcolor"].getValue();
    if(!strBackgroundTextColor.isEmpty())
    {
        // The colour information is *not* lead by a hash (#)
        stackItem->bgColor.setNamedColor("#"+strBackgroundTextColor);
    }

    QString strFontSize=abiPropsMap["font-size"].getValue();
    if (!strFontSize.isEmpty())
    {
        const int size=int(ValueWithLengthUnit(strFontSize));
        if (size>0)
        {
            stackItem->fontSize=size;
        }
    }

    QString strFontFamily=abiPropsMap["font-family"].getValue();
    if (!strFontFamily.isEmpty())
    {
        // TODO: transform the font-family in a font we have on the system on which KWord runs.
        stackItem->fontName=strFontFamily;
    }
}

void AddFormat(QDomElement& formatElementOut, StackItem* stackItem, QDomDocument& mainDocument)
{
    QDomElement element;
    if (!stackItem->fontName.isEmpty())
    {
        element=mainDocument.createElement("FONT");
        element.setAttribute("name",stackItem->fontName); // Font name
        formatElementOut.appendChild(element); //Append to <FORMAT>
    }

    if (stackItem->fontSize)
    {
        element=mainDocument.createElement("SIZE");
        element.setAttribute("value",stackItem->fontSize);
        formatElementOut.appendChild(element); //Append to <FORMAT>
    }

    if (stackItem->italic)
    {
        element=mainDocument.createElement("ITALIC");
        element.setAttribute("value",1);
        formatElementOut.appendChild(element); //Append to <FORMAT>
    }

    if (stackItem->bold)
    {
        element=mainDocument.createElement("WEIGHT");
        element.setAttribute("value",75);
        formatElementOut.appendChild(element); //Append to <FORMAT>
    }

    if (stackItem->underline)
    {
        element=mainDocument.createElement("UNDERLINE");
        element.setAttribute("value",1);
        formatElementOut.appendChild(element); //Append to <FORMAT>
    }

    if (stackItem->strikeout)
    {
        element=mainDocument.createElement("STRIKEOUT");
        element.setAttribute("value",1);
        formatElementOut.appendChild(element); //Append to <FORMAT>
    }

    if (stackItem->textPosition)
    {
        element=mainDocument.createElement("VERTALIGN");
        element.setAttribute("value",stackItem->textPosition);
        formatElementOut.appendChild(element); //Append to <FORMAT>
    }

    if (stackItem->fgColor.isValid())
    {
        element=mainDocument.createElement("COLOR");
        element.setAttribute("red",  stackItem->fgColor.red());
        element.setAttribute("green",stackItem->fgColor.green());
        element.setAttribute("blue", stackItem->fgColor.blue());
        formatElementOut.appendChild(element); //Append to <FORMAT>
    }

    if (stackItem->bgColor.isValid())
    {
        element=mainDocument.createElement("TEXTBACKGROUNDCOLOR");
        element.setAttribute("red",  stackItem->bgColor.red());
        element.setAttribute("green",stackItem->bgColor.green());
        element.setAttribute("blue", stackItem->bgColor.blue());
        formatElementOut.appendChild(element); //Append to <FORMAT>
    }
}
