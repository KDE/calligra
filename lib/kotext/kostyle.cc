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


KoStyleCollection::KoStyleCollection()
{
    m_styleList.setAutoDelete( false );
    m_deletedStyles.setAutoDelete( true );
    m_lastStyle = 0L;
}

KoStyleCollection::~KoStyleCollection()
{
    //todo
}

KoStyle* KoStyleCollection::findStyle( const QString & _name )
{
    // Caching, to speed things up
    if ( m_lastStyle && m_lastStyle->name() == _name )
        return m_lastStyle;

    QPtrListIterator<KoStyle> styleIt( m_styleList );
    for ( ; styleIt.current(); ++styleIt )
    {
        if ( styleIt.current()->name() == _name ) {
            m_lastStyle = styleIt.current();
            return m_lastStyle;
        }
    }

    if(_name == "Standard") return m_styleList.at(0); // fallback..

    return 0L;
}


KoStyle* KoStyleCollection::addStyleTemplate( KoStyle * sty )
{
    // First check for duplicates.
    for ( KoStyle* p = m_styleList.first(); p != 0L; p = m_styleList.next() )
    {
        if ( p->name() == sty->name() ) {
            // Replace existing style
            if ( sty != p )
            {
                *p = *sty;
                delete sty;
            }
            return p;
        }
    }
    m_styleList.append( sty );
    return sty;
}

void KoStyleCollection::removeStyleTemplate ( KoStyle *style ) {
    if( m_styleList.removeRef(style)) {
        if ( m_lastStyle == style )
            m_lastStyle = 0L;
        // Remember to delete this style when deleting the document
        m_deletedStyles.append(style);
    }
}

