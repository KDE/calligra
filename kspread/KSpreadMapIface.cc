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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <dcopclient.h>
#include <kapplication.h>
#include <kdebug.h>

#include "kspread_doc.h"
#include "kspread_map.h"
#include "kspread_sheet.h"

#include "KSpreadMapIface.h"
//Added by qt3to4:
#include <Q3PtrList>

using namespace KSpread;

MapIface::MapIface( Map* map )
    : DCOPObject( map )
{
    m_map = map;
}

DCOPRef MapIface::sheet( const QString& name )
{
    Sheet* t = m_map->findSheet( name );
    if ( !t )
        return DCOPRef();

    return DCOPRef( kapp->dcopClient()->appId(), t->dcopObject()->objId() );
}

DCOPRef MapIface::sheetByIndex( int index )
{
    Sheet* t = m_map->sheetList().at( index );
    if ( !t )
    {
        kDebug(36001) << "+++++ No table found at index " << index << endl;
        return DCOPRef();
    }

    kDebug(36001) << "+++++++ Returning table " << t->QObject::objectName() << endl;

    return DCOPRef( kapp->dcopClient()->appId(), t->dcopObject()->objId() );
}

int MapIface::sheetCount() const
{
    return m_map->count();
}

QStringList MapIface::sheetNames() const
{
    QStringList names;

    Q3PtrList<Sheet>& lst = m_map->sheetList();
    Q3PtrListIterator<Sheet> it( lst );
    for( ; it.current(); ++it )
      names.append( it.current()->objectName() );

    return names;
}

QList<DCOPRef> MapIface::sheets()
{
    QList<DCOPRef> t;

    Q3PtrList<Sheet>& lst = m_map->sheetList();
    Q3PtrListIterator<Sheet> it( lst );
    for( ; it.current(); ++it )
        t.append( DCOPRef( kapp->dcopClient()->appId(), it.current()->dcopObject()->objId() ) );

    return t;
}

DCOPRef MapIface::insertSheet( const QString& name )
{
    if ( m_map->findSheet( name ) )
        return sheet( name );

    Sheet* t = m_map->addNewSheet ();
    t->setSheetName( name );

    return sheet( name );
}

bool MapIface::processDynamic(const DCOPCString &fun, const QByteArray &/*data*/,
                                     DCOPCString& replyType, QByteArray &replyData)
{
    // Does the name follow the pattern "foobar()" ?
    uint len = fun.length();
    if ( len < 3 )
        return false;

    if ( fun[ len - 1 ] != ')' || fun[ len - 2 ] != '(' )
        return false;

    Sheet* t = m_map->findSheet( fun.left( len - 2 ).data() );
    if ( !t )
        return false;

    replyType = "DCOPRef";
    QDataStream out( &replyData,QIODevice::WriteOnly );
    out.setVersion(QDataStream::Qt_3_1);
    out << DCOPRef( kapp->dcopClient()->appId(), t->dcopObject()->objId() );
    return true;
}
