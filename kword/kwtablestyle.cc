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

#include "kwtablestyle.h"
#include "kwdoc.h"
#include "kwframe.h"

#include <kdebug.h>
#include <klocale.h>
#include <qdom.h>


/******************************************************************/
/* Class: KWTableStyleCollection                                  */
/******************************************************************/
//necessary to create unique shortcut
int KWTableStyleCollection::styleNumber = 0;

KWTableStyleCollection::KWTableStyleCollection()
{
    m_styleList.setAutoDelete( false );
    m_deletedStyles.setAutoDelete( true );
    m_lastStyle = 0L;
}

KWTableStyleCollection::~KWTableStyleCollection()
{
    m_styleList.setAutoDelete( true );
    m_styleList.clear();
    m_deletedStyles.clear();
}

KWTableStyle* KWTableStyleCollection::findTableStyle( const QString & _name )
{
    // Caching, to speed things up
    if ( m_lastStyle && m_lastStyle->name() == _name )
        return m_lastStyle;

    QPtrListIterator<KWTableStyle> styleIt( m_styleList );
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

KWTableStyle* KWTableStyleCollection::findStyleShortCut( const QString & _shortCut )
{
    // Caching, to speed things up
    if ( m_lastStyle && m_lastStyle->shortCutName() == _shortCut )
        return m_lastStyle;

    QPtrListIterator<KWTableStyle> styleIt( m_styleList );
    for ( ; styleIt.current(); ++styleIt )
    {
        if ( styleIt.current()->shortCutName() == _shortCut ) {
            m_lastStyle = styleIt.current();
            return m_lastStyle;
        }
    }
    return 0L;
}

KWTableStyle* KWTableStyleCollection::addTableStyleTemplate( KWTableStyle * sty )
{
    // First check for duplicates.
    for ( KWTableStyle* p = m_styleList.first(); p != 0L; p = m_styleList.next() )
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
    sty->setShortCutName( QString("shortcut_tablestyle_%1").arg(styleNumber).latin1());
    styleNumber++;
    return sty;
}

void KWTableStyleCollection::removeTableStyleTemplate ( KWTableStyle *style ) {
    if( m_styleList.removeRef(style)) {
        if ( m_lastStyle == style )
            m_lastStyle = 0L;
        // Remember to delete this style when deleting the document
        m_deletedStyles.append(style);
    }
}

void KWTableStyleCollection::updateTableStyleListOrder( const QStringList &list )
{
    QPtrList<KWTableStyle> orderStyle;
    QStringList lst( list );
    for ( QStringList::Iterator it = lst.begin(); it != lst.end(); ++it )
    {
        //kdDebug()<<" style :"<<(*it)<<endl;
        QPtrListIterator<KWTableStyle> style( m_styleList );
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
/* Class: KWTableStyle                                            */
/******************************************************************/

KWTableStyle::KWTableStyle( const QString & name, KWStyle * _style, KWFrameStyle * _frameStyle )
{
    m_name = name;
    m_shortCut_name = QString::null;
    m_style = _style;
    m_frameStyle = _frameStyle;
}

KWTableStyle::KWTableStyle( QDomElement & parentElem, KWDocument *_doc, int /*docVersion*/ )
{
    QDomElement element = parentElem.namedItem( "NAME" ).toElement();
    if ( ( !element.isNull() ) && ( element.hasAttribute("value") ) )
        m_name = element.attribute( "value" );

    element = parentElem.namedItem( "PFRAMESTYLE" ).toElement();
    if ( ( !element.isNull() ) && ( element.hasAttribute("name") )
         && ( _doc->frameStyleCollection()->findFrameStyle( element.attribute( "name" ) ) ) )
        m_frameStyle = _doc->frameStyleCollection()->findFrameStyle( element.attribute( "name" ) );
    else {
        if ( _doc->frameStyleCollection()->frameStyleList().count()>0 )
            m_frameStyle = _doc->frameStyleCollection()->frameStyleAt( 0 );
        else { // Isn't possible ( I hope )
            KWFrameStyle * standardFrameStyle = new KWFrameStyle( "Plain" );
            standardFrameStyle->setBackgroundColor(QColor("white"));
            standardFrameStyle->setTopBorder(KoBorder(QColor("black"),KoBorder::SOLID,0));
            standardFrameStyle->setRightBorder(KoBorder(QColor("black"),KoBorder::SOLID,0));
            standardFrameStyle->setLeftBorder(KoBorder(QColor("black"),KoBorder::SOLID,0));
            standardFrameStyle->setBottomBorder(KoBorder(QColor("black"),KoBorder::SOLID,0));
            _doc->frameStyleCollection()->addFrameStyleTemplate( standardFrameStyle );
            m_frameStyle = _doc->frameStyleCollection()->frameStyleAt( 0 );
        }
    }

    element = parentElem.namedItem( "PSTYLE" ).toElement();
    if ( ( !element.isNull() ) && ( element.hasAttribute("name") )
         && ( _doc->styleCollection()->findStyle( element.attribute( "name" ) ) ) )
        m_style = _doc->styleCollection()->findStyle( element.attribute( "name" ) );
    else {
        if ( _doc->styleCollection()->styleList().count()>0 )
            m_style = _doc->styleCollection()->styleAt( 0 );
        else { // Isn't possible ( I hope )
            KWStyle * standardStyle = new KWStyle( "Standard" );
            standardStyle->format().setFont( _doc->defaultFont() );
            _doc->styleCollection()->addStyleTemplate( standardStyle );
            m_style = _doc->styleCollection()->styleAt( 0 );
        }
    }
}

void KWTableStyle::operator=( const KWTableStyle &rhs )
{
    m_name = rhs.m_name;
    m_shortCut_name = rhs.m_shortCut_name;
    m_style = rhs.pStyle();
    m_frameStyle = rhs.pFrameStyle();
}

QString KWTableStyle::displayName() const
{
    return i18n( "Style name", name().utf8() );
}

void KWTableStyle::saveTableStyle( QDomElement & parentElem )
{
    QDomDocument doc = parentElem.ownerDocument();
    QDomElement element = doc.createElement( "NAME" );
    parentElem.appendChild( element );
    element.setAttribute( "value", name() );

    if (m_frameStyle)
    {
        element = doc.createElement( "PFRAMESTYLE" );
        parentElem.appendChild( element );
        element.setAttribute( "name", m_frameStyle->name() );
    }
    if (m_style)
    {
        element = doc.createElement( "PSTYLE" );
        parentElem.appendChild( element );
        element.setAttribute( "name", m_style->name() );
    }

}

KWTableStyle *KWTableStyle::loadStyle( QDomElement & parentElem, KWDocument *_doc, int docVersion )
{
    return new KWTableStyle( parentElem, _doc, docVersion );
}
