/* This file is part of the KDE project
   Copyright (C) 2002 Nash Hoogwater <nrhoogwater@wanadoo.nl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; using
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kwframestyle.h"
#include "kwdoc.h"
#include "kwframe.h"

#include <kdebug.h>
#include <klocale.h>
#include <qdom.h>


/******************************************************************/
/* Class: KWFrameStyleCollection                                  */
/******************************************************************/
//necessary to create unique shortcut
int KWFrameStyleCollection::styleNumber = 0;

KWFrameStyleCollection::KWFrameStyleCollection()
{
    m_styleList.setAutoDelete( false );
    m_deletedStyles.setAutoDelete( true );
    m_lastStyle = 0L;
}

KWFrameStyleCollection::~KWFrameStyleCollection()
{
    m_styleList.setAutoDelete( true );
    m_styleList.clear();
    m_deletedStyles.clear();
}

KWFrameStyle* KWFrameStyleCollection::findStyleShortCut( const QString & _shortCut )
{
    // Caching, to speed things up
    if ( m_lastStyle && m_lastStyle->shortCutName() == _shortCut )
        return m_lastStyle;

    QPtrListIterator<KWFrameStyle> styleIt( m_styleList );
    for ( ; styleIt.current(); ++styleIt )
    {
        if ( styleIt.current()->shortCutName() == _shortCut ) {
            m_lastStyle = styleIt.current();
            return m_lastStyle;
        }
    }
    return 0L;
}


KWFrameStyle* KWFrameStyleCollection::findFrameStyle( const QString & _name )
{
    // Caching, to speed things up
    if ( m_lastStyle && m_lastStyle->name() == _name )
        return m_lastStyle;

    QPtrListIterator<KWFrameStyle> styleIt( m_styleList );
    for ( ; styleIt.current(); ++styleIt )
    {
        if ( styleIt.current()->name() == _name ) {
            m_lastStyle = styleIt.current();
            return m_lastStyle;
        }
    }

    if(_name == "Plain") return m_styleList.at(0); // fallback..

    return 0L;
}


KWFrameStyle* KWFrameStyleCollection::addFrameStyleTemplate( KWFrameStyle * sty )
{
    // First check for duplicates.
    for ( KWFrameStyle* p = m_styleList.first(); p != 0L; p = m_styleList.next() )
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
    sty->setShortCutName( QString("shortcut_framestyle_%1").arg(styleNumber).latin1());
    styleNumber++;
    return sty;
}

void KWFrameStyleCollection::removeFrameStyleTemplate ( KWFrameStyle *style ) {
    if( m_styleList.removeRef(style)) {
        if ( m_lastStyle == style )
            m_lastStyle = 0L;
        // Remember to delete this style when deleting the document
        m_deletedStyles.append(style);
    }
}

void KWFrameStyleCollection::updateFrameStyleListOrder( const QStringList &list )
{
    QPtrList<KWFrameStyle> orderStyle;
    QStringList lst( list );
    for ( QStringList::Iterator it = lst.begin(); it != lst.end(); ++it )
    {
        //kdDebug()<<" style :"<<(*it)<<endl;
        QPtrListIterator<KWFrameStyle> style( m_styleList );
        for ( ; style.current() ; ++style )
        {
            if ( style.current()->name() == *it)
            {
                orderStyle.append( style.current() );
                //kdDebug()<<" found !!!!!!!!!!!!\n";
                break;
            }
        }
    }
    m_styleList.setAutoDelete( false );
    m_styleList.clear();
    m_styleList = orderStyle;
#if 0
    QPtrListIterator<KoParagStyle> style( m_styleList );
    for ( ; style.current() ; ++style )
    {
        kdDebug()<<" style.current()->name() :"<<style.current()->name()<<endl;
    }
#endif
}


/******************************************************************/
/* Class: KWFrameStyle                                            */
/******************************************************************/

KWFrameStyle::KWFrameStyle( const QString & name )
{
    m_name = name;
    m_shortCut_name = QString::null;
    m_backgroundColor.setColor( Qt::white );
}

KWFrameStyle::KWFrameStyle( const QString & name, KWFrame * frame )
{
    m_name = name;
    m_backgroundColor = frame->backgroundColor();
    m_borderLeft = frame->leftBorder();
    m_borderRight = frame->rightBorder();
    m_borderTop = frame->topBorder();
    m_borderBottom = frame->bottomBorder();
}

