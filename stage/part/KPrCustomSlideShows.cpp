/* This file is part of the KDE project
   Copyright (C) 2007-2008      Carlos Licea <carlos.licea@kdemail.org>
   Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>

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

//Calligra includes
#include <KoPAPageBase.h>
#include <KoPAPage.h>
#include <KoPALoadingContext.h>
#include <KoPASavingContext.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>
#include <KoXmlReader.h>

//Stage includes
#include <KPrDocument.h>
#include "StageDebug.h"

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
    Q_UNUSED( it ); // only used in the above Q_ASSERT.
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
    QMap< QString, QList<KoPAPageBase*> >::const_iterator it = m_customSlideShows.constFind( name );
    Q_ASSERT( it != m_customSlideShows.constEnd() );
    Q_UNUSED( it ); // only used in the above Q_ASSERT.
    m_customSlideShows.insert( name, slideShow );
    emit updated();
}
void KPrCustomSlideShows::rename( const QString &oldName, const QString &newName )
{
    QMap< QString, QList<KoPAPageBase*> >::const_iterator it = m_customSlideShows.constFind( oldName );
    Q_ASSERT( it !=  m_customSlideShows.constEnd() );
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

KoPAPageBase *KPrCustomSlideShows::pageByIndex(const QString &name, int index) const
{
    QList<KoPAPageBase*> pages = getByName(name);
    return pages.value(index);
}


int KPrCustomSlideShows::indexByPage(const QString &name, KoPAPageBase *page) const
{
    QList<KoPAPageBase*> pages = getByName(name);
    return pages.indexOf(page);
}

void KPrCustomSlideShows::addSlideToAll( KoPAPageBase *page, unsigned int position )
{
    QMap< QString, QList<KoPAPageBase*> >::iterator it = m_customSlideShows.begin();
    //FIXME: should we allow negative index?
    //if( position < 0 ) return;
    while( it != m_customSlideShows.end() ) {
        uint size = it.value().size();
        it.value().insert( (position<=size)? position : size, page );
        ++it;
    }
    emit updated();
}

void KPrCustomSlideShows::addSlidesToAll( const QList<KoPAPageBase*> &slideShow, unsigned int position )
{
    //FIXME: should we allow negative index?
    //if( position < 0 ) return;
    for( int i=0; i < slideShow.size(); ++i ) {
        addSlideToAll( slideShow[i], position + i );
    }
}

void KPrCustomSlideShows::removeSlideFromAll( KoPAPageBase* page )
{
    QMap< QString, QList<KoPAPageBase*> >::iterator it = m_customSlideShows.begin();
    while( it != m_customSlideShows.end() ) {
        it.value().removeAll( page );
        ++it;
    }
    emit updated();
}

void KPrCustomSlideShows::removeSlidesFromAll( const QList<KoPAPageBase*> &slideShow )
{
    for( int i=0; i < slideShow.size(); ++i ) {
        removeSlideFromAll( slideShow[i] );
    }
}

void KPrCustomSlideShows::saveOdf( KoPASavingContext & context )
{
    QMap<QString, QList<KoPAPageBase*> >::ConstIterator it = m_customSlideShows.constBegin();
    const QMap<QString, QList<KoPAPageBase*> >::ConstIterator end = m_customSlideShows.constEnd();
    for (; it != end; ++it) {
        const QString &name = it.key();
        const QList<KoPAPageBase*> &slideList = it.value();

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
            pages.chop(1); //remove the last comma
        }
        context.xmlWriter().addAttribute( "presentation:pages", pages );
        context.xmlWriter().endElement();//presentation:show
    }
}

void KPrCustomSlideShows::loadOdf( const KoXmlElement & presentationSettings, KoPALoadingContext & context )
{
    m_customSlideShows.clear();

    KoXmlElement element;
    forEachElement( element, presentationSettings ) {
        if ( element.tagName() == "show" && element.namespaceURI() == KoXmlNS::presentation ) {
            if ( element.hasAttributeNS( KoXmlNS::presentation, "name" ) && element.hasAttributeNS( KoXmlNS::presentation, "pages" ) ) {
                QString name = element.attributeNS( KoXmlNS::presentation, "name" );
                QString pages = element.attributeNS( KoXmlNS::presentation, "pages" );

                QStringList splitedPages = pages.split( ',' );
                QList<KoPAPageBase*> slideShow;
                foreach (const QString &pageName, splitedPages) {
                    KoPAPage * page = context.pageByName( pageName );
                    if ( page ) {
                        slideShow.append( page );
                    }
                    else {
                        warnStage << "missing attributes is presentation:show";
                    }
                }
                if ( !m_customSlideShows.contains( name ) ) {
                    m_customSlideShows.insert( name, slideShow );
                }
                else {
                    warnStage << "slide show with name" << name << "already existing. It will not be inserted.";
                }
            }
            else {
                warnStage << "missing attributes is presentation:show";
            }
        }
    }
}

QStringList KPrCustomSlideShows::namesByPage(KoPAPageBase *page) const
{
    QMap< QString, QList<KoPAPageBase*> >::ConstIterator it = m_customSlideShows.begin();
    QStringList names;
    while(it != m_customSlideShows.end()) {
        if (it.value().contains(page)) {
            names.append(it.key());
        }
        ++it;
    }
    return names;
}
