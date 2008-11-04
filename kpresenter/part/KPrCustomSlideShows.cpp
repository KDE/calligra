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

#include "KPrCustomSlideShows.h"

//KOffice includes
#include <KoPAPageBase.h>
#include <KoPAPage.h>
#include <KoXmlWriter.h>

//KPresenter includes
#include <KPrDocument.h>

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

void KPrCustomSlideShows::saveOdf( KoPASavingContext & context )
{
    foreach( QString name, m_customSlideShows.keys() ) {
        QList<KoPAPageBase*> slideList = m_customSlideShows.value( name );
        context.xmlWriter().startElement( "presentation:show" );
        context.xmlWriter().addAttribute( "presentation:name", name );
        QString pages;
        foreach( KoPAPageBase* page, slideList ) {
            KoPAPage * p = dynamic_cast<KoPAPage *>( page );
            if ( p ) {
                pages += context.pageName( p ) + ',';
            }
        }
        if( !slideList.isEmpty() ) {
            pages.truncate( pages.size() - 1 );//remove the last comma
        }
        context.xmlWriter().addAttribute( "presentation:pages", pages );
        context.xmlWriter().endElement();//presentation:show
    }
}

void KPrCustomSlideShows::loadOdf( KoXmlElement* presentationSettings, KPrDocument* kprDocument )
{
    m_customSlideShows.clear();

    //Create a map of Name-Address so that we can look for the names much faster
    QMap<QString,KoPAPageBase*> pageNames;
    foreach( KoPAPageBase* page, kprDocument->pages() )
    {
        pageNames.insert( page->name(), page );
    }

    //Now load the customSlidesShows
    QList<QString> usedNames;//used to avoid duplicates, shouldn't happen but we cannot assert it
    KoXmlElement presentationShow = presentationSettings->firstChild().toElement();
    for( ; !presentationShow.isNull(); presentationShow = presentationShow.nextSibling().toElement() )
    {
        QString name = presentationShow.attribute( "name" );
        QString pages = presentationShow.attribute( "pages" );
        QStringList splitedPages = pages.split( ',' );
        QList<KoPAPageBase*> slideShow;
        foreach( QString pageName, splitedPages )
        {
            slideShow.append( pageNames.value( pageName ) );
        }
        if( usedNames.contains( name ) )
        {
            insert( name, slideShow );
        }
        else
        {
            //The item is repeated, just update
            update( name, slideShow );
        }
    }//for
}
