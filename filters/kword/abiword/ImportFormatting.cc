/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2004 Nicolas GOUTTE <goutte@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <kdebug.h>

#include "ImportFormatting.h"
#include "ImportStyle.h"

StackItem::StackItem() : fontSize(0), /* No explicit font size */
    pos(0), italic(false), bold(false), underline(false), strikeout(false),
    textPosition(0)
{
}

StackItem::~StackItem()
{
}

void PopulateProperties(StackItem* stackItem, const QString& strStyleProps,
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

    // Style goes first
    kDebug(30506)<< "===== from style=\"" << strStyleProps << "\"" << endl;
    abiPropsMap.splitAndAddAbiProps(strStyleProps);
    // Treat the props attributes in the two available flavors: lower case and upper case.
    kDebug(30506)<< "========== props=\"" << attributes.value("props") << "\"" << endl;
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

    QString strColor=abiPropsMap["color"].getValue();
    if (!strColor.isEmpty())
    {
        // The color information is *not* lead by a hash (#)
        stackItem->fgColor.setNamedColor("#"+strColor);
   }

    QString strBackgroundTextColor=abiPropsMap["bgcolor"].getValue();
    if (strBackgroundTextColor=="transparent")
    {
        // KWord has no idea what transparency is, so we use white
        stackItem->bgColor.setRgb(255,255,255);
    }
    else if(!strBackgroundTextColor.isEmpty())
    {
        // The color information is *not* lead by a hash (#)
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
    if (!strFontFamily.isEmpty() && (strFontFamily!="(null)"))
    {
        // TODO: transform the font-family in a font that we have on the system on which KWord runs.
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

    if (stackItem->fontSize>0)
    {
        element=mainDocument.createElement("SIZE");
        element.setAttribute("value",stackItem->fontSize);
        formatElementOut.appendChild(element); //Append to <FORMAT>
    }

    element=mainDocument.createElement("ITALIC");
    element.setAttribute("value",stackItem->italic?1:0);
    formatElementOut.appendChild(element); //Append to <FORMAT>

    element=mainDocument.createElement("WEIGHT");
    element.setAttribute("value",stackItem->bold?75:50);
    formatElementOut.appendChild(element); //Append to <FORMAT>

    element=mainDocument.createElement("UNDERLINE");
    element.setAttribute("value",stackItem->underline?1:0);
    formatElementOut.appendChild(element); //Append to <FORMAT>

    element=mainDocument.createElement("STRIKEOUT");
    element.setAttribute("value",stackItem->strikeout?1:0);
    formatElementOut.appendChild(element); //Append to <FORMAT>

    if ((stackItem->textPosition>=0) && (stackItem->textPosition<=2))
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

void AddLayout(const QString& strStyleName, QDomElement& layoutElement,
    StackItem* stackItem, QDomDocument& mainDocument,
    const AbiPropsMap& abiPropsMap, const int level, const bool isStyle)
{
    QDomElement element;
    element=mainDocument.createElement("NAME");
    element.setAttribute("value",strStyleName);
    layoutElement.appendChild(element);

    QString strFollowing=abiPropsMap["followedby"].getValue();
    QDomElement followingElement=mainDocument.createElement("FOLLOWING");
    followingElement.setAttribute("name",strFollowing);
    if ((strFollowing.isEmpty())
        || (strFollowing=="Current Settings")) // "Current Settings" is only a pseudo-style!
    {
        // We have no idea what style follows
        if (isStyle)
        {
            // We are a style, so we need a default
            followingElement.setAttribute("name","Normal");
            layoutElement.appendChild(followingElement);
        }
        // Else: we are a layout, so we leave the work to KWord (from the style)
    }
    else
    {
        // Following style is defined
        // TODO: we should be sure that this style is defined!
        layoutElement.appendChild(followingElement);
    }

    QString strFlow=abiPropsMap["text-align"].getValue();
    element=mainDocument.createElement("FLOW");
    if ((strFlow=="left") || (strFlow=="center") || (strFlow=="right") || (strFlow=="justify"))
    {
        element.setAttribute("align",strFlow);
    }
    else
    {
        element.setAttribute("align","left");
    }
    layoutElement.appendChild(element);

    int kwordDepth;
    int kwordNumberingType;
    int kwordType;
    QString kwordRightText;
    // level is 1 based like AbiWord, any value lower than 1 means no level!
    if ((level<=0) || (level>=15))
    {
        kwordDepth=0;
        kwordNumberingType=2;
        kwordType=0;
    }
    else
    {
        kwordDepth=level-1;
        kwordNumberingType=1;
        kwordType=1; // PROVISORY
        kwordRightText=".";
    }

    element=mainDocument.createElement("COUNTER");
    element.setAttribute("type",kwordType);
    element.setAttribute("depth",kwordDepth);
    element.setAttribute("start",1);
    element.setAttribute("numberingtype",kwordNumberingType);
    element.setAttribute("lefttext","");
    element.setAttribute("righttext",kwordRightText);
    element.setAttribute("bullet",64);
    element.setAttribute("bulletfont","Symbol");
    element.setAttribute("customdef","");
    layoutElement.appendChild(element);

    QString strLeftMargin=abiPropsMap["margin-left"].getValue();
    QString strRightMargin=abiPropsMap["margin-right"].getValue();
    QString strTextIndent=abiPropsMap["text-indent"].getValue();

    if ( !strLeftMargin.isEmpty()
        || !strRightMargin.isEmpty()
        || !strTextIndent.isEmpty() )
    {
        element=mainDocument.createElement("INDENTS");
        if (!strLeftMargin.isEmpty())
            element.setAttribute("left",ValueWithLengthUnit(strLeftMargin));
        if (!strRightMargin.isEmpty())
            element.setAttribute("right",ValueWithLengthUnit(strRightMargin));
        if (!strTextIndent.isEmpty())
            element.setAttribute("first",ValueWithLengthUnit(strTextIndent));
        layoutElement.appendChild(element);
    }

    QString strTopMargin=abiPropsMap["margin-top"].getValue();
    QString strBottomMargin=abiPropsMap["margin-bottom"].getValue();
    if (!strTopMargin.isEmpty() || !strBottomMargin.isEmpty() )
    {
        element=mainDocument.createElement("OFFSETS");
        const double margin_top=ValueWithLengthUnit(strTopMargin);
        const double margin_bottom=ValueWithLengthUnit(strBottomMargin);
        // Zero is propably a valid value!
        if (!strBottomMargin.isEmpty())
            element.setAttribute("after",margin_bottom);
        if (!strTopMargin.isEmpty())
            element.setAttribute("before",margin_top);
        layoutElement.appendChild(element);
    }

    QString strLineHeight=abiPropsMap["line-height"].getValue();
    if(!strLineHeight.isEmpty())
    {
        element=mainDocument.createElement("LINESPACING");
        double lineHeight;
        // Do we have a unit symbol or not?
        bool flag=false;
        lineHeight=strLineHeight.toDouble(&flag);

        if (flag)
        {
            if ( lineHeight == 1.0 )
            {
                element.setAttribute( "value", "single" );
                element.setAttribute( "type", "single" );
            }
            else if (lineHeight==1.5)
            {
                element.setAttribute( "value", "oneandhalf" );
                element.setAttribute( "type", "oneandhalf" );
            }
            else if (lineHeight==2.0)
            {
                element.setAttribute( "value", "double" );
                element.setAttribute( "type", "double" );
            }
            else if ( lineHeight > 0.0 )
            {
                element.setAttribute( "type", "multiple" );
                element.setAttribute( "spacingvalue", lineHeight );
            }
            else
            {
                kWarning(30506) << "Unsupported line height " << lineHeight << " (Ignoring !)" << endl;
            }
        }
        else
        {
            // Something went wrong, so we assume that an unit is specified
            bool atleast = false;
            lineHeight = ValueWithLengthUnit( strLineHeight, &atleast );
            if (lineHeight>1.0)
            {
                if ( atleast )
                {
                    kDebug(30506) << "at-least" << endl;
                    element.setAttribute( "type", "atleast" );
                }
                else
                {
                    element.setAttribute( "type", "exact" );
                }

                // We have a meaningful value, so use it!
                //element.setAttribute( "value", lineHeight );
                element.setAttribute( "spacingvalue", lineHeight );
            }
        }
        layoutElement.appendChild(element);
    }

    QString strTab=abiPropsMap["tabstops"].getValue();
    if(!strTab.isEmpty())
    {
        QStringList listTab=QStringList::split(",",strTab);
        for ( QStringList::Iterator it = listTab.begin(); it != listTab.end(); ++it )
        {
            QStringList tab=QStringList::split("/",*it);
            const QChar tabType=tab[1].at(0);
            const QChar tabFilling=tab[1].at(1); // Might be empty in old AbiWord files
            int type;
            if (tabType=='L') // left
                type=0;
            else if (tabType=='C') // center
                type=1;
            else if (tabType=='R') // right
                type=2;
            else if(tabType=='D') // decimal
                type=3;
            else if(tabType=='B') // bar (unsupported by KWord)
                type=0;
            else
            {
                kWarning(30506)<<"Unknown tabulator type: " << QString(tabType) << endl;
                type=0;
            }
            int filling;
            int width=72; // Any non-null value
            if (tabFilling.isNull() || tabFilling=='0') // No filling
                filling=0;
            else if (tabFilling=='1') // dot
            {
                filling=1;
                width=2;    // TODO: which width?
            }
            else if (tabFilling=='3') // underline
                filling=2;
            else
                filling=0;
            element=mainDocument.createElement("TABULATOR");
            element.setAttribute("ptpos",ValueWithLengthUnit(tab[0]));
            element.setAttribute("type",type);
            element.setAttribute("filling",filling);
            element.setAttribute("width",width);
            layoutElement.appendChild(element);
        }
    }

    QDomElement formatElementOut=mainDocument.createElement("FORMAT");
    layoutElement.appendChild(formatElementOut);

    AddFormat(formatElementOut, stackItem, mainDocument);
}

void AddStyle(QDomElement& styleElement, const QString& strStyleName,
    const StyleData& styleData, QDomDocument& mainDocument)
{
    // NOTE; styleElement is <STYLE> (singular), not <STYLES> (plural)

    StackItem stackItem;
    QXmlAttributes attributes; // This is just a dummy for reusing already existing functions (TODO)
    AbiPropsMap abiPropsMap;

    PopulateProperties(&stackItem, styleData.m_props, attributes, abiPropsMap, false);
    AddLayout(strStyleName, styleElement, &stackItem, mainDocument, abiPropsMap, styleData.m_level, true);
}