KWFrameStyle::KWFrameStyle( QDomElement & parentElem, int /*docVersion=2*/ )
{
    QDomElement element = parentElem.namedItem( "NAME" ).toElement();
    if ( ( !element.isNull() ) && ( element.hasAttribute("value") ) )
        m_name = element.attribute( "value" );

    element = parentElem.namedItem( "LEFTBORDER" ).toElement();
    if ( !element.isNull() )
        m_borderLeft = KoBorder::loadBorder( element );
    else
        m_borderLeft.setPenWidth( 0 );

    element = parentElem.namedItem( "RIGHTBORDER" ).toElement();
    if ( !element.isNull() )
        m_borderRight = KoBorder::loadBorder( element );
    else
        m_borderRight.setPenWidth( 0 );

    element = parentElem.namedItem( "TOPBORDER" ).toElement();
    if ( !element.isNull() )
        m_borderTop = KoBorder::loadBorder( element );
    else
        m_borderTop.setPenWidth( 0 );

    element = parentElem.namedItem( "BOTTOMBORDER" ).toElement();
    if ( !element.isNull() )
        m_borderBottom = KoBorder::loadBorder( element );
    else
        m_borderBottom.setPenWidth( 0 );

    QColor c("white");
    if ( parentElem.hasAttribute("red") )
        c.setRgb(
            KWDocument::getAttribute( parentElem, "red", 0 ),
            KWDocument::getAttribute( parentElem, "green", 0 ),
            KWDocument::getAttribute( parentElem, "blue", 0 ) );

    m_backgroundColor = QBrush( c );
}


void KWFrameStyle::operator=( const KWFrameStyle &rhs )
{
    m_name = rhs.m_name;
    m_backgroundColor = rhs.m_backgroundColor;
    m_shortCut_name = rhs.m_shortCut_name;
    m_borderLeft = rhs.m_borderLeft;
    m_borderRight = rhs.m_borderRight;
    m_borderTop = rhs.m_borderTop;
    m_borderBottom = rhs.m_borderBottom;
}

QString KWFrameStyle::displayName() const
{
    return i18n( "Style name", name().utf8() );
}

int KWFrameStyle::compare( const KWFrameStyle & frameStyle ) const
{
    int flags = 0;
    if ( m_borderLeft != frameStyle.m_borderLeft
         || m_borderRight != frameStyle.m_borderRight
         || m_borderTop != frameStyle.m_borderTop
         || m_borderBottom != frameStyle.m_borderBottom )
        flags |= Borders;
    if ( m_backgroundColor.color() != frameStyle.m_backgroundColor.color() )
        flags |= Background;

    return flags;
}


void KWFrameStyle::saveFrameStyle( QDomElement & parentElem )
{
    QDomDocument doc = parentElem.ownerDocument();
    QDomElement element = doc.createElement( "NAME" );
    parentElem.appendChild( element );
    element.setAttribute( "value", name() );

    if ( m_borderLeft.width() > 0 )
    {
        element = doc.createElement( "LEFTBORDER" );
        parentElem.appendChild( element );
        m_borderLeft.save( element );
    }
    if ( m_borderRight.width() > 0 )
    {
        element = doc.createElement( "RIGHTBORDER" );
        parentElem.appendChild( element );
        m_borderRight.save( element );
    }
    if ( m_borderTop.width() > 0 )
    {
        element = doc.createElement( "TOPBORDER" );
        parentElem.appendChild( element );
        m_borderTop.save( element );
    }
    if ( m_borderBottom.width() > 0 )
    {
        element = doc.createElement( "BOTTOMBORDER" );
        parentElem.appendChild( element );
        m_borderBottom.save( element );
    }

    if(m_backgroundColor.color().isValid())
    {
        parentElem.setAttribute( "red", m_backgroundColor.color().red() );
        parentElem.setAttribute( "green", m_backgroundColor.color().green() );
        parentElem.setAttribute( "blue", m_backgroundColor.color().blue() );
    }
}

KWFrameStyle *KWFrameStyle::loadStyle( QDomElement & parentElem, int docVersion )
{
    return new KWFrameStyle( parentElem, docVersion );
}
