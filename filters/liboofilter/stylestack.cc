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

#include "stylestack.h"
#include <koUnit.h>
#include <kdebug.h>

//#define DEBUG_STYLESTACK

StyleStack::StyleStack()
{
    clear();
}

StyleStack::~StyleStack()
{
}

void StyleStack::clear()
{
    m_stack.clear();
#ifdef DEBUG_STYLESTACK
    kdDebug(30519) << "clear!" << endl;
#endif
}

void StyleStack::save()
{
    m_marks.push( m_stack.count() );
#ifdef DEBUG_STYLESTACK
    kdDebug(30519) << "save (level " << m_marks.count() << ") -> index " << m_stack.count() << endl;
#endif
}

void StyleStack::restore()
{
    Q_ASSERT( !m_marks.isEmpty() );
    int toIndex = m_marks.pop();
#ifdef DEBUG_STYLESTACK
    kdDebug(30519) << "restore (level " << m_marks.count()+1 << ") -> to index " << toIndex << endl;
#endif
    Q_ASSERT( toIndex > -1 );
    Q_ASSERT( toIndex <= (int)m_stack.count() ); // If equal, nothing to remove. If greater, bug.
    for ( int index = (int)m_stack.count() - 1; index >= toIndex; --index )
        m_stack.pop_back();
}

void StyleStack::pop()
{
    m_stack.pop_back();
#ifdef DEBUG_STYLESTACK
    kdDebug(30519) << "pop -> count=" << m_stack.count() << endl;
#endif
}

void StyleStack::push( const QDomElement& style )
{
    m_stack.append( style );
#ifdef DEBUG_STYLESTACK
    kdDebug(30519) << "pushed " << style.attribute("style:name") << " -> count=" << m_stack.count() << endl;
#endif
}

bool StyleStack::hasAttribute( const QString& name ) const
{
    QValueList<QDomElement>::ConstIterator it = m_stack.end();
    while ( it != m_stack.begin() )
    {
        --it;
        QDomElement properties = (*it).namedItem( "style:properties" ).toElement();
        if ( properties.hasAttribute( name ) )
            return true;
    }

    return false;
}

QString StyleStack::attribute( const QString& name ) const
{
    QValueList<QDomElement>::ConstIterator it = m_stack.end();
    while ( it != m_stack.begin() )
    {
        --it;
        QDomElement properties = (*it).namedItem( "style:properties" ).toElement();
        if ( properties.hasAttribute( name ) )
            return properties.attribute( name );
    }

    return QString::null;
}

bool StyleStack::hasAttribute( const QString& name, const QString& detail ) const
{
    QString fullName( name );
    fullName += '-';
    fullName += detail;
    QValueList<QDomElement>::ConstIterator it = m_stack.end();
    while ( it != m_stack.begin() )
    {
        --it;
        QDomElement properties = (*it).namedItem( "style:properties" ).toElement();
        if ( properties.hasAttribute( name ) || properties.hasAttribute( fullName ) )
            return true;
    }

    return false;
}

QString StyleStack::attribute( const QString& name, const QString& detail ) const
{
    QString fullName( name );
    fullName += '-';
    fullName += detail;
    QValueList<QDomElement>::ConstIterator it = m_stack.end();
    while ( it != m_stack.begin() )
    {
        --it;
        QDomElement properties = (*it).namedItem( "style:properties" ).toElement();
        if ( properties.hasAttribute( fullName ) )
            return properties.attribute( fullName );
        if ( properties.hasAttribute( name ) )
            return properties.attribute( name );
    }

    return QString::null;
}

// Font size is a bit special. "115%" applies to "the fontsize of the parent style".
// This can be generalized though (hasAttributeThatCanBePercentOfParent() ? :)
// Although, if we also add support for fo:font-size-rel here then it's not general anymore.
double StyleStack::fontSize() const
{
    QString name = "fo:font-size";
    double percent = 1;
    QValueList<QDomElement>::ConstIterator it = m_stack.end();
    while ( it != m_stack.begin() )
    {
        --it;
        QDomElement properties = (*it).namedItem( "style:properties" ).toElement();
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

bool StyleStack::hasChildNode(const QString & name) const
{
    QValueList<QDomElement>::ConstIterator it = m_stack.end();
    while ( it != m_stack.begin() )
    {
        --it;
        QDomElement properties = (*it).namedItem( "style:properties" ).toElement();
        if ( !properties.namedItem( name ).isNull() )
            return true;
    }

    return false;
}

QDomNode StyleStack::childNode(const QString & name) const
{
    QValueList<QDomElement>::ConstIterator it = m_stack.end();
    while ( it != m_stack.begin() )
    {
        --it;
        QDomElement properties = (*it).namedItem( "style:properties" ).toElement();
        if ( !properties.namedItem( name ).isNull() )
            return properties.namedItem( name );
    }

    return QDomNode();          // a null node
}

static bool isUserStyle( const QDomElement& e )
{
    QDomElement parent = e.parentNode().toElement();
    //kdDebug(30519) << k_funcinfo << "tagName=" << e.tagName() << " parent-tagName=" << parent.tagName() << endl;
    return parent.tagName() == "office:styles";
}

QString StyleStack::userStyleName() const
{
    QValueList<QDomElement>::ConstIterator it = m_stack.end();
    while ( it != m_stack.begin() )
    {
        --it;
        //kdDebug(30519) << k_funcinfo << (*it).attribute("style:name") << endl;
        if ( isUserStyle( *it ) )
            return (*it).attribute("style:name");
    }
    // Can this ever happen?
    return "Standard";
}
