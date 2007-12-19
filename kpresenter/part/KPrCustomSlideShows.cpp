/* This file is part of the KDE project
   Copyright (C) 2007      Carlos Licea <carlos.licea@kdemail.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software itation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software itation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include"KPrCustomSlideShows.h"

KPrCustomSlideShows::KPrCustomSlideShows()
{
}

KPrCustomSlideShows::~KPrCustomSlideShows()
{
}

void KPrCustomSlideShows::insertCustomSlideShow( const QString &name, const QList<KoPAPageBase*> &slideShow )
{
    QMap< QString, QList<KoPAPageBase*> >::iterator it = m_customSlideShows.find( name );
    Q_ASSERT( it == m_customSlideShows.end() );
    m_customSlideShows.insert( name, slideShow );
}

void KPrCustomSlideShows::removeCustomSlideShow( const QString &name )
{
    QMap< QString, QList<KoPAPageBase*> >::iterator it = m_customSlideShows.find( name );
    Q_ASSERT( it != m_customSlideShows.end() );
    m_customSlideShows.erase( it );
}

void KPrCustomSlideShows::updateCustomSlideShow( const QString &name, const QList<KoPAPageBase*> &slideShow )
{
    QMap< QString, QList<KoPAPageBase*> >::const_iterator it = m_customSlideShows.find( name );
    Q_ASSERT( it != m_customSlideShows.end() );
    m_customSlideShows.insert( name, slideShow );
}

const QList<QString> KPrCustomSlideShows::customSlideShowsNames() const
{
    return m_customSlideShows.keys();
}

QList<KoPAPageBase*> KPrCustomSlideShows::getCustomSlideShowByName( const QString &name ) const
{
    QMap< QString, QList<KoPAPageBase*> >::const_iterator it = m_customSlideShows.find( name );
    Q_ASSERT( it !=  m_customSlideShows.end() );
    if( it == m_customSlideShows.end() ) {
        return QList<KoPAPageBase*>();
    }
    return it.value();
}

void KPrCustomSlideShows::addSlideToAllCustomSlideShows( KoPAPageBase* page, unsigned int position )
{
    QMap< QString, QList<KoPAPageBase*> >::iterator it = m_customSlideShows.begin();
    //FIXME: should we allow negative index?
    //if( position < 0 ) return;
    int size;
    while( it != m_customSlideShows.constEnd() ) {
        size = it.value().size();
        it.value().insert( (position<=size)? position : size, page );
        ++it;
    }
}

void KPrCustomSlideShows::addSlidesToAllCustomSlideShows( const QList<KoPAPageBase*> &slideShow, unsigned int position )
{
    //FIXME: should we allow negative index?
    //if( position < 0 ) return;
    for( int i=0; i < slideShow.size(); ++i ) {
        addSlideToAllCustomSlideShows( slideShow[i], position + i );
    }
}

void KPrCustomSlideShows::removeSlideFromAllCustomSlideShows( KoPAPageBase* page )
{
    QMap< QString, QList<KoPAPageBase*> >::iterator it = m_customSlideShows.begin();
    while( it != m_customSlideShows.constEnd() ) {
        it.value().removeAll( page );
        ++it;
    }
}

void KPrCustomSlideShows::removeSlidesFromAllCustomSlideShows( const QList<KoPAPageBase*> &slideShow )
{
    for( int i=0; i < slideShow.size(); ++i ) {
        removeSlideFromAllCustomSlideShows( slideShow[i] );
    }
}
