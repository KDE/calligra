/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>

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

#include "kostyle.h"
#include <kdebug.h>
#include <klocale.h>
#include <qdom.h>
#include <koparagcounter.h>

KoStyle::KoStyle( const QString & name )
{
    m_name = name;
    m_followingStyle = this;
    m_format.setColor( Qt::black ); // ### why ? what about other color schemes ?

    // This way, KWTextParag::setParagLayout also sets the style pointer, to this style
    m_paragLayout.style = this;
}

void KoStyle::operator=( const KoStyle &rhs )
{
    m_paragLayout = rhs.m_paragLayout;
    m_name = rhs.m_name;
    m_format = rhs.m_format;
    m_followingStyle = rhs.m_followingStyle;
    m_paragLayout.style = this; // must always be "this"
}

QString KoStyle::translatedName() const
{
    return i18n( "Style name", name().utf8() );
}

void KoStyle::saveStyle( const KoParagLayout& layout,KoStyle *sty, QDomElement & parentElem )
{
    QDomDocument doc = parentElem.ownerDocument();
    QDomElement element = doc.createElement( "NAME" );
    parentElem.appendChild( element );
    if ( layout.style )
        element.setAttribute( "value", layout.style->name() );
    else
        kdWarning() << "KoStyle::saveParagLayout: style==0L!" << endl;

    element = doc.createElement( "FLOW" );
    parentElem.appendChild( element );
    int a = layout.alignment;
    element.setAttribute( "align", a==Qt::AlignRight ? "right" : a==Qt::AlignCenter ? "center" : a==Qt::AlignJustify ? "justify" : "left" );

    if ( layout.margins[QStyleSheetItem::MarginFirstLine] != 0 ||
         layout.margins[QStyleSheetItem::MarginLeft] != 0 ||
         layout.margins[QStyleSheetItem::MarginRight] != 0 )
    {
        element = doc.createElement( "INDENTS" );
        parentElem.appendChild( element );
        if ( layout.margins[QStyleSheetItem::MarginFirstLine] != 0 )
            element.setAttribute( "first", layout.margins[QStyleSheetItem::MarginFirstLine] );
        if ( layout.margins[QStyleSheetItem::MarginLeft] != 0 )
            element.setAttribute( "left", layout.margins[QStyleSheetItem::MarginLeft] );
        if ( layout.margins[QStyleSheetItem::MarginRight] != 0 )
            element.setAttribute( "right", layout.margins[QStyleSheetItem::MarginRight] );
    }

    if ( layout.margins[QStyleSheetItem::MarginTop] != 0 ||
         layout.margins[QStyleSheetItem::MarginBottom] != 0 )
    {
        element = doc.createElement( "OFFSETS" );
        parentElem.appendChild( element );
        if ( layout.margins[QStyleSheetItem::MarginTop] != 0 )
            element.setAttribute( "before", layout.margins[QStyleSheetItem::MarginTop] );
        if ( layout.margins[QStyleSheetItem::MarginBottom] != 0 )
            element.setAttribute( "after", layout.margins[QStyleSheetItem::MarginBottom] );
    }

    if ( layout.lineSpacing != 0 )
    {
        element = doc.createElement( "LINESPACING" );
        parentElem.appendChild( element );
        if ( layout.lineSpacing == KoParagLayout::LS_ONEANDHALF )
            element.setAttribute( "value", "oneandhalf" );
        else if ( layout.lineSpacing == KoParagLayout::LS_DOUBLE )
            element.setAttribute( "value", "double" );
        else
            element.setAttribute( "value", layout.lineSpacing );
    }

    if ( layout.pageBreaking != 0 )
    {
        element = doc.createElement( "PAGEBREAKING" );
        parentElem.appendChild( element );
        if ( layout.pageBreaking & KoParagLayout::KeepLinesTogether )
            element.setAttribute( "linesTogether",  "true" );
        if ( layout.pageBreaking & KoParagLayout::HardFrameBreakBefore )
            element.setAttribute( "hardFrameBreak", "true" );
        if ( layout.pageBreaking & KoParagLayout::HardFrameBreakAfter )
            element.setAttribute( "hardFrameBreakAfter", "true" );
    }

    if ( layout.leftBorder.ptWidth > 0 )
    {
        element = doc.createElement( "LEFTBORDER" );
        parentElem.appendChild( element );
        layout.leftBorder.save( element );
    }
    if ( layout.rightBorder.ptWidth > 0 )
    {
        element = doc.createElement( "RIGHTBORDER" );
        parentElem.appendChild( element );
        layout.rightBorder.save( element );
    }
    if ( layout.topBorder.ptWidth > 0 )
    {
        element = doc.createElement( "TOPBORDER" );
        parentElem.appendChild( element );
        layout.topBorder.save( element );
    }
    if ( layout.bottomBorder.ptWidth > 0 )
    {
        element = doc.createElement( "BOTTOMBORDER" );
        parentElem.appendChild( element );
        layout.bottomBorder.save( element );
    }
    if ( layout.counter && layout.counter->numbering() != KoParagCounter::NUM_NONE )
    {
        element = doc.createElement( "COUNTER" );
        parentElem.appendChild( element );
        if ( layout.counter )
            layout.counter->save( element );
    }

    KoTabulatorList tabList = layout.tabList();
    KoTabulatorList::Iterator it = tabList.begin();
    for ( ; it != tabList.end() ; it++ )
    {
        element = doc.createElement( "TABULATOR" );
        parentElem.appendChild( element );
        element.setAttribute( "type", (*it).type );
        element.setAttribute( "ptpos", (*it).ptPos );
    }
    if(layout.shadowDistance!=0 || layout.shadowDirection!=KoParagLayout::SD_RIGHT_BOTTOM)
    {
        element = doc.createElement( "SHADOW" );
        parentElem.appendChild( element );
        element.setAttribute( "distance", layout.shadowDistance );
        element.setAttribute( "direction", layout.shadowDirection );
        if (layout.shadowColor.isValid())
        {
            element.setAttribute("red", layout.shadowColor.red());
            element.setAttribute("green", layout.shadowColor.green());
            element.setAttribute("blue", layout.shadowColor.blue());
        }
    }

    if ( sty->followingStyle() )
    {
        QDomElement element = doc.createElement( "FOLLOWING" );
        parentElem.appendChild( element );
        element.setAttribute( "name", sty->followingStyle()->name() );
    }

}

