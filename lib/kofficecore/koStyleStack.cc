/* This file is part of the KDE project
   Copyright (c) 2003 Lukas Tinkl <lukas@kde.org>
   Copyright (c) 2003 David Faure <faure@kde.org>

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

#include "koStyleStack.h"
#include <koUnit.h>
#include <kdebug.h>
#include "koxmlns.h"

//#define DEBUG_STYLESTACK

KoStyleStack::KoStyleStack()
{
    clear();
}

KoStyleStack::~KoStyleStack()
{
}

void KoStyleStack::clear()
{
    m_stack.clear();
#ifdef DEBUG_STYLESTACK
    kdDebug(30003) << "clear!" << endl;
#endif
}

void KoStyleStack::save()
{
    m_marks.push( m_stack.count() );
#ifdef DEBUG_STYLESTACK
    kdDebug(30003) << "save (level " << m_marks.count() << ") -> index " << m_stack.count() << endl;
#endif
}

void KoStyleStack::restore()
{
    Q_ASSERT( !m_marks.isEmpty() );
    int toIndex = m_marks.pop();
#ifdef DEBUG_STYLESTACK
    kdDebug(30003) << "restore (level " << m_marks.count()+1 << ") -> to index " << toIndex << endl;
#endif
    Q_ASSERT( toIndex > -1 );
    Q_ASSERT( toIndex <= (int)m_stack.count() ); // If equal, nothing to remove. If greater, bug.
    for ( int index = (int)m_stack.count() - 1; index >= toIndex; --index )
        m_stack.pop_back();
}

void KoStyleStack::pop()
{
    Q_ASSERT( !m_stack.isEmpty() );
    m_stack.pop_back();
#ifdef DEBUG_STYLESTACK
    kdDebug(30003) << "pop -> count=" << m_stack.count() << endl;
#endif
}

void KoStyleStack::push( const QDomElement& style )
{
    m_stack.append( style );
#ifdef DEBUG_STYLESTACK
    kdDebug(30003) << "pushed " << style.attribute("style:name") << " -> count=" << m_stack.count() << endl;
#endif
}

bool KoStyleStack::hasAttribute( const QString& name, const QString& detail ) const
{
    QString fullName( name );
    if ( !detail.isEmpty() )
    {
        fullName += '-';
        fullName += detail;
    }
    QValueList<QDomElement>::ConstIterator it = m_stack.end();
    while ( it != m_stack.begin() )
    {
        --it;
        QDomElement properties = (*it).namedItem( m_propertiesTagName ).toElement();
        if ( properties.hasAttribute( name ) ||
             ( !detail.isEmpty() && properties.hasAttribute( fullName ) ) )
            return true;
    }
    return false;
}

QString KoStyleStack::attribute( const QString& name, const QString& detail ) const
{
    QString fullName( name );
    if ( !detail.isEmpty() )
    {
        fullName += '-';
        fullName += detail;
    }
    QValueList<QDomElement>::ConstIterator it = m_stack.end();
    while ( it != m_stack.begin() )
    {
        --it;
        QDomElement properties = (*it).namedItem( m_propertiesTagName ).toElement();
        if ( properties.hasAttribute( name ) )
            return properties.attribute( name );
        if ( !detail.isEmpty() && properties.hasAttribute( fullName ) )
            return properties.attribute( fullName );
    }
    return QString::null;
}

// Font size is a bit special. "115%" applies to "the fontsize of the parent style".
// This can be generalized though (hasAttributeThatCanBePercentOfParent() ? :)
// Although, if we also add support for fo:font-size-rel here then it's not general anymore.
double KoStyleStack::fontSize() const
{
    QString name = "fo:font-size";
    double percent = 1;
    QValueList<QDomElement>::ConstIterator it = m_stack.end();

    while ( it != m_stack.begin() )
    {
        --it;
        QDomElement properties = (*it).namedItem( m_propertiesTagName ).toElement();
        if ( properties.hasAttribute( name ) ) {
            QString value = properties.attribute( name );
            if ( value.endsWith( "%" ) )
                percent *= value.toDouble() / 100.0;
            else
                return percent * KoUnit::parseValue( value ); // e.g. 12pt
        }
    }
    return 0;
}

// TODO: make this namespace-aware (probably needs localName and nsURI in the API)
bool KoStyleStack::hasChildNode(const QString & name) const
{
    QValueList<QDomElement>::ConstIterator it = m_stack.end();
    while ( it != m_stack.begin() )
    {
        --it;
        QDomElement properties = (*it).namedItem( m_propertiesTagName ).toElement();
        if ( !properties.namedItem( name ).isNull() )
            return true;
    }

    return false;
}

// TODO: make this namespace-aware (probably needs localName and nsURI in the API)
QDomNode KoStyleStack::childNode(const QString & name) const
{
    QValueList<QDomElement>::ConstIterator it = m_stack.end();

    while ( it != m_stack.begin() )
    {
        --it;
        QDomElement properties = (*it).namedItem( m_propertiesTagName ).toElement();
        if ( !properties.namedItem( name ).isNull() )
            return properties.namedItem( name );
    }

    return QDomNode();          // a null node
}

static bool isUserStyle( const QDomElement& e )
{
    QDomElement parent = e.parentNode().toElement();
    //kdDebug(30003) << k_funcinfo << "tagName=" << e.tagName() << " parent-tagName=" << parent.tagName() << endl;
    return parent.localName() == "styles" && parent.namespaceURI() == KoXmlNS::office;
}

QString KoStyleStack::userStyleName() const
{
    QValueList<QDomElement>::ConstIterator it = m_stack.end();
    while ( it != m_stack.begin() )
    {
        --it;
        //kdDebug(30003) << k_funcinfo << (*it).attribute("style:name") << endl;
        if ( isUserStyle( *it ) )
            return (*it).attributeNS( KoXmlNS::style, "name", QString::null );
    }
    // Can this ever happen?
    return "Standard";
}

void KoStyleStack::setTypeProperties( const QString& typeProperties )
{
    m_propertiesTagName = typeProperties.isEmpty() ? "style:properties" : ( "style:"+typeProperties+"-properties" );
}
