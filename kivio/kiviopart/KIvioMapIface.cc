/* This file is part of the KDE project
   Copyright (C) 2002 Laurent MONTEL <lmontel@mandrakesoft.com>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "KIvioMapIface.h"

#include "kivio_map.h"
#include "kivio_doc.h"
#include "kivio_page.h"

#include <kapplication.h>
#include <dcopclient.h>
#include <kdebug.h>

KIvioMapIface::KIvioMapIface( KivioMap* map )
    : DCOPObject( map )
{
    m_map = map;
}

DCOPRef KIvioMapIface::page( const QString& name )
{
    KivioPage* t = m_map->findPage( name );
    if ( !t )
        return DCOPRef();

    return DCOPRef( kapp->dcopClient()->appId(), t->dcopObject()->objId() );
}

DCOPRef KIvioMapIface::pageByIndex( int index )
{
    KivioPage* t = m_map->pageList().at( index );
    if ( !t )
    {
        kDebug(43000) << "+++++ No page found at index " << index << endl;
        return DCOPRef();
    }

    kDebug(43000) << "+++++++ Returning page " << t->QObject::name() << endl;

    return DCOPRef( kapp->dcopClient()->appId(), t->dcopObject()->objId() );
}

int KIvioMapIface::pageCount() const
{
    return m_map->count();
}

QStringList KIvioMapIface::pageNames() const
{
    QStringList names;

    QPtrList<KivioPage>& lst = m_map->pageList();
    QPtrListIterator<KivioPage> it( lst );
    for( ; it.current(); ++it )
        names.append( it.current()->name() );

    return names;
}

QValueList<DCOPRef> KIvioMapIface::pages()
{
    QValueList<DCOPRef> t;

    QPtrList<KivioPage>& lst = m_map->pageList();
    QPtrListIterator<KivioPage> it( lst );
    for( ; it.current(); ++it )
        t.append( DCOPRef( kapp->dcopClient()->appId(), it.current()->dcopObject()->objId() ) );

    return t;
}

DCOPRef KIvioMapIface::insertPage( const QString& name )
{
    if ( m_map->findPage( name ) )
        return page( name );

    KivioPage* t = new KivioPage( m_map, name );
    t->setPageName( name );
    m_map->doc()->addPage( t );

    return page( name );
}

bool KIvioMapIface::processDynamic(const QCString &fun, const QByteArray &/*data*/,
                                     QCString& replyType, QByteArray &replyData)
{
    // Does the name follow the pattern "foobar()" ?
    uint len = fun.length();
    if ( len < 3 )
        return false;

    if ( fun[ len - 1 ] != ')' || fun[ len - 2 ] != '(' )
        return false;

    KivioPage* t = m_map->findPage( fun.left( len - 2 ).data() );
    if ( !t )
        return false;

    replyType = "DCOPRef";
    QDataStream out( &replyData,QIODevice::WriteOnly );
    out.setVersion(QDataStream::Qt_3_1);
    out << DCOPRef( kapp->dcopClient()->appId(), t->dcopObject()->objId() );
    return true;
}