// Create a KoParagLayout from XML.
KoParagLayout KoStyle::loadStyle( QDomElement & parentElem, int docVersion )
{
    KoParagLayout layout;

    // Load the paragraph tabs - forget about the ones from the style first.
    // We can't apply the 'default comes from the style' in this case, because
    // there is no way to differenciate between "I want no tabs in the parag"
    // and "use default from style".
    //m_tabList.clear();
    KoTabulatorList tabList;
    QDomElement element = parentElem.firstChild().toElement();
    for ( ; !element.isNull() ; element = element.nextSibling().toElement() )
    {
        if ( element.tagName() == "TABULATOR" )
        {
            KoTabulator tab;
            tab.type = static_cast<KoTabulators>( KoStyle::getAttribute( element, "type", T_LEFT ) );
            tab.ptPos = KoStyle::getAttribute( element, "ptpos", 0.0 );
            tabList.append( tab );
        }
    }
    layout.setTabList( tabList );
    layout.alignment = Qt::AlignLeft;
    element = parentElem.namedItem( "FLOW" ).toElement(); // Flow is what is now called alignment internally
    if ( !element.isNull() )
    {
        QString flow = element.attribute( "value" ); // KWord-0.8
        if ( !flow.isEmpty() )
        {
            static const int flow2align[] = { Qt::AlignLeft, Qt::AlignRight, Qt::AlignCenter, Qt::AlignJustify };
            layout.alignment = flow2align[flow.toInt()];
        } else {
            flow = element.attribute( "align" ); // KWord-1.0 DTD
            layout.alignment = flow=="right" ? (int)Qt::AlignRight : flow=="center" ? (int)Qt::AlignCenter : flow=="justify" ? (int)Qt::AlignJustify : (int)Qt::AlignLeft;
        }
    }

    if ( docVersion < 2 )
    {
        element = parentElem.namedItem( "OHEAD" ).toElement(); // used by KWord-0.8
        if ( !element.isNull() )
            layout.margins[QStyleSheetItem::MarginTop] = KoStyle::getAttribute( element, "pt", 0.0 );

        element = parentElem.namedItem( "OFOOT" ).toElement(); // used by KWord-0.8
        if ( !element.isNull() )
            layout.margins[QStyleSheetItem::MarginBottom] = KoStyle::getAttribute( element, "pt", 0.0 );

        element = parentElem.namedItem( "IFIRST" ).toElement(); // used by KWord-0.8
        if ( !element.isNull() )
            layout.margins[QStyleSheetItem::MarginFirstLine] = KoStyle::getAttribute( element, "pt", 0.0 );

        element = parentElem.namedItem( "ILEFT" ).toElement(); // used by KWord-0.8
        if ( !element.isNull() )
            layout.margins[QStyleSheetItem::MarginLeft] = KoStyle::getAttribute( element, "pt", 0.0 );
    }

    // KWord-1.0 DTD
    element = parentElem.namedItem( "INDENTS" ).toElement();
    if ( !element.isNull() )
    {
        layout.margins[QStyleSheetItem::MarginFirstLine] = KoStyle::getAttribute( element, "first", 0.0 );
        layout.margins[QStyleSheetItem::MarginLeft] = KoStyle::getAttribute( element, "left", 0.0 );
        layout.margins[QStyleSheetItem::MarginRight] = KoStyle::getAttribute( element, "right", 0.0 );
    }
    element = parentElem.namedItem( "OFFSETS" ).toElement();
    if ( !element.isNull() )
    {
        layout.margins[QStyleSheetItem::MarginTop] = KoStyle::getAttribute( element, "before", 0.0 );
        layout.margins[QStyleSheetItem::MarginBottom] = KoStyle::getAttribute( element, "after", 0.0 );
    }

    if ( docVersion < 2 )
    {
        element = parentElem.namedItem( "LINESPACE" ).toElement(); // used by KWord-0.8
        if ( !element.isNull() )
            layout.lineSpacing = KoStyle::getAttribute( element, "pt", 0.0 );
    }

    element = parentElem.namedItem( "LINESPACING" ).toElement(); // KWord-1.0 DTD
    if ( !element.isNull() )
    {
        QString value = element.attribute( "value" );
        if ( value == "oneandhalf" )
            layout.lineSpacing = KoParagLayout::LS_ONEANDHALF;
        else if ( value == "double" )
            layout.lineSpacing = KoParagLayout::LS_DOUBLE;
        else
            layout.lineSpacing = value.toDouble();
    }

    int pageBreaking = 0;
    element = parentElem.namedItem( "PAGEBREAKING" ).toElement();
    if ( !element.isNull() )
    {
        if ( element.attribute( "linesTogether" ) == "true" )
            pageBreaking |= KoParagLayout::KeepLinesTogether;
        if ( element.attribute( "hardFrameBreak" ) == "true" )
            pageBreaking |= KoParagLayout::HardFrameBreakBefore;
        if ( element.attribute( "hardFrameBreakAfter" ) == "true" )
            pageBreaking |= KoParagLayout::HardFrameBreakAfter;
    }
    if ( docVersion < 2 )
    {
        element = parentElem.namedItem( "HARDBRK" ).toElement(); // KWord-0.8
        if ( !element.isNull() )
            pageBreaking |= KoParagLayout::HardFrameBreakBefore;
    }
    layout.pageBreaking = pageBreaking;

    element = parentElem.namedItem( "LEFTBORDER" ).toElement();
    if ( !element.isNull() )
        layout.leftBorder = KoBorder::loadBorder( element );
    else
        layout.leftBorder.ptWidth = 0;

    element = parentElem.namedItem( "RIGHTBORDER" ).toElement();
    if ( !element.isNull() )
        layout.rightBorder = KoBorder::loadBorder( element );
    else
        layout.rightBorder.ptWidth = 0;

    element = parentElem.namedItem( "TOPBORDER" ).toElement();
    if ( !element.isNull() )
        layout.topBorder = KoBorder::loadBorder( element );
    else
        layout.topBorder.ptWidth = 0;

    element = parentElem.namedItem( "BOTTOMBORDER" ).toElement();
    if ( !element.isNull() )
        layout.bottomBorder = KoBorder::loadBorder( element );
    else
        layout.bottomBorder.ptWidth = 0;

    element = parentElem.namedItem( "COUNTER" ).toElement();
    if ( !element.isNull() )
    {
        layout.counter = new KoParagCounter;
        layout.counter->load( element );
    }

    element = parentElem.namedItem( "SHADOW" ).toElement();
    if ( !element.isNull() )
    {
        layout.shadowDistance=element.attribute("distance").toInt();
        layout.shadowDirection=element.attribute("direction").toInt();
        if ( element.hasAttribute("red") )
        {
            int r = element.attribute("red").toInt();
            int g = element.attribute("green").toInt();
            int b = element.attribute("blue").toInt();
            layout.shadowColor.setRgb( r, g, b );
        }
    }

    return layout;
}
