/* This file is part of the KDE project
   
   Copyright 2002 Ariya Hidayat <ariya@kde.org>
   Copyright 2001 Laurent Montel <montel@kde.org>
   Copyright 2001 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2000 Werner Trobin <trobin@kde.org>
   Copyright 1999-2000 Torben Weis <weis@kde.org>

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

#include "KSpreadMapIface.h"

#include "kspread_map.h"
#include "kspread_doc.h"

#include <kapplication.h>
#include <dcopclient.h>
#include <kdebug.h>

KSpreadMapIface::KSpreadMapIface( KSpreadMap* map )
    : DCOPObject( map )
{
    m_map = map;
}

DCOPRef KSpreadMapIface::sheet( const QString& name )
{
    KSpreadSheet* t = m_map->findSheet( name );
    if ( !t )
        return DCOPRef();

    return DCOPRef( kapp->dcopClient()->appId(), t->dcopObject()->objId() );
}

DCOPRef KSpreadMapIface::sheetByIndex( int index )
{
    KSpreadSheet* t = m_map->sheetList().at( index );
    if ( !t )
    {
        kdDebug(36001) << "+++++ No table found at index " << index << endl;
        return DCOPRef();
    }

    kdDebug(36001) << "+++++++ Returning table " << t->QObject::name() << endl;

    return DCOPRef( kapp->dcopClient()->appId(), t->dcopObject()->objId() );
}

int KSpreadMapIface::sheetCount() const
{
    return m_map->count();
}

QStringList KSpreadMapIface::sheetNames() const
{
    QStringList names;

    QPtrList<KSpreadSheet>& lst = m_map->sheetList();
    QPtrListIterator<KSpreadSheet> it( lst );
    for( ; it.current(); ++it )
        names.append( it.current()->name() );

    return names;
}

QValueList<DCOPRef> KSpreadMapIface::sheets()
{
    QValueList<DCOPRef> t;

    QPtrList<KSpreadSheet>& lst = m_map->sheetList();
    QPtrListIterator<KSpreadSheet> it( lst );
    for( ; it.current(); ++it )
        t.append( DCOPRef( kapp->dcopClient()->appId(), it.current()->dcopObject()->objId() ) );

    return t;
}

DCOPRef KSpreadMapIface::insertSheet( const QString& name )
{
    if ( m_map->findSheet( name ) )
        return sheet( name );

    KSpreadSheet* t = m_map->addNewSheet ();
    t->setSheetName( name );

    return sheet( name );
}

bool KSpreadMapIface::processDynamic(const QCString &fun, const QByteArray &/*data*/,
                                     QCString& replyType, QByteArray &replyData)
{
    // Does the name follow the pattern "foobar()" ?
    uint len = fun.length();
    if ( len < 3 )
        return FALSE;

    if ( fun[ len - 1 ] != ')' || fun[ len - 2 ] != '(' )
        return FALSE;

    KSpreadSheet* t = m_map->findSheet( fun.left( len - 2 ).data() );
    if ( !t )
        return FALSE;

    replyType = "DCOPRef";
    QDataStream out( replyData, IO_WriteOnly );
    out << DCOPRef( kapp->dcopClient()->appId(), t->dcopObject()->objId() );
    return TRUE;
}
