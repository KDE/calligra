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
    m_styleList.setAutoDelete( true );
    m_styleList.clear();
    m_deletedStyles.clear();
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
    m_paragLayout.saveParagLayout( parentElem, m_paragLayout.alignment );

    if ( followingStyle() )
    {
        QDomElement element = parentElem.ownerDocument().createElement( "FOLLOWING" );
        parentElem.appendChild( element );
        element.setAttribute( "name", followingStyle()->name() );
    }
}

// Create a KoParagLayout from XML.
KoParagLayout KoStyle::loadStyle( QDomElement & parentElem, int docVersion )
{
    KoParagLayout layout;

    KoParagLayout::loadParagLayout( layout, parentElem, docVersion );

    // The followingStyle stuff is done by KWDocument after loading all styles.

    return layout;
}
