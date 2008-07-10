/* This file is part of the KDE project
   Copyright (C) 2007-2008      Carlos Licea <carlos.licea@kdemail.org>

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
#include <QDebug>

KPrCustomSlideShows::KPrCustomSlideShows()
{
}

KPrCustomSlideShows::~KPrCustomSlideShows()
{
}

void KPrCustomSlideShows::insert( const QString &name, const QList<KoPAPageBase*> &slideShow )
{
    QMap< QString, QList<KoPAPageBase*> >::iterator it = m_customSlideShows.find( name );
    Q_ASSERT( it == m_customSlideShows.end() );
    m_customSlideShows.insert( name, slideShow );
}

void KPrCustomSlideShows::remove( const QString &name )
{
    QMap< QString, QList<KoPAPageBase*> >::iterator it = m_customSlideShows.find( name );
    Q_ASSERT( it != m_customSlideShows.end() );
    m_customSlideShows.erase( it );
}

void KPrCustomSlideShows::update( const QString &name, const QList<KoPAPageBase*> &slideShow )
{
    QMap< QString, QList<KoPAPageBase*> >::const_iterator it = m_customSlideShows.find( name );
    Q_ASSERT( it != m_customSlideShows.end() );
    m_customSlideShows.insert( name, slideShow );
}
void KPrCustomSlideShows::rename( const QString &oldName, const QString &newName )
{
    QMap< QString, QList<KoPAPageBase*> >::const_iterator it = m_customSlideShows.find( oldName );
    Q_ASSERT( it !=  m_customSlideShows.end() );
    QList<KoPAPageBase*> value( it.value() );
    remove( oldName );
    insert( newName, value );
}

const QList<QString> KPrCustomSlideShows::names() const
{
    return m_customSlideShows.keys();
}

QList<KoPAPageBase*> KPrCustomSlideShows::getByName( const QString &name ) const
{
    QMap< QString, QList<KoPAPageBase*> >::const_iterator it = m_customSlideShows.constFind( name );
    Q_ASSERT( it !=  m_customSlideShows.constEnd() );
//     if( it == m_customSlideShows.constEnd() ) {
//         return QList<KoPAPageBase*>();
//     }
    qDebug()<<"Found by getByName:"<<it.value();
    return it.value();
}

void KPrCustomSlideShows::addSlideToAll( KoPAPageBase* page, unsigned int position )
{
    QMap< QString, QList<KoPAPageBase*> >::iterator it = m_customSlideShows.begin();
    //FIXME: should we allow negative index?
    //if( position < 0 ) return;
    while( it != m_customSlideShows.constEnd() ) {
        uint size = it.value().size();
        it.value().insert( (position<=size)? position : size, page );
        ++it;
    }
}

void KPrCustomSlideShows::addSlidesToAll( const QList<KoPAPageBase*> &slideShow, unsigned int position )
{
    //FIXME: should we allow negative index?
    //if( position < 0 ) return;;
    for( int i=0; i < slideShow.size(); ++i ) {
        addSlideToAll( slideShow[i], position + i );
    }
}

void KPrCustomSlideShows::removeSlideFromAll( KoPAPageBase* page )
{
    QMap< QString, QList<KoPAPageBase*> >::iterator it = m_customSlideShows.begin();
    while( it != m_customSlideShows.constEnd() ) {
        it.value().removeAll( page );
        ++it;
    }
}

void KPrCustomSlideShows::removeSlidesFromAll( const QList<KoPAPageBase*> &slideShow )
{
    for( int i=0; i < slideShow.size(); ++i ) {
        removeSlideFromAll( slideShow[i] );
    }
}
