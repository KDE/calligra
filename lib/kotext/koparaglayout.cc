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

#include "koparaglayout.h"
#include "koparagcounter.h"
#include "kostyle.h"
#include <qdom.h>
#include <kdebug.h>

// Create a default KoParagLayout.
KoParagLayout::KoParagLayout()
{
    initialise();
}

void KoParagLayout::operator=( const KoParagLayout &layout )
{
    alignment = layout.alignment;
    for ( int i = 0 ; i < 5 ; ++i )
        margins[i] = layout.margins[i];
    pageBreaking = layout.pageBreaking;
    leftBorder = layout.leftBorder;
    rightBorder = layout.rightBorder;
    topBorder = layout.topBorder;
    bottomBorder = layout.bottomBorder;
    if ( layout.counter )
        counter = new KoParagCounter( *layout.counter );
    else
        counter = 0L;
    lineSpacing = layout.lineSpacing;
    lineSpacingType = layout.lineSpacingType;
    style = layout.style;
    shadowDistance = layout.shadowDistance;
    shadowColor = layout.shadowColor;
    shadowDirection = layout.shadowDirection;
    direction = layout.direction;
    setTabList( layout.tabList() );
}

int KoParagLayout::compare( const KoParagLayout & layout ) const
{
    int flags = 0;
    if ( alignment != layout.alignment )
        flags |= Alignment;
    for ( int i = 0 ; i < 5 ; ++i )
        if ( margins[i] != layout.margins[i] )
        {
            flags |= Margins;
            break;
        }
    if ( pageBreaking != layout.pageBreaking )
        flags |= PageBreaking;
    if ( leftBorder != layout.leftBorder
         || rightBorder != layout.rightBorder
         || topBorder != layout.topBorder
         || bottomBorder != layout.bottomBorder )
        flags |= Borders;

    if ( layout.counter )
    {
        if ( counter )
        {
            if ( ! ( *layout.counter == *counter ) )
                flags |= BulletNumber;
        } else
            if ( layout.counter->numbering() != KoParagCounter::NUM_NONE )
                flags |= BulletNumber;
    }
    else
        if ( counter && counter->numbering() != KoParagCounter::NUM_NONE )
            flags |= BulletNumber;

    if ( lineSpacing != layout.lineSpacing
        || lineSpacingType != layout.lineSpacingType )
        flags |= LineSpacing;
    //if ( style != layout.style )
    //    flags |= Style;
    if ( m_tabList != layout.m_tabList )
        flags |= Tabulator;
    if ( shadowDistance != layout.shadowDistance
         || ( shadowDistance > 0 && shadowColor != layout.shadowColor )
         || ( shadowDistance > 0 && shadowDirection != layout.shadowDirection ) )
         flags |= Shadow;

    // This method is used for the GUI stuff only, so we don't have a flag
    // for the Direction value.
    return flags;
}

void KoParagLayout::initialise()
{
    alignment = Qt::AlignAuto;
    for ( int i = 0 ; i < 5 ; ++i ) // use memset ?
        margins[i] = 0;
    lineSpacingType = LS_SINGLE;
    lineSpacing = 0;
    counter = 0L;
    leftBorder.setPenWidth( 0);
    rightBorder.setPenWidth( 0);
    topBorder.setPenWidth( 0);
    bottomBorder.setPenWidth( 0);
    pageBreaking = 0;
    style = 0L;
    shadowDistance = 0;
    shadowColor = QColor();
    shadowDirection = SD_RIGHT_BOTTOM;
    direction = QChar::DirON;
    m_tabList.clear();
}

KoParagLayout::~KoParagLayout()
{
    delete counter;
}