KoStyle::KoStyle( const QString & name )
{
    m_name = name;
    m_followingStyle = this;

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

void KoStyle::saveStyle( QDomElement & parentElem )
{
    QDomDocument doc = parentElem.ownerDocument();
    QDomElement element = doc.createElement( "NAME" );
    parentElem.appendChild( element );
    element.setAttribute( "value", name() );

    element = doc.createElement( "FLOW" );
    parentElem.appendChild( element );
    int a = m_paragLayout.alignment;
    element.setAttribute( "align", a==Qt::AlignRight ? "right" : a==Qt::AlignCenter ? "center" : a==Qt::AlignJustify ? "justify" : "left" );

    if ( m_paragLayout.margins[QStyleSheetItem::MarginFirstLine] != 0 ||
         m_paragLayout.margins[QStyleSheetItem::MarginLeft] != 0 ||
         m_paragLayout.margins[QStyleSheetItem::MarginRight] != 0 )
    {
        element = doc.createElement( "INDENTS" );
        parentElem.appendChild( element );
        if ( m_paragLayout.margins[QStyleSheetItem::MarginFirstLine] != 0 )
            element.setAttribute( "first", m_paragLayout.margins[QStyleSheetItem::MarginFirstLine] );
        if ( m_paragLayout.margins[QStyleSheetItem::MarginLeft] != 0 )
            element.setAttribute( "left", m_paragLayout.margins[QStyleSheetItem::MarginLeft] );
        if ( m_paragLayout.margins[QStyleSheetItem::MarginRight] != 0 )
            element.setAttribute( "right", m_paragLayout.margins[QStyleSheetItem::MarginRight] );
    }

    if ( m_paragLayout.margins[QStyleSheetItem::MarginTop] != 0 ||
         m_paragLayout.margins[QStyleSheetItem::MarginBottom] != 0 )
    {
        element = doc.createElement( "OFFSETS" );
        parentElem.appendChild( element );
        if ( m_paragLayout.margins[QStyleSheetItem::MarginTop] != 0 )
            element.setAttribute( "before", m_paragLayout.margins[QStyleSheetItem::MarginTop] );
        if ( m_paragLayout.margins[QStyleSheetItem::MarginBottom] != 0 )
            element.setAttribute( "after", m_paragLayout.margins[QStyleSheetItem::MarginBottom] );
    }

    if ( m_paragLayout.lineSpacing != 0 )
    {
        element = doc.createElement( "LINESPACING" );
        parentElem.appendChild( element );
        if ( m_paragLayout.lineSpacing == KoParagLayout::LS_ONEANDHALF )
            element.setAttribute( "value", "oneandhalf" );
        else if ( m_paragLayout.lineSpacing == KoParagLayout::LS_DOUBLE )
            element.setAttribute( "value", "double" );
        else
            element.setAttribute( "value", m_paragLayout.lineSpacing );
    }

    if ( m_paragLayout.pageBreaking != 0 )
    {
        element = doc.createElement( "PAGEBREAKING" );
        parentElem.appendChild( element );
        if ( m_paragLayout.pageBreaking & KoParagLayout::KeepLinesTogether )
            element.setAttribute( "linesTogether",  "true" );
        if ( m_paragLayout.pageBreaking & KoParagLayout::HardFrameBreakBefore )
            element.setAttribute( "hardFrameBreak", "true" );
        if ( m_paragLayout.pageBreaking & KoParagLayout::HardFrameBreakAfter )
            element.setAttribute( "hardFrameBreakAfter", "true" );
    }

    if ( m_paragLayout.leftBorder.penWidth() > 0 )
    {
        element = doc.createElement( "LEFTBORDER" );
        parentElem.appendChild( element );
        m_paragLayout.leftBorder.save( element );
    }
    if ( m_paragLayout.rightBorder.penWidth() > 0 )
    {
        element = doc.createElement( "RIGHTBORDER" );
        parentElem.appendChild( element );
        m_paragLayout.rightBorder.save( element );
    }
    if ( m_paragLayout.topBorder.penWidth() > 0 )
    {
        element = doc.createElement( "TOPBORDER" );
        parentElem.appendChild( element );
        m_paragLayout.topBorder.save( element );
    }
    if ( m_paragLayout.bottomBorder.penWidth() > 0 )
    {
        element = doc.createElement( "BOTTOMBORDER" );
        parentElem.appendChild( element );
        m_paragLayout.bottomBorder.save( element );
    }
    if ( m_paragLayout.counter && m_paragLayout.counter->numbering() != KoParagCounter::NUM_NONE )
    {
        element = doc.createElement( "COUNTER" );
        parentElem.appendChild( element );
        if ( m_paragLayout.counter )
            m_paragLayout.counter->save( element );
    }

    KoTabulatorList tabList = m_paragLayout.tabList();
    KoTabulatorList::Iterator it = tabList.begin();
    for ( ; it != tabList.end() ; it++ )
    {
        element = doc.createElement( "TABULATOR" );
        parentElem.appendChild( element );
        element.setAttribute( "type", (*it).type );
        element.setAttribute( "ptpos", (*it).ptPos );
        element.setAttribute( "filling", (*it).filling );
        element.setAttribute( "width", (*it).ptWidth );
    }
    if(m_paragLayout.shadowDistance!=0 || m_paragLayout.shadowDirection!=KoParagLayout::SD_RIGHT_BOTTOM)
    {
        element = doc.createElement( "SHADOW" );
        parentElem.appendChild( element );
        element.setAttribute( "distance", m_paragLayout.shadowDistance );
        element.setAttribute( "direction", m_paragLayout.shadowDirection );
        if (m_paragLayout.shadowColor.isValid())
        {
            element.setAttribute("red", m_paragLayout.shadowColor.red());
            element.setAttribute("green", m_paragLayout.shadowColor.green());
            element.setAttribute("blue", m_paragLayout.shadowColor.blue());
        }
    }

    if ( followingStyle() )
    {
        QDomElement element = doc.createElement( "FOLLOWING" );
        parentElem.appendChild( element );
        element.setAttribute( "name", followingStyle()->name() );
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
            tab.filling = static_cast<KoTabulatorFilling>( KoStyle::getAttribute( element, "filling", TF_BLANK ) );
            tab.ptWidth = KoStyle::getAttribute( element, "width", 0.5 );
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
        layout.leftBorder.setPenWidth (0);

    element = parentElem.namedItem( "RIGHTBORDER" ).toElement();
    if ( !element.isNull() )
        layout.rightBorder = KoBorder::loadBorder( element );
    else
        layout.rightBorder.setPenWidth(0);

    element = parentElem.namedItem( "TOPBORDER" ).toElement();
    if ( !element.isNull() )
        layout.topBorder = KoBorder::loadBorder( element );
    else
        layout.topBorder.setPenWidth(0);

    element = parentElem.namedItem( "BOTTOMBORDER" ).toElement();
    if ( !element.isNull() )
        layout.bottomBorder = KoBorder::loadBorder( element );
    else
        layout.bottomBorder.setPenWidth(0);

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