void KoParagLayout::loadParagLayout( KoParagLayout& layout, const QDomElement& parentElem, int docVersion )
{
    // layout is an input and output parameter
    // It can have been initialized already, e.g. by copying from a style
    // (we don't do that anymore though).

    // Load the paragraph tabs - we load into a clean list, not mixing with those already in "layout"
    // We can't apply the 'default comes from the style' in this case, because
    // there is no way to differenciate between "I want no tabs in the parag"
    // and "use default from style".
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
    qHeapSort( tabList );
    layout.setTabList( tabList );
    layout.alignment = Qt::AlignAuto;
    element = parentElem.namedItem( "FLOW" ).toElement(); // Flow is what is now called alignment internally
    if ( !element.isNull() )
    {
        QString flow = element.attribute( "align" ); // KWord-1.0 DTD
        if ( !flow.isEmpty() )
        {
            layout.alignment = flow=="right" ? Qt::AlignRight :
                         flow=="center" ? Qt::AlignCenter :
                         flow=="justify" ? Qt::AlignJustify :
                         flow=="left" ? Qt::AlignLeft : Qt::AlignAuto;

            QString dir = element.attribute( "dir" ); // KWord-1.2
            if ( !dir.isEmpty() ) {
                if ( dir == "L" )
                    layout.direction = QChar::DirL;
                else if ( dir == "R" )
                    layout.direction = QChar::DirR;
                else
                    kdWarning() << "Unexpected value for paragraph direction: " << dir << endl;
            }
        } else {
            flow = element.attribute( "value" ); // KWord-0.8
            static const int flow2align[] = { Qt::AlignAuto, Qt::AlignRight, Qt::AlignCenter, Qt::AlignJustify };
            layout.alignment = flow2align[flow.toInt()];
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
        //compatibility with koffice 1.1
        if ( element.hasAttribute( "value" ))
        {
            QString value = element.attribute( "value" );
            if ( value == "oneandhalf" )
            {
                layout.lineSpacingType = KoParagLayout::LS_ONEANDHALF;
                layout.lineSpacing = 0;
            }
            else if ( value == "double" )
            {
                layout.lineSpacingType = KoParagLayout::LS_DOUBLE;
                layout.lineSpacing = 0;
            }
            else
            {
                layout.lineSpacingType = KoParagLayout::LS_CUSTOM;
                layout.lineSpacing = value.toDouble();
            }
        }
        else
        {
            QString type = element.attribute( "type" );
            if ( type == "oneandhalf" )
            {
                layout.lineSpacingType = KoParagLayout::LS_ONEANDHALF;
                layout.lineSpacing = 0;
            }
            else if ( type == "double" )
            {
                layout.lineSpacingType = KoParagLayout::LS_DOUBLE;
                layout.lineSpacing = 0;
            }
            else if ( type == "custom" )
            {
                layout.lineSpacingType = KoParagLayout::LS_CUSTOM;
                layout.lineSpacing = element.attribute( "spacingvalue" ).toDouble();
            }
            else if ( type == "atleast" )
            {
                layout.lineSpacingType = KoParagLayout::LS_AT_LEAST;
                layout.lineSpacing = element.attribute( "spacingvalue" ).toDouble();
            }
            else if ( type == "exactly" )
            {
                layout.lineSpacingType = KoParagLayout::LS_EXACTLY;
                layout.lineSpacing = element.attribute( "spacingvalue" ).toDouble();
            }
            else if ( type == "multiple" )
            {
                layout.lineSpacingType = KoParagLayout::LS_MULTIPLE;
                layout.lineSpacing = element.attribute( "spacingvalue" ).toDouble();
            }
        }
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
        layout.leftBorder.setPenWidth(0);

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
}

void KoParagLayout::saveParagLayout( QDomElement & parentElem, int alignment ) const
{
    const KoParagLayout& layout = *this; // code moved from somewhere else;)
    QDomDocument doc = parentElem.ownerDocument();
    QDomElement element = doc.createElement( "NAME" );
    parentElem.appendChild( element );
    if ( layout.style )
        element.setAttribute( "value", layout.style->name() );
    //else
    //    kdWarning() << "KoParagLayout::saveParagLayout: style==0L!" << endl;

    element = doc.createElement( "FLOW" );
    parentElem.appendChild( element );

    element.setAttribute( "align", alignment==Qt::AlignRight ? "right" :
                          alignment==Qt::AlignCenter ? "center" :
                          alignment==Qt::AlignJustify ? "justify" :
                          alignment==Qt::AlignAuto ? "auto" : "left" ); // Note: styles can have AlignAuto. Not paragraphs.

    if ( static_cast<QChar::Direction>(layout.direction) == QChar::DirR )
        element.setAttribute( "dir", "R" );

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
    if ( layout.lineSpacingType != LS_SINGLE )
    {
        element = doc.createElement( "LINESPACING" );
        parentElem.appendChild( element );
        if ( layout.lineSpacingType == KoParagLayout::LS_ONEANDHALF )
            element.setAttribute( "type", "oneandhalf" );
        else if ( layout.lineSpacingType == KoParagLayout::LS_DOUBLE )
            element.setAttribute( "type", "double" );
        else if ( layout.lineSpacingType == KoParagLayout::LS_CUSTOM )
        {
            element.setAttribute( "type", "custom" );
            element.setAttribute( "spacingvalue", layout.lineSpacing);
        }
        else if ( layout.lineSpacingType == KoParagLayout::LS_AT_LEAST )
        {
            element.setAttribute( "type", "atleast" );
            element.setAttribute( "spacingvalue", layout.lineSpacing);
        }
        else if ( layout.lineSpacingType == KoParagLayout::LS_EXACTLY )
        {
            element.setAttribute( "type", "exactly" );
            element.setAttribute( "spacingvalue", layout.lineSpacing);

        }
        else if ( layout.lineSpacingType == KoParagLayout::LS_MULTIPLE )
        {
            element.setAttribute( "type", "multiple" );
            element.setAttribute( "spacingvalue", layout.lineSpacing);
        }
        else
            kdDebug()<<" error in lineSpacing Type\n";
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

    if ( layout.leftBorder.penWidth() > 0 )
    {
        element = doc.createElement( "LEFTBORDER" );
        parentElem.appendChild( element );
        layout.leftBorder.save( element );
    }
    if ( layout.rightBorder.penWidth() > 0 )
    {
        element = doc.createElement( "RIGHTBORDER" );
        parentElem.appendChild( element );
        layout.rightBorder.save( element );
    }
    if ( layout.topBorder.penWidth() > 0 )
    {
        element = doc.createElement( "TOPBORDER" );
        parentElem.appendChild( element );
        layout.topBorder.save( element );
    }
    if ( layout.bottomBorder.penWidth() > 0 )
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
    KoTabulatorList::ConstIterator it = tabList.begin();
    for ( ; it != tabList.end() ; it++ )
    {
        element = doc.createElement( "TABULATOR" );
        parentElem.appendChild( element );
        element.setAttribute( "type", (*it).type );
        element.setAttribute( "ptpos", (*it).ptPos );
        element.setAttribute( "filling", (*it).filling );
        element.setAttribute( "width", (*it).ptWidth );
    }
    if(layout.shadowDistance!=0)
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
}
